#include "Texture.h"

Texture::Texture()
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = NULL;
}

Texture::Texture(char* fileLocation)
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    this->fileLocation = fileLocation;
}

// Load the texture at fileLocation
void Texture::loadTexture()
{
    // 1 char is equal to one byte, so this is an array of bytes really
    unsigned char *textData = stbi_load(fileLocation, &width, &height, &bitDepth, 4);
    if (!textData)
    {
        printf("Failed to find %s\n", fileLocation);
        return;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Consider using CLAMP_TO_EDGE instead of GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    // since the data has been copied to the glTextImage2D we don't need the raw data anymore so we free it
    stbi_image_free(textData);
}

void Texture::useTexture()
{
    // This is a texture unit. When the texture is run in the fragment shader, there's a sampler, and the sampler has the data
    // for the texture but it accesses it through the texture unit.
    // GL_TEXTURE0 is a texture unit. There's usually like 32 of them, this is the first. If we wanted this would let us
    // bind multiple textures at once. We're making the first texture unit active for this project then binding it
    // Then the fragment shader could access all of them. Would require writing a uniform variable for each one in the shader
    // 0 is default so technically we don't need this arg
    glActiveTexture(GL_TEXTURE0);
    // Binds this texture (this class we're in, Texture.h) to the unit we just activated
    // So now when we try to draw GL_TEXTURE0 (texture unit) it will draw this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::clearTexture()
{
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = NULL;
}

Texture::~Texture()
{
    clearTexture();
}
