#include "../headers/SSBO.h"

SSBO::SSBO() : id(0) {
    glCreateBuffers(1, &id);
}

SSBO::SSBO(SSBO&& other) noexcept : id(other.id) {
    other.id = 0;
}

SSBO& SSBO::operator=(SSBO&& other) noexcept {
    if (this != &other) {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

SSBO::~SSBO() {
    glDeleteBuffers(1, &id);
}

void SSBO::setBufferStorage(const void* bufferData, GLsizeiptr bufferSize, GLbitfield flags) const {
    glNamedBufferStorage(id, bufferSize, bufferData, flags);
}

const unsigned int& SSBO::getSSBO() const {
    return id;
}

void SSBO::setBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const {
    glNamedBufferSubData(id, offset, size, data);
}