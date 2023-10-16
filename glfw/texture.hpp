#ifndef _GLFW_TEXTURE_DEFINITION_HPP_
    #define _GLFW_TEXTURE_DEFINITION_HPP_

    #include "./glfw.hpp"
    #include "../stb/stb.h"

    #include <memory>
    
    _GLFW_START_

    #ifdef _GLFW_VERTEX_BUFFER_OBJECT_DEFINITION_HPP_
        /** @brief Shorthand name for Texture Buffer Objects. */
        using __tbo = buffer_object;
    #endif

    using byte_t    = char;
    using ubyte_t   = unsigned char;
    
    template <typename T> using ptr_t       = T*;
    template <typename T> using constptr_t  = const T*;
    
    namespace texture
    {
        /**
         * @brief Contains information required by opengl for creating textures. All data is required 
         * unless marked [optional].
        */
        struct image_data
        {
            /** @brief The path to the image file. [optional] */
            const char* path;

            /** @brief The width of the image. */
            int width;
            /** @brief The height of the image. */
            int height;
            /** @brief The number of color channels that the image has. i.e. RGB, RGBA, etc. */
            int color_channels;

            /** @brief The data retrieved after loading the image. */
            ptr_t<ubyte_t> data;
        };

        class texture_object
        {
        public:
            GLuint texture_id;

            void find_free_id() { glGenTextures(1, &texture_id); }

            void bind() { glBindTexture(GL_TEXTURE_2D, texture_id); }
            static void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

            static void set_mapping_for_x_axis(GLint value) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, value); }
            static void set_mapping_for_y_axis(GLint value) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, value); } 

            static void set_min_filter(GLint value) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value); }
            static void set_mag_filter(GLint value) { glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, value); }

            static void set_active_texture_slot(GLenum slot) { glActiveTexture(slot); }

            void create(const image_data& data, GLuint format)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format, GL_UNSIGNED_BYTE, data.data);
            }

            static void generate_mipmaps() { glGenerateMipmap(GL_TEXTURE_2D); }

            ~texture_object() noexcept { glDeleteTextures(1, &texture_id); }
        };
        void reset_texture_slot(GLenum slot)
        {
            glActiveTexture(slot);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        #ifdef _USE_STB_IMAGE_LOADER_
            namespace loader
            {
                /** @brief Helps in loading images. It supports most image formats. */
                class image_loader
                {
                private:
                    image_data _data;

                public:
                    image_loader() = default;
                    image_loader(const char* path) { _data.path = path; }

                    /**
                     * @brief Sets the path to the image to load.
                     * @param path The path to set.
                    */
                    void set_path(const char* path) { _data.path = path; }

                    /** @brief Loads the data from the specified image. */
                    void load_file() 
                    { 
                        _data.data = stbi_load(_data.path, &_data.width, &_data.height, &_data.color_channels, 0); 
                        if(!_data.data) fprintf(stderr, "Error loading image at \"%s\"\n", _data.path);
                    }

                    /** @brief Returns the data retrieved from the specified image. */
                    ptr_t<ubyte_t> get_data() const { return _data.data; }

                    /** @brief Returns the path to the specified image. */
                    const char* get_path() const { return _data.path; }

                    /** @brief Returns the width of the image. */
                    int get_width() const { return _data.width; }
                    /** @brief Return the height of the image. */
                    int get_height() const { return _data.height; }
                    /** @brief Returns the number of color channels in the image.*/
                    int get_channels() const { return _data.color_channels; }

                    /** @brief Returns the image data. It will only return the proper data after the image has been loaded. */
                    const image_data& get_image_data() const { return _data; }

                    /**
                     * @brief The destructor, it will be called when any instance of this class goes out of scope. Or
                     * it can be called manually to destroy or free the resources hogged by this class for storing
                     * the data of the image.
                    */
                    ~image_loader() noexcept { stbi_image_free(_data.data); }
                };

                /**
                 * @brief Gets an image loader for the specified image.
                 * @param path The path to the image. 
                 * @returns The pointer to the loader which has been heap allocated.
                */
                std::unique_ptr<image_loader> get_image_loader(const char* path)
                {
                    return std::unique_ptr<image_loader>(new image_loader(path));
                }

                /**
                 * @brief Loads the image data using the specified image loader.
                 * @param loader A reference to the image loader to use for loading the image data. 
                */
                void load_image(image_loader& loader)
                {
                    loader.load_file();
                }

                /**
                 * @brief Gets the image data from the specified image loader. It is assumed that the loader has 
                 * already loaded the image.
                 * @param loader A const reference to the image loader. (Nothing will be modified)
                 * @returns The pointer to the image data which has been heap allocated.
                */
                std::unique_ptr<image_data> get_image_data_from_loader(const image_loader& loader)
                {
                    return std::unique_ptr<image_data>(
                        new image_data {
                            loader.get_path(),
                            loader.get_width(),
                            loader.get_height(),
                            loader.get_channels(),
                            loader.get_data()
                        }
                    );
                }
            }
        #endif
        
    } // namespace texture

    

    _GLFW_END_

#endif