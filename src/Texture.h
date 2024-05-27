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
        GLuint textureID {};
        int width {}, height {};

        std::string fileLocation {};

        void clearTexture();
};
