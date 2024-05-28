#pragma once

#include <GL/glew.h>
#include <string>

class Texture
{
    public:
        Texture();
        explicit Texture(std::string fileLocation);

        std::string getFileLocation() const;

        bool loadTexture();
        void useTexture() const;

        ~Texture();

    private:
        GLuint m_textureID {};
        int m_width {}, m_height {};

        std::string m_fileLocation {};

        void clearTexture();
};
