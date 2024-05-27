#pragma once

#include "ShadowMap.h"

class OmniShadowMap : public ShadowMap
{
    public:
        OmniShadowMap();

        bool init(GLuint width, GLuint height);
        void read(GLenum textureUnit) const override;

        ~OmniShadowMap();

    private:
};
