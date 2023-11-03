#include "../headers/RBO.h"

// Constructor
RBO::RBO() : id(0) {
    glGenRenderbuffers(1, &id);
}

// Move constructor
RBO::RBO(RBO&& other) noexcept : id(other.id) {
    other.id = 0; // Reset the moved-from object's ID to ensure it doesn't get deleted
}

// Move assignment operator
RBO& RBO::operator=(RBO&& other) noexcept {
    if (this != &other) {
        // Release the existing resource
        glDeleteRenderbuffers(1, &id);

        // Transfer ownership from other
        id = other.id;
        // Reset the moved-from object's ID
        other.id = 0; 
    }
    return *this;
}

// Destructor
RBO::~RBO() {
    glDeleteRenderbuffers(1, &id);
}

// Bind Renderbuffer
void RBO::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, id);
}

// Unbind Renderbuffer
void RBO::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // 0 unbinds the current renderbuffer
}

// Get RBO ID
GLuint RBO::getRBO() const {
    return id;
}