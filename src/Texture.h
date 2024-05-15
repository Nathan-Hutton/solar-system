#pragma once

#include <GL/glew.h>
#include <string>
#include <memory>
#include <iostream>
#include "CommonValues.h"

class Texture
{
    public:
        Texture();
        explicit Texture(std::string fileLocation);

        std::string getFileLocation();

        bool loadTexture();
        void useTexture();
        void clearTexture();

        ~Texture();

    private:
        GLuint textureID {};
        int width {}, height {};

        std::string fileLocation {};
};
