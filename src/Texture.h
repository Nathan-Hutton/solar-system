#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
    public:
        explicit Texture(std::string fileLocation) : m_fileLocation { fileLocation } {}

        bool loadTexture();

        // Specify which texture unit we'll be using for texture operations.
        // We'll only have 1 active at a time.

        // Bind our texture for draw operations
        void useTexture() const { glBindTexture(GL_TEXTURE_2D, m_textureID); }

        ~Texture();

    private:
        GLuint m_textureID {};
        int m_width {}, m_height {};

        std::string m_fileLocation {};
};
