#include "../headers/VBO.h"

#include <GLAD/glad/glad.h>

VBO::VBO() : id(0) {
    // Pre 4.6 opengl.
    // glGenBuffers(1, &id);

    // Post 4.6 opengl with Direct State Access.
    glCreateBuffers(1, &id);
}

// Move constructor
VBO::VBO(VBO&& other) noexcept : id(other.id) {
    other.id = 0;
}

// Move assignment operator
VBO& VBO::operator=(VBO&& other) noexcept {
    if (this != &other) {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

// Delete VBO
VBO::~VBO() {
    glDeleteBuffers(1, &id);
}

void VBO::setBufferData(const void* bufferData, GLsizeiptr bufferSize, GLenum bufferDrawType) const
{
    glNamedBufferData(id, bufferSize, bufferData, bufferDrawType);
}

void VBO::setBufferStorage(const void* bufferData, GLsizeiptr bufferSize, GLbitfield flags) const
{
    glNamedBufferStorage(id, bufferSize, bufferData, flags);
}

const unsigned int& VBO::getVBO()
{
    return id;
}

void VBO::setBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const
{
    glNamedBufferSubData(id, offset, size, data);
}