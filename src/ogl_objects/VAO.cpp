#include <../gameOfLife2D/src/headers/VAO.h>

/*Constructor*/
VAO::VAO() : id(0) {
    glGenVertexArrays(1, &id);
}

/*Move constructor*/
VAO::VAO(VAO&& other) noexcept : id(other.id) {
    other.id = 0;
}

/*Move assignment operator*/
VAO& VAO::operator=(VAO&& other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

/*Destructor*/
VAO::~VAO() {
    glDeleteVertexArrays(1, &id);
}

/*Bind Vertex Array*/
void VAO::bind() const {
    glBindVertexArray(id);
}

/*Unbind Vertex Array*/
void VAO::unbind() const {
    glBindVertexArray(0);
}

/*Get FBO*/
GLuint VAO::getVAO() const {
    return id;
}