#pragma once

#include <GLAD/glad/glad.h>

class VBO {
public:

    VBO();

    // Delete copy constructor and copy assignment operators
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    // Move constructor
    VBO(VBO&& other) noexcept;

    // Move assignment operator
    VBO& operator=(VBO&& other) noexcept;

    // Destructor
    ~VBO();

    // Used if VBO buffer size is altered in application.
    void setBufferData(const void* data, GLsizeiptr bufferSize, GLenum bufferDrawType) const;
    
    // Used if VBO buffer size is known to be fixed. Can alter the data so long as size remains constant. Better performance.
    void setBufferStorage(const void* bufferData, GLsizeiptr bufferSize, GLbitfield flags) const;

    void setBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const;

    const unsigned int& getVBO();

private:
    GLuint id;
};
