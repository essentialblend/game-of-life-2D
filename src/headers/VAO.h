#pragma once

#include <glad/glad.h>

class VAO {
public:
    /*Constructor*/
    VAO();

    /*Delete copy constructor and copy assignment operators*/
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    /*Move constructor*/
    VAO(VAO&& other) noexcept;

    /*Move assignment operator*/
    VAO& operator=(VAO&& other) noexcept;

    /*Destructor*/
    ~VAO();

    /*Bind Vertex Array*/
    void bind() const;

    /*Unbind Vertex Array*/
    void unbind() const;

    /*Get VAO ID*/
    GLuint getVAO() const;

private:
    /*Vertex Array ID*/
    GLuint id;
};