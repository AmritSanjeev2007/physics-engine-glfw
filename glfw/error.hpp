#ifndef _GLFW_ERRORS_DEFINTION_HPP_
    #define _GLFW_ERRORS_DEFINTION_HPP_

    #include <string>

    #include "./glfw.hpp"

    #include "stdio.h"

    _GLFW_START_

    /** @brief The namespace that contains the standard error class and other error related stuff. */
    namespace error
    { 
        /** @brief A list of all the error messages. Range:[0,9) */
        constexpr const char* errors[] = 
        {
            "GL_NO_ERROR",
            "GL_INVALID_ENUM",
            "GL_INVALID_VALUE",
            "GL_INVALID_OPERATION",
            "GL_STACK_OVERFLOW",
            "GL_STACK_UNDERFLOW",
            "GL_OUT_OF_MEMORY",
            "GL_INVALID_FRAMEBUFFER_OPERATION",
            "GL_CONTEXT_LOST"
        };

        /** @brief Converts an OpenGL error code into a readable string. */
        #define CONVERT_GL_ERRCODE_TO_STRING(errcode) (errors[errcode==0?0:errcode-(GL_INVALID_ENUM-1)])

        /** @brief The base error class. Other error classes can derive from this class. */
        struct error_base 
        { 
        private:
            GLenum _code = 0; 
            std::string _msg; 

        public:
            error_base()=default;
            error_base(const char* msg, GLenum code)
                : _msg(msg), _code(code) {}

            /** @brief The contents of the error. */
            const char* what() const { return _msg.c_str(); } 
            /** @brief The error code associated with the error. */
            const GLenum code() const { return _code; }

            /** 
             * @brief Allows you to reuse this class to display another error. 
             * @param msg The new error message to set.
             * @param code The new error code to set.
            */
            void reuse(const char* msg, GLenum code)
            {
                _msg=msg;
                _code=code; 
            }

            /** @brief Prints the error message to stdout (The standard console). */
            void print() const { printf("errmsg:%s,\nerrcode:%u\n",_msg.c_str(),_code); }
        };

        using gl_error = error_base;

        /** @brief Gets the last error on opengl's error stack. Side effects: It will pop the error off the stack so 
         * that it wont be accessible after calling this function. */
        gl_error get_last_error()
        {
            GLenum errcode = glGetError();
            return gl_error {
                CONVERT_GL_ERRCODE_TO_STRING(errcode),
                errcode
            };
        }

        using ERROR_READING_FUNCTION = void(*)(const gl_error&);

        /** 
         * @brief Executes the specified callback function for every error on the error stack. 
         * @param func The callback function to execute. The function is: void (const gl_error&).
        */
        void read_error_stack(ERROR_READING_FUNCTION func)
        {
            GLenum errcode;
            gl_error err;
            while((errcode = glGetError()) != GL_NO_ERROR)
            {
                err.reuse(CONVERT_GL_ERRCODE_TO_STRING(errcode),errcode);
                func(err);
            }
        }

    }; // namespace error

    _GLFW_END_

#endif