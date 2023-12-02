#pragma once

#include <GL/glew.h>
#include "stb_image.h"

class Texture
{
    public:
        Texture();
        Texture(char* fileLocation);

        void loadTexture();
        void useTexture();
        void clearTexture();

        ~Texture();

    private:
        GLuint textureID;
        int width, height, bitDepth;

        char* fileLocation;
};