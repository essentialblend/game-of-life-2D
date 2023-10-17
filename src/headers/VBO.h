#pragma once

#include <glad/glad.h>

class VBO {
public:

    VBO();

    /*Delete copy constructor and copy assignment operators*/
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    /*Move constructor*/
    VBO(VBO&& other) noexcept;

    /*Move assignment operator*/
    VBO& operator=(VBO&& other) noexcept;

    /*Destructor*/
    ~VBO();

    /*Bind VBO*/
    void bind() const;

    /*Unbind VBO*/
    void unbind() const;

private:
    GLuint id;
};
