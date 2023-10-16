#ifndef _GLFW_VERTEX_ARRAY_OBJECT_DEFINITION_HPP_
    #define _GLFW_VERTEX_ARRAY_OBJECT_DEFINITION_HPP_

    #include "./glfw.hpp"


    _GLFW_START_

        class buffer_object; // Opaque definition

        class vertex_array_object
        {
        public:
            /// @brief The id of the array object. It is used for referencing and identifying this specific array object. (It is unique!)
            GLuint array_id;

            /// @brief Finds an unused and unique id for this buffer object.
            void find_free_id() { glGenVertexArrays(1, &array_id); }

            /// @brief Binds the array object and makes it the current active one. This also binds all the buffers assigned to this vertex array object.
            void bind() { glBindVertexArray(array_id); }

            #ifdef _GLFW_VERTEX_BUFFER_OBJECT_DEFINITION_HPP_
                /// @brief Binds the buffer to the vertex array object. This only has to be called once per buffer object.
                /// @param buffer A reference to the buffer object. (State will be modified)
                void bind_buffer(buffer_object& buffer)
                {
                    buffer.bind();
                }
            #endif

            /// @brief Unbinds the array object.
            static void unbind() { glBindVertexArray(0); }
            
            /** 
             * @brief Creates an attribute pointer that tells opengl what this piece of data does. Attributes 
             * are disabled by default.
             * @param index The index of the attribute.
             * @param count The number of elements in the attribute. [1<=count<=4]
             * @param type The type of the data in the attribute. ex; float, uint, etc.
             * @param normalized Whether or not to convert to fixed point values.
             * @param size The total size(in bytes) of the attribute. (Or in other words the distance between any 
             * two attributes.)
             * @param offset The offset of the attribute compared to the data.  
            */
            void create_attribute(GLuint index, GLint count, GLenum type, bool normalized, size_t size, const void* offset)
            {
                glVertexAttribPointer(index, count, type, normalized, size, offset);
            }

            /// @brief Enables the specified attribute.
            /// @param index The same index supplied during a call to "create_attribute".
            void enable_attribute(GLuint index) { glEnableVertexAttribArray(index); }
            /// @brief Disables the specified attribute.
            /// @param index The same index supplied during a call to "create_attribute".
            void disable_attribute(GLuint index) { glDisableVertexAttribArray(index); }

            /**
             * @brief Draws the specified type of elements using the vertices(from a bounded buffer object) and
             * using the order provided by the index buffer object.
             * @param type The type of figure to draw. ex; point, line, triangle, etc.
             * @param number_of_elements The number of indices in the array .
             * @param type_of_indices The type of the data in the indices array. i.e. uint, int, ubyte, etc.
             * @param offset? If no IBO has been bound to the VAO then the location to where the IBO is present 
             *  will be required, if otherwise then specify the offset of indices for this render. 
            */
            void draw_elements(GLenum type, GLsizei number_of_elements, GLenum type_of_indices, const void* offset=0)
            {
                glDrawElements(type, number_of_elements, type_of_indices, offset);
            }

            /// @brief Deletes the array object after this instance of the class goes out of scope or is deleted. Hence, it is better to heap allocate instances of this class.
            ~vertex_array_object() noexcept(true) { glDeleteVertexArrays(1, &array_id); }
        };

        /// @brief Shorthand name for vertex buffer objects.
        using __vao = vertex_array_object;

    _GLFW_END_

#endif