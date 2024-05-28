#pragma once

#include <GL/glew.h>

class ShadowMap
{
    public:
        ShadowMap();

        virtual bool init(GLuint width, GLuint height);

        // Bind the framebuffer
        virtual void write();

        // Put the shadowMap in the shaders in the main render pass
        virtual void read(GLenum textureUnit) const;

        GLuint getShadowWidth() const { return m_shadowWidth; }
        GLuint getShadowHeight() const { return m_shadowHeight; }

        ~ShadowMap();
    protected:
        GLuint m_FBO {}; 
        GLuint m_shadowMap {};
        // Want to make sure that the viewport is the same size as the shadowmap texture 
        // so that we have a 1 to 1 relationship with the FBO and the ShadowMap
        GLuint m_shadowWidth {}, m_shadowHeight {};
};
