#include "../headers/SSBO.h"

SSBO::SSBO() : id(0) {
    glCreateBuffers(1, &id);
    this->pMappedBuffer = nullptr;
}

SSBO::SSBO(SSBO&& other) noexcept : id(other.id) {
    other.id = 0;
    other.pMappedBuffer = nullptr;

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

GLvoid* SSBO::getPersistentlyMappedBuffer() const
{
    return pMappedBuffer;
}

void SSBO::setBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const {
    glNamedBufferSubData(id, offset, size, data);
}

void SSBO::setupSSBOPersistentMappedBuffer(int bindingIndex, GLsizeiptr bufferSize)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, id);
    setBufferStorage((void*) nullptr, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    pMappedBuffer = glMapNamedBufferRange(id, 0, bufferSize , GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}
