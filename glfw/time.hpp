/**
 * Please try to keep this file independent of any other files in this project.
*/

#ifndef _GLFW_TIME_DEFINITION_HPP_
    #define _GLFW_TIME_DEFINITION_HPP_

    #include "glfw.hpp"

    #include "string"
    #include "thread"

    #include "ctime"
    #include "algorithm"

    _GLFW_START_

    namespace time
    {
        /** @brief The quantity that measures the time taken for the previous frame to finish
         * rendering. This quantity is measured in (seconds).
        */
        long double deltaTime = .0L;

        
        /** @brief Returns the current time in seconds. */
        long double now() noexcept(true)
        {
            return std::chrono::high_resolution_clock::now().time_since_epoch().count() / std::chrono::nanoseconds::period::den;
        }

        /**
         * @brief Halts the execution of the current thread for the specified amount of time.
         * @param sec The amount of time to sleep in seconds.
        */
        void sleep(long double sec) noexcept(true)
        {   
            std::this_thread::sleep_for(
                std::chrono::nanoseconds(
                    static_cast<uint64_t>(
                        sec * std::chrono::nanoseconds::period::den
                    )
                )
            );
        }
    }

    _GLFW_END_

#endif