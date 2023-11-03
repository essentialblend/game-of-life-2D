#pragma once

#include <GLAD/glad/glad.h>
#include <iostream>

class SSBO {
public:
    SSBO();
    SSBO(SSBO&& other) noexcept;
    SSBO& operator=(SSBO&& other) noexcept;
    ~SSBO();

    void setBufferStorage(const void* bufferData, GLsizeiptr bufferSize, GLbitfield flags) const;
    const unsigned int& getSSBO() const;
    void setBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const;

private:
    unsigned int id;
};