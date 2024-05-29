#pragma once

#include <GL/glew.h>

class OmniShadowMap
{
    public:
        OmniShadowMap(GLuint width, GLuint height);

        GLuint getShadowWidth() const { return m_shadowWidth; }
        GLuint getShadowHeight() const { return m_shadowHeight; }

        bool init();
        void write() const;
        void read(GLenum textureUnit) const;

        ~OmniShadowMap();

    private:
        GLuint m_FBO {}; 
        GLuint m_shadowMap {};
        // Want to make sure that the viewport is the same size as the shadowmap texture 
        // so that we have a 1 to 1 relationship with the FBO and the ShadowMap
        GLuint m_shadowWidth {}, m_shadowHeight {};
};
