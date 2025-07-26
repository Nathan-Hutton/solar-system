#include "Texture.h"

#include <stdexcept>
#include <iostream>

#include "stb_image.h"

// Load the texture at fileLocation
Texture::Texture(const std::string fileLocation)
{
    // 1 char is equal to one byte, so this is an array of bytes really
    int bitDepth, width, height;
    unsigned char *textData {stbi_load(fileLocation.c_str(), &width, &height, &bitDepth, 0)};
    if (!textData)
        throw std::runtime_error("Failed to find " + fileLocation + '\n');

    // Create 1 texture object and store its name (ID) in an array which textureID points to
    glGenTextures(1, &m_textureID);
    
    // Bind texture to a texture target so that when we call methods on that target, it's applied to it.
    // The target in our case is GL_TEXTURE_2D.
    // Means that all subsequent calls to that texture target will be applied to our texture.
    // This is due to the fact that OpenGL works as a state machine, so commands affect the
    // currently bound object
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Apply params to our texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Specify our texture.
    if (bitDepth == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textData);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textData);

    // Mipmaps are smaller versions of textures used for rendering at different distances.
    // Imporves performance and reduces artifacts (look at some of the imported models, for example)
    // In our case, we only defined one glTexImage2D (at the base level, 0),
    // so we won't really be using this but we do have to call it.
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // since the data has been copied to the glTextImage2D we don't need the raw data anymore so we free it
    stbi_image_free(textData);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
    m_textureID       = 0;
}
