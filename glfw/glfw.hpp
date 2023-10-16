/**
 * The default header that every file will have to include.
 * 
 * This for making it easier to refactor later on.
*/

#ifndef _GLFW_DEFINITION_HPP_
    #define _GLFW_DEFINITION_HPP_

    #define GLEW_STATIC
        #include <glad/glad.h>
    #include <GLFW/glfw3.h>

    /// @brief The namespace that will contain all the abstractions and simplications for the opengl functions.
    namespace glfw 
    {
        /// @brief Initialize GLFW library so that we can use all of it's functions.
        void init() { glfwInit(); }
        /// @brief Terminate GLFW library to free resources.
        void terminate() { glfwTerminate(); }

        /** 
         * @brief Initialize GLAD library to use all of it's functions. Should be called after the window
         * has been created and it's context has been made current. i.e. after a call of
         * @c glfw::gl_window::make_context_current
        */
        void init_glad() { gladLoadGL(); }
    };

    #define _GLFW_START_ namespace glfw {
    #define _GLFW_END_ };

#endif