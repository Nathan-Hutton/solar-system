#pragma once

#include <GL/glew.h>

class OmniShadowMap
{
    public:
        explicit OmniShadowMap(GLuint width=1024, GLuint height=1024);

        GLuint getShadowWidth() const { return m_shadowWidth; }
        GLuint getShadowHeight() const { return m_shadowHeight; }

        bool init();

        // Draw to a framebuffer that is off-screen
        void write() const { glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); }

        void read(GLenum textureUnit) const;

        ~OmniShadowMap();

    private:
        GLuint m_FBO {}; 
        GLuint m_shadowMap {};
        // Want to make sure that the viewport is the same size as the shadowmap texture 
        // so that we have a 1 to 1 relationship with the FBO and the ShadowMap
        GLuint m_shadowWidth {}, m_shadowHeight {};
};
