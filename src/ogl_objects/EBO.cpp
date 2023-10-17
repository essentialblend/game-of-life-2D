#include <../gameOfLife2D/src/headers/EBO.h>

/*Constructor*/
EBO::EBO() : id(0) {
    glGenBuffers(1, &id);
}

/*Move constructor*/
EBO::EBO(EBO&& other) noexcept : id(other.id) {
    other.id = 0;
}

/*Move assignment operator*/
EBO& EBO::operator=(EBO&& other) noexcept {
    if (this != &other) {
        glDeleteBuffers(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

/*Destructor*/
EBO::~EBO() {
    glDeleteBuffers(1, &id);
}

/*Bind EBO*/
void EBO::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

/*Unbind EBO*/
void EBO::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*Get VBO*/
unsigned int EBO::getEBO() const
{
    return id;
}