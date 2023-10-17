#pragma once

#include <glad/glad.h>

class RBO {
public:
    // Constructor
    RBO();

    // Delete copy constructor and copy assignment operators
    RBO(const RBO&) = delete;
    RBO& operator=(const RBO&) = delete;

    // Move constructor
    RBO(RBO&& other) noexcept;

    // Move assignment operator
    RBO& operator=(RBO&& other) noexcept;

    // Destructor
    ~RBO();

    // Bind Renderbuffer
    void bind() const;

    // Unbind Renderbuffer
    void unbind() const;

    // Get RBO ID
    GLuint getRBO() const;

private:
    // Renderbuffer ID
    GLuint id;
};