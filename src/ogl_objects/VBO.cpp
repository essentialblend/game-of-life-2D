#include "../headers/VBO.h"

VBO::VBO() : id(0) {
    glGenBuffers(1, &id);
}

/*Move constructor*/
VBO::VBO(VBO&& other) noexcept : id(other.id) {
    other.id = 0;
}

/*Move assignment operator*/
VBO& VBO::operator=(VBO&& other) noexcept {
    if (this != &other) {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

VBO::~VBO() {
    glDeleteBuffers(1, &id);
}

void VBO::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VBO::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}