#include "OmniShadowMap.h"

#include <stdexcept>

OmniShadowMap::OmniShadowMap(GLuint width, GLuint height)
    : m_shadowWidth { width }
    , m_shadowHeight { height }
{}

bool OmniShadowMap::init()
{
    // Make the framebuffer and texture
    glGenFramebuffers(1, &m_FBO);
    glGenTextures(1, &m_shadowMap);

    // Bind our texture to the GL_TEXTURE_CUBE_MAP target
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);

    // Initialize the 6 textures of the cubemap
    // These enums represent integers
    for (int i {0}; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    // Attach the texture to the FBO
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0);

    // No color, just the depth
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};

    if (status != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Framebuffer error in OmniShadowMap init method");

    return true;
}

void OmniShadowMap::read(GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap);
}

OmniShadowMap::~OmniShadowMap()
{
    if(m_FBO)
        glDeleteFramebuffers(1, &m_FBO);
    
    if(m_shadowMap)
        glDeleteTextures(1, &m_shadowMap);
}
