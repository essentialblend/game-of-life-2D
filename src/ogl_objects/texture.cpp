#include "../headers/texture.h"

#pragma warning(push)
#include <../stb_image.h>
#pragma warning(pop)


// Constructor
TextureObject::TextureObject() : id(0) {

    glCreateTextures(GL_TEXTURE_2D, 1, &id);
}

// Move constructor
TextureObject::TextureObject(TextureObject&& other) noexcept : id(other.id) {
    other.id = 0;
}

// Move assignment operator
TextureObject& TextureObject::operator=(TextureObject&& other) noexcept {
    if (this != &other) {

        glDeleteTextures(1, &id);
        id = other.id;
        other.id = 0;
    }
    return *this;
}

// Destructor
TextureObject::~TextureObject() {

    glDeleteTextures(1, &id);
    id = 0;
}

// Load texture from file
bool TextureObject::loadFromFile(const std::string& filePath, GLint internalFormat, GLenum format, GLenum type, GLint mipmapCount) {
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (data) {
        if (channels != 4 && channels != 3) {
            stbi_image_free(data);
            // Unsupported image format.
            return false; 
        }

        glTextureStorage2D(id, mipmapCount, internalFormat, width, height);
        glTextureSubImage2D(id, 0, 0, 0, width, height, format, type, data);
        glGenerateTextureMipmap(id);

        stbi_image_free(data);
        return true;
    }
    return false;
}

// Bind texture
void TextureObject::bind(GLuint unit) const {
    glBindTextureUnit(unit, id);
}

// Unbind texture
void TextureObject::unbind(GLuint unit) const {
    glBindTextureUnit(unit, 0);
}

// Get texture ID
GLuint TextureObject::getTextureObjectID() const {
    return id;
}

// Set wrapping, filtering and mipmaps
void TextureObject::setParameters(GLint minWrapBehavior, GLint maxWrapBehavior, GLint minFilterBehavior, GLint maxFilterBehavior, bool IsSingleMipMap) const
{
    // Texture wrapping
    glTextureParameteri(id, GL_TEXTURE_WRAP_S, minWrapBehavior);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, maxWrapBehavior);

    // Texture filtering
    if (IsSingleMipMap)
    {
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilterBehavior);
    }
    else
    {
        glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilterBehavior);
    }
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, maxFilterBehavior);
}