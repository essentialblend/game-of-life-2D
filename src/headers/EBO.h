#pragma once

#include <GLAD/glad/glad.h>

class EBO {
public:

    EBO();

    // Delete copy constructor and copy assignment operators
    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;

    // Define the Move constructor
    EBO(EBO&& other) noexcept;

    // Move assignment operator
    EBO& operator=(EBO&& other) noexcept;

    // Destructor
    ~EBO();

    // Bind EBO
    void bind() const;

    // Unbind EBO
    void unbind() const;

    // Get EBO
    unsigned int getEBO() const;

    // Set buffer data.
    void setBufferData(GLsizeiptr size, const void* data, GLenum usage) const;

    void setBufferStorage(const void* bufferData, GLsizeiptr bufferSize, GLbitfield flags) const;

private:
    unsigned int id;
};