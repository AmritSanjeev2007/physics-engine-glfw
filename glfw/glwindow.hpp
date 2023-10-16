/**
 * This header simplifies and abstracts the process of creating and 
 * managing windows using the GLFW library/framework.
 * 
 * 
 * Refer to this site for a detailed documentation;
 * 
 * Window reference: @link https://www.glfw.org/docs/3.3/group__window.html#ga7d9c8c62384b1e2821c4dc48952d2033
 * Window hints: @link https://www.glfw.org/docs/3.3/window_guide.html#window_hints 
 * 
 * The monitor abstraction is yet to be defined. 
 * This link has a detailed documentation; @link https://www.glfw.org/docs/3.3/monitor_guide.html#monitor_monitors
*/

#include "./glfw.hpp"
#include "./time.hpp"

#ifndef _GLFW_WINDOW_DEFINITION_HPP_
    #define _GLFW_WINDOW_DEFINITION_HPP_

    _GLFW_START_

    struct normalized_color { float r,g,b; };
    struct normalized_color_alpha { float r,g,b,a; };
    
    /// @brief For listening for window states and events.
    struct gl_window_listener
    {
    private:
        GLFWwindow* context = nullptr;
        void set_window_context(GLFWwindow* context) {this->context=context;};

        friend class gl_window;

    public:
        /// @brief Whether or not the window has recieved a close event.
        bool should_close() { return glfwWindowShouldClose(this->context); }
    };

    /// @brief The default class for managing and handling GLFW windows.
    class gl_window
    {
    public:
        using renderer_function_type = void();
    
    private:
        renderer_function_type* _renderer = nullptr;

    public:

        /// @brief Sets some settings for the window. These settings will be applied when the window is created (not after).
        /// @param hint The hint (ex; resizable?, visible?, etc)
        /// @param value The corresponding value for the hint.
        static void set_hint(int hint, int value) { glfwWindowHint(hint, value); }
            /** 
             * Some hints not only affect the window but can also affect the context and how 
             * stuff will be rendered on the window.
            */
        
        /// @brief The context of the window. We can use this to access the window even if we don't have this class.
        GLFWwindow* context = nullptr;

        /// @brief Creates the window. The window will use all the hints that have been set before the window was created.
        /// @param width The width of the window.
        /// @param height The height of the window.
        /// @param title The title of the window.
        /// @param monitor The monitor to use for displaying the window. If no monitor is specified, the window will be displayed in windowed mode.
        /// @param share Another window with whom it will share it's resources with. Set to NULL if no sharing.
        void create(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
        { 
            this->context = glfwCreateWindow(width,height,title,monitor,share); 
            event_listener.set_window_context(this->context);
        }

        /// @brief Makes the context of this window the current context of the calling thread.
        void make_context_current() { glfwMakeContextCurrent(this->context); }

        /// @brief Listens for window states and events.
        gl_window_listener event_listener;

        /// @brief Swaps the front and back buffers.
        void swap_buffers() { glfwSwapBuffers(this->context); }

        /// @brief Processes all the events sent to the window. ex; resize, move, etc.
        void handle_events() { glfwPollEvents(); }
        
        /// @brief Destroys this window.
        void destroy() { glfwDestroyWindow(this->context); }
        /// @brief Automatically destroyes the window after this instance of the class has gone out of scope. Hence, it is better to heap allocate any instances of this class.
        ~gl_window() noexcept { glfwDestroyWindow(this->context); }

        /**
         * @brief Sets the initial viewport for the window.
         * 
         * @param x The x-coordinate of the window
         * @param y The y-coordinate of the window
         * @param w The width of the window
         * @param h The height of the window
        */
        static void set_viewport(int x, int y, int w, int h)
        {
            glViewport(x, y, w, h);
        }

        /**
         * @brief Sets the color that will be used after the window's bitplane is being cleared.
         * 
         * @param color The color to set.
        */
        static void set_clear_color(const normalized_color& color)
        {
            glClearColor(color.r, color.g, color.b, 1.0f);
        }
        static void set_clear_color(const normalized_color_alpha& color)
        {
            glClearColor(color.r, color.g, color.b, color.a);
        }

        /// @brief Clears the screen of the window.
        static void clear_screen()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }

        /**
         * @brief Sets the renderer that the window will use to render the graphics.
         * @param fn The function that will be used to render the graphics. i.e. put the graphics
         * code in the function which is to be passed as the parameter.
        */
        void set_renderer(renderer_function_type fn) noexcept(true)
        {
            this->_renderer = fn;
        }

        /** @brief Calls the rendered that is attached to this window. */
        void render() const noexcept(true)
        {
            long double start_of_renderer = glfw::time::now();
            if(this->_renderer) this->_renderer();
            glfw::time::deltaTime = glfw::time::now() - start_of_renderer;
        }
    };  

    /**
     * @brief Gets the width and height of the window.
     * 
     * @param window A reference to the window.
     * @param w A reference to the width variable to set.
     * @param h A reference to the height variable to set.
    */
    void get_window_size(const gl_window& window, int& w, int& h)
    {
        glfwGetWindowSize(window.context, &w, &h);
    }
    /**
     * @brief Sets the width and height of the window.
     * 
     * @param window A reference to the window.
     * @param w The width to set.
     * @param h The height to set.
    */
    void set_window_size(const gl_window& window, int w, int h)
    {
        glfwSetWindowSize(window.context, w, h);
    }

    /**
     * @brief Calculates the aspect-ratio of the specified window. Aspect-Ratio
     * is in the form @c w/h Where @c w is the window width and @c h is the window height.
    */
    long double aspect_ratio(const gl_window& window)
    {
        int w; int h; get_window_size(window, w, h);
        return static_cast<long double>(w)/h;
    }
    void aspect_ratio(const gl_window& window, int& w, int& h)
    {
        get_window_size(window, w, h);
    }

    _GLFW_END_

#endif