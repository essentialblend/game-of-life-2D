#pragma once
#include "GLAD/glad/glad.h"

class FBO {
public:
    // Constructor
    FBO();

    // Delete copy constructor and copy assignment operators
    FBO(const FBO&) = delete;
    FBO& operator=(const FBO&) = delete;

    // Define the Move constructor
    FBO(FBO&& other) noexcept;

    // Move assignment operator
    FBO& operator=(FBO&& other) noexcept;

    // Destructor
    ~FBO();

    // Bind operation
    void bind() const;

    // Unbind operation
    void unbind() const;

    // Get FBO object
    GLuint getFBO() const;

private:
    GLuint fbo = 0;
};
