/**
 * This file contains the definition and implementation of the buffer_object class.
 * 
 * THis class is used to store data about opengl objects. Such as the vertices of the object,
 * the color of the object, the order in which the vertices have to be interpreted, etc.
 * 
 * This class by default ships with two type definitions for the buffer_object class;
 *  typename __vbo,
 *  typename __ibo
 * 
 * They are just defined as follows;
 *  using __vbo = buffer_object;
 *  using __ibo = buffer_object;
 * 
 * THey are just renames of the buffer_object class. Hence, they themselves aren't significant.
 * 
 * Using the above example, you can also rename the buffer_object class.
 * 
 * The texture.hpp file also ships with the __tbo rename of the buffer_object class.
 * 
 * What these names stand for;
 *  - __vbo : Vertex Buffer Object
 *  - __ibo : Index Buffer Object
 *  - __tbo : Texture Buffer Object
 * 
 * Refer to this link for more information about the different types of buffer objects.
 *  @link https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml
*/

#ifndef _GLFW_VERTEX_BUFFER_OBJECT_DEFINITION_HPP_
    #define _GLFW_VERTEX_BUFFER_OBJECT_DEFINITION_HPP_

    #include "./glfw.hpp"

    
    _GLFW_START_
    
    class buffer_object
    {
    public:
        /// @brief The id of the buffer object. It is used for referencing and identifying this specific buffer object. (It is unique!)
        GLuint buffer_id;
        /// @brief The type of the buffer object.
        GLenum buffer_type;

        /// @brief Returns the id of the buffer object.
        GLuint get_id() const { return buffer_id; }
        /// @brief Returns the type of the buffer object.
        GLenum get_type() const { return buffer_type; }

        /// @brief Sets the type of the buffer object.
        /// @param target The type to set. ex; vertex buffers, index buffers, etc.
        void set_type(GLenum target) { buffer_type = target; }

        /// @brief Finds an unused and unique id for this buffer object.
        void find_free_id() { glGenBuffers(1, &buffer_id); }

        /// @brief Binds this buffer to being the active buffer.
        void bind() { glBindBuffer(buffer_type, buffer_id); }
        /// @brief Unbinds this buffer from being the active buffer.
        void unbind() { glBindBuffer(buffer_type, 0); }

        /// @brief Creates the buffer object.
        /// @tparam T The type of the data. Should be an arithmetic type!
        /// @param data The data. If it is null, then a buffer with uninitialized data of the specified size will be created.
        /// @param size The number of elements in the data.
        /// @param usage How and How much it will be used.
        template <typename T>
        void create(const T* data, size_t size, GLenum usage)
        { 
            glBufferData(buffer_type, sizeof(T)*size/*byte-sized*/, data, usage); 
        }

        /// @brief Deletes the buffer after this instance of the class goes out of scope or is deleted. Hence, it is better to heap allocate instances of this class.
        ~buffer_object() noexcept { glDeleteBuffers(1, &buffer_id); }
    };

    /// @brief Shorthand name for Vertex Buffer Objects.
    using __vbo = buffer_object;
    /// @brief Shorthand name for Index Buffer Objects.
    using __ibo = buffer_object;

    _GLFW_END_

#endif