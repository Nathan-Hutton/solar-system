#include "ShadowMap.h"

#include <stdexcept>

ShadowMap::ShadowMap() {}

bool ShadowMap::init(GLuint width, GLuint height)
{
    shadowWidth     = width;
    shadowHeight    = height;

    // A framebuffer is like a screen.
    // We're making another one that won't be rendered to the screen and we'll draw to it.
    // This framebuffer will render to a texture.
    glGenFramebuffers(1, &FBO);

    // Initialize a bunch of depth values at zero
    // Everytime the FBO is updated it will output the depth value to a texture
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    // GL_DEPTH_COMPONENT specifies a single float instead of RGB which specifies 3. 
    // Depth will be between 0 and 1
    // This initializes an empty texture that we'll later fill in (why we use NULL)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    const float bColor[] {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);

    // Bind the framebuffer so that the next operations apply to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Whenever our framebuffer renders, it will render to our texture.
    // We will only write the depth values to the texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    // Specify that we don't want the buffer to write anything to the color
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    const GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};

    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer error in ShadowMap init method");

    // Unbind by going back to default framebuffer (the main one that renders)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void ShadowMap::write()
{
    // Draw to a framebuffer that is off-screen
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ShadowMap::read(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    // 2D is for directional lighting
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
{
    if(FBO)
        glDeleteFramebuffers(1, &FBO);
    
    if(shadowMap)
        glDeleteTextures(1, &shadowMap);
}
