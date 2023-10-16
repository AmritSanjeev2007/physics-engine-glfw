#ifndef _GLFW_SHADER_DEFINITION_HPP_
    #define _GLFW_SHADER_DEFINITION_HPP_

    #include "./glfw.hpp"

    #include <fstream>
    #include <filesystem>

    #include <thread>

    _GLFW_START_

    namespace shader
    {

    /// @brief A struct which represents the paths(filenames) of the vertex and fragment shaders.
    struct shader_paths 
    { 
        const char *vertex_shader = nullptr;
        const char *fragment_shader = nullptr; 
    };
    /// @brief A struct which contains the contents of the vertex and fragment shaders. i.e. the actual stuff that will get compiled into a shader.
    struct shader_contents 
    { 
        char *vertex_shader_content = nullptr; 
        char *fragment_shader_content = nullptr; 
    };

    /// @brief Represents a null path or in other words, a path to nothing.
    constexpr shader_paths null_shader_path = {};
    #define NULL_SHADER_PATH null_shader_path
    
    /// @brief Pointer comparison
    constexpr bool operator==(const shader_paths& left, const shader_paths& right)
    { 
        return left.vertex_shader == right.vertex_shader && left.fragment_shader == right.fragment_shader; 
    }


    class shader_program
    {
    private:
        shader_paths paths;
        shader_contents contents;

        /// @brief Loades the file contents of the shaders into memeory.
        /// @param vsc A reference to the variable where the content of the vertex shader will be set.
        /// @param fsc A reference to the variable where the content of the fragment shader will be set.
        /// @param vpth The path to the file where the vertex shader is present.
        /// @param fpth The path to the file where the fragment shader is present.
        static void __loader_func(char *&vsc, char*&fsc, const char* vpth, const char* fpth)
        {
            std::fstream vertfile(vpth);
            size_t vertsz = std::filesystem::file_size(vpth);
            vsc = new char[vertsz];
            vertfile.get(vsc, vertsz,'\0'/* delim only works if the file read is a text file (not binary serialized). */);

            std::fstream fragfile(fpth);
            size_t fragsz = std::filesystem::file_size(fpth);
            fsc = new char[fragsz];
            fragfile.get(fsc, fragsz,'\0');
        }

        uint8_t _shader_state = static_cast<uint8_t>(shader_state::EMPTY);

    public:
        /// @brief Sets the filenames where the vertex and fragment shader will be found.
        /// @param paths The filenames (preferrably the full path). If the passed in value is NULL_SHADER_PATH then call the set_content nethod to set your won content for the vertex and fragment shaders.
        void set_path(const shader_paths& paths) noexcept(true)
        { 
            this->paths = paths; // copy
            _shader_state = static_cast<uint8_t>(shader_state::FOUND_PATH); 
        }

        template <bool async=false>
        void load_file()
        {
            if constexpr (async) // Loades the file in a different thread, concurrently.
            {
                std::thread thread_loader = 
                    std::thread(
                        __loader_func,                                  // Function
                            std::ref(contents.vertex_shader_content),   // Params
                            std::ref(contents.fragment_shader_content), // Params
                            paths.vertex_shader,                        // Params
                            paths.fragment_shader                       // Params
                    );
                thread_loader.detach(); // Is this safe?
            } 
            else 
            {
                __loader_func(                                  // Syncronous
                    std::ref(contents.vertex_shader_content),   // Params
                    std::ref(contents.fragment_shader_content), // Params
                    paths.vertex_shader,                        // Params
                    paths.fragment_shader                       // Params
                );                     
                return;
            }
        }

        /// @brief This function should only be called when no file has been provided for getting the vertex and fragment shaders from.
        /// @param vertex_shader_content The content to set for the vertex shader.
        /// @param fragment_shader_content The content to set for the fragment shader.
        void set_content(char* vertex_shader_content, char* fragment_shader_content) noexcept(true)
        {
            this->contents.vertex_shader_content = vertex_shader_content;
            this->contents.fragment_shader_content = fragment_shader_content;

            _shader_state = static_cast<uint8_t>(shader_state::CONTENT);
        }

    private:
        GLuint vertex_shader_id;
        GLuint fragment_shader_id;
        
    public:
        /// @brief The id of the shader program.
        GLuint shader_id = 0;

        /// @brief Returns the id of the shader program.
        const GLuint get_id() const noexcept(true) { return shader_id; }
        
        /// @brief Loads the actual compiled shader into memory.
        void compile_shader()
        {
            vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex_shader_id, 1, &contents.vertex_shader_content, NULL);
            glCompileShader(vertex_shader_id);

            fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment_shader_id, 1, &contents.fragment_shader_content, NULL);
            glCompileShader(fragment_shader_id);

            _shader_state = static_cast<uint8_t>(shader_state::COMPILED);
        }

        /// @brief Returns the id of the vertex shader. The vertex shader will be deleted after the shader program has been linked i.e. after link_shader( ) has been called. 
        const GLuint get_vertex_shader_id() const noexcept(true) { return vertex_shader_id; }
        /// @brief Returns the id of the fragment shader. The fragment shader will be deleted after the shader program has been linked i.e. after link_shader( ) has been called. 
        const GLuint get_fragment_shader_id() const noexcept(true) { return fragment_shader_id; }

        /// @brief Finally links the shaders to opengl so that they can be used later.
        void link_shader()
        {
            shader_id = glCreateProgram();
            glAttachShader(shader_id, vertex_shader_id);
            glAttachShader(shader_id, fragment_shader_id);

            glLinkProgram(shader_id);

            // We dont need these shaders anymore as they have already been linked.
                glDeleteShader(vertex_shader_id); 
                glDeleteShader(fragment_shader_id);

            _shader_state = static_cast<uint8_t>(shader_state::LINKED);
        }
        
        /// @brief Uses the shader program while rendering. i.e. makes this the current active shader program.
        void use_shader() noexcept(true) { glUseProgram(shader_id); }
        
        /**
         * @brief Automatically deletes the shader program after any instance of this class is deleted or goes out 
            of scope. Hence, it is beteer to heap allocate any instances of this class. Call this function manually 
            to destroy the shader.
        */ 
        ~shader_program() noexcept(true) 
        { 
            glDeleteProgram(shader_id); 
            _shader_state = static_cast<uint8_t>(shader_state::DESTROYED);
        }

        /// @brief The various states a shader can be in.
        enum class shader_state : uint8_t
        {
            /// @brief It has no path and has no content.
            EMPTY       = 0,
            /// @brief A path has been specified for the vertex and fragment shaders. This state however is not necessarily reached all the time.
            FOUND_PATH  = 1,
            /// @brief The shader has loaded the content.
            CONTENT     = 2,
            /// @brief The contents of the shader have been compiled.
            COMPILED    = 3,
            /// @brief The compiled shader has been linked with opengl and can be used for rendering.
            LINKED      = 4,
            /// @brief The shader has been destroyed.
            DESTROYED   = 5
        };
        /// @brief Returns the state of the shader.
        shader_state get_shader_state() const noexcept(true) { return shader_state(_shader_state); }
        /// @brief Returns the paths of the vertex and fragment shader files.
        const shader_paths& get_shader_paths() const noexcept(true) { return this->paths; }
        /// @brief Returns the contents loaded from the vertex and shader files.
        const shader_contents& get_shader_contents() const noexcept(true) { return this->contents; }

        /**
         * @brief Returns the address of the uniform variable in the shader. So taht it can be modified in our 
         * actual program.
         * @param name The name of the variable to search for.
        */ 
        GLuint get_uniform_location(const char* name) const noexcept(true)
        {
            return glGetUniformLocation(this->shader_id, name);
        }
    };

    /// @brief A namespace that helps with loading shaders quickly and efficiently.
    namespace loader
    {
        /// @brief Creates a shader and sets the paths of the shader to the specified value.
        /// @param paths The paths of the vertex and fragment shaders.
        /// @return A unique pointer to the memory where the shader is heap allocated.
        std::unique_ptr<shader_program> get_shader(const shader_paths& paths)
        {
            shader_program* sh = new shader_program();
            sh->set_path(paths);

            return std::unique_ptr<shader_program>(sh);
        }

        /// @brief Creates mutiple shaders and sets each and every one of their paths to the specified paths.
        /// @param paths A vector of paths to set for the shaders.
        /// @return Returns a vector of these shaders.
        std::vector<shader_program> get_multiple_shaders(const std::vector<shader_paths>& paths)
        {
            std::vector<shader_program> _hRes = std::vector<shader_program>(paths.size());
            for(int i = 0; i < paths.size(); i++)
            {
                _hRes[i].set_path(paths[i]);
            }
            return _hRes; // Hoping for move eliding.
        }

        /// @brief Loads the contents of the shader.
        /// @param shader A reference to the shader. (The shader will be modified.)
        void load_shader_contents(shader_program& shader) { shader.load_file<false>(); }
        /// @brief Loads the contents of the shader asyncronously i.e. in a different thread.
        /// @param shader A reference to the shader. (The shader will be modified.)
        void load_shader_contents_async(shader_program& shader) { shader.load_file<true>(); }

        /// @brief Loads multiple shaders at once. The program is paused until all the shaders have been loaded.
        /// @param shaders A reference to a vector of shaders. 
        void load_multiple_shader_contents(std::vector<shader_program>& shaders)
        {
            for(shader_program& shader : shaders)
            {
                shader.load_file<false>();
            }
        }
        /// @brief Loads multiple shaders at once. The program will continue to run even while the shaders are being loaded.
        /// @param shaders A reference to a vector of shaders. 
        void load_multiple_shader_contents_async(std::vector<shader_program>& shaders)
        {
            for(shader_program& shader : shaders)
            {
                shader.load_file<true>();
            }
        }

        /**
         * @brief Attempts to fully load and link the shader program. That is, it does every step such as loading 
            the file, compiling the shader and finally linking the shader. This function assumes that the paths are 
            already set and if not uses the specified contents of the vertex and fragment shaders.
         * @param shader A reference to the shader program.
         * @param async_load? Whether or not the files should be loaded asynchronously. False by default.
        */
        void full_load_shader(shader_program& shader, bool async_load=false)
        {
            if(!(shader.get_shader_paths() == NULL_SHADER_PATH))
            {
                async_load?shader.load_file<true>():shader.load_file<false>();
            } 
            shader.compile_shader();
            shader.link_shader();
        }
    }

    /// @brief A namespace that makes it easier to compile shaders and check any problems that would have occurred during the compilation.
    namespace compiler
    {
        /// @brief Compiles the shader and links it to memory.
        /// @param shader A reference to the shader.
        void compile_shader(shader_program& shader) { shader.link_shader(); }

        /// @brief Compiles all the shaders and links them into memory.
        /// @param shaders A vector of shaders.
        void compile_shaders(std::vector<shader_program>& shaders) 
        {
            for(shader_program& shader : shaders)
            {
                shader.link_shader();
            }
        }

        /// @brief Prints the compilation status of the vertex and fragment shaders. This should be called before the shader program has been linked as the compiled log exist only until that point.
        /// @param shader A const reference to the shader program. (Nothing will be modified!)
        void log_compilation_status(const shader_program& shader)
        {
            GLint param;

            // ============================= VERTEX SHADER =============================
            glGetShaderiv(shader.get_vertex_shader_id(), GL_COMPILE_STATUS, &param);
            if(param == GL_FALSE)
            {
                printf("SHADER_COMPILATION_ERROR for vertex shader%c",'\n'); // Avoid flushing

                glGetShaderiv(shader.get_vertex_shader_id(), GL_INFO_LOG_LENGTH, &param);

                char* infoLog = new char[param];
                glGetShaderInfoLog(shader.get_vertex_shader_id(), param, NULL, infoLog);
                printf("INFO_LOG:%c%s\n",'\n',infoLog);
                delete[] infoLog;

            }else printf("SHADER_STATUS for vertex shader: GOOD\n");

            // ============================= FRAGMENT SHADER =============================

            glGetShaderiv(shader.get_fragment_shader_id(), GL_COMPILE_STATUS, &param);
            if(param == GL_FALSE)
            {
                printf("SHADER_COMPILATION_ERROR for fragment shader%c",'\n'); // Avoid flushing

                glGetShaderiv(shader.get_fragment_shader_id(), GL_INFO_LOG_LENGTH, &param);

                char* infoLog = new char[param];
                glGetShaderInfoLog(shader.get_fragment_shader_id(), param, NULL, infoLog);
                printf("INFO_LOG:%c%s\n",'\n',infoLog);
                delete[] infoLog;

            }else printf("SHADER_STATUS for fragment shader: GOOD\n");
        }

        /// @brief Logs the compilation status of the vertex and fragment shaders to the specified file. This should be called before the shader program has been linked as the compiled log exist only until that point.
        /// @param shader A const reference to the shader program. (Nothing will be modified!)
        /// @param file_or_console The contents of the log will be written to the specified file or console[=stdout,stderr,etc].
        void log_compilation_status(const shader_program& shader, std::ostream& file_or_console)
        {
            GLint param;

            // ============================= VERTEX SHADER =============================
            glGetShaderiv(shader.get_vertex_shader_id(), GL_COMPILE_STATUS, &param);
            if(param == GL_FALSE)
            {
                file_or_console << "SHADER_COMPILATION_ERROR for vertex shader\n"; // Avoid flushing

                glGetShaderiv(shader.get_vertex_shader_id(), GL_INFO_LOG_LENGTH, &param);

                char* infoLog = new char[param];
                glGetShaderInfoLog(shader.get_vertex_shader_id(), param, NULL, infoLog);
                file_or_console << "INFO_LOG:\n" << infoLog << "\n";
                delete[] infoLog;

            }else file_or_console << "SHADER_STATUS for vertex shader: GOOD\n";

            // ============================= FRAGMENT SHADER =============================

            glGetShaderiv(shader.get_fragment_shader_id(), GL_COMPILE_STATUS, &param);
            if(param == GL_FALSE)
            {
                file_or_console << "SHADER_COMPILATION_ERROR for fragment shader\n"; // Avoid flushing

                glGetShaderiv(shader.get_fragment_shader_id(), GL_INFO_LOG_LENGTH, &param);

                char* infoLog = new char[param];
                glGetShaderInfoLog(shader.get_fragment_shader_id(), param, NULL, infoLog);
                file_or_console << "INFO_LOG:\n" << infoLog << std::endl;
                delete[] infoLog;

            }else file_or_console << "SHADER_STATUS for fragment shader: GOOD" << std::endl;
        }
    };

    }; // namespace shader

    _GLFW_END_

    #undef PRIVATE

#endif