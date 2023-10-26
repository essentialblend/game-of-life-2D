#pragma once

#include <string>
#include <GLAD/glad/glad.h>

class TextureObject {
public:
    // Constructor
    TextureObject();
    // Delete copy constructor and copy assignment operators
    TextureObject(const TextureObject&) = delete;
    TextureObject& operator=(const TextureObject&) = delete;
    // Move constructor
    TextureObject(TextureObject&& other) noexcept;
    // Move assignment operator
    TextureObject& operator=(TextureObject&& other) noexcept;
    // Destructor
    ~TextureObject();

    // Load texture from file
    bool loadFromFile(const std::string& filePath, GLint internalFormat, GLenum format, GLenum type, GLint mipmapCount);

    // Bind texture
    void bind(GLuint unit) const;
    // Unbind texture
    void unbind(GLuint unit) const;

    // Get texture ID
    GLuint getTextureObjectID() const;

    void setParameters(GLint minWrapBehavior, GLint maxWrapBehavior, GLint minFilterBehavior, GLint maxFilterBehavior, bool IsSingleMipMap) const;

private:
    // Texture ID
    GLuint id;
};
