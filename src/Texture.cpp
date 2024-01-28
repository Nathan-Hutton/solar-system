#include "Texture.h"

Texture::Texture()
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    fileLocation = NULL;
}

Texture::Texture(const char* fileLocation)
{
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    this->fileLocation = strdup(fileLocation);
}

const char* Texture::getFileLocation()
{
    return fileLocation;
}

// Load the texture at fileLocation
bool Texture::loadTexture()
{
    // 1 char is equal to one byte, so this is an array of bytes really
    unsigned char *textData = stbi_load(fileLocation, &width, &height, &bitDepth, STBI_rgb);
    if (!textData)
    {
        printf("Failed to find %s\n", fileLocation);
        return false;
    }

    // Create 1 texture object and store its name (ID) in an array which textureID points to
    glGenTextures(1, &textureID);
    
    // Bind texture to a texture target so that when we call methods on that target, it's applied to it.
    // The target in our case is GL_TEXTURE_2D.
    // Means that all subsequent calls to that texture target will be applied to our texture.
    // This is due to the fact that OpenGL works as a state machine, so commands affect the
    // currently bound object
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Apply params to our texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Specify our texture.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textData);

    // Mipmaps are smaller versions of textures used for rendering at different distances.
    // Imporves performance and reduces artifacts (look at some of the imported models, for example)
    // In our case, we only defined one glTexImage2D (at the base level, 0),
    // so we won't really be using this but we do have to call it.
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // since the data has been copied to the glTextImage2D we don't need the raw data anymore so we free it
    stbi_image_free(textData);
    return true;
}

// Load the texture at fileLocation with alpha values
bool Texture::loadTextureA()
{
    // 1 char is equal to one byte, so this is an array of bytes really
    unsigned char *textData = stbi_load(fileLocation, &width, &height, &bitDepth, STBI_rgb_alpha);
    if (!textData)
    {
        printf("Failed to find %s\n", fileLocation);
        return false;
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
    return true;
}

void Texture::useTexture()
{
    // Specify which texture unit we'll be using for texture operations.
    // We'll only have 1 active at a time.
    glActiveTexture(GL_TEXTURE2);

    // Bind our texture for draw operations
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
