#pragma once

#include <GL/glew.h>
#include "stb_image.h"

class Texture
{
    public:
        Texture();
        Texture(const char* fileLocation);

        bool loadTexture();
        bool loadTextureA();
        void useTexture();
        void clearTexture();

        ~Texture();

    private:
        GLuint textureID;
        int width, height, bitDepth;

        const char* fileLocation;
};
