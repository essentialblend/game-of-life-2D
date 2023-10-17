#include "../headers/FBO.h"

/*Constructor.*/
FBO::FBO() {
    glGenFramebuffers(1, &fbo);
}

/*Move constructor*/
FBO::FBO(FBO&& other) noexcept : fbo(other.fbo) {
    other.fbo = 0;
}

/*Move assignment operator*/
FBO& FBO::operator=(FBO&& other) noexcept {
    if (this != &other) {
        glDeleteFramebuffers(1, &fbo);
        fbo = other.fbo;
        other.fbo = 0;
    }
    return *this;
}

/*Destructor*/
FBO::~FBO() {
    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
    }
}

/*Bind FBO*/
void FBO::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

/*Unbind FBO*/
void FBO::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/*Get FBO*/
GLuint FBO::getFBO() const {
    return fbo;
}