#pragma once

#include "ShadowMap.h"
#include "iostream"

class OmniShadowMap : public ShadowMap
{
    public:
        OmniShadowMap();

        bool init(unsigned int width, unsigned int height);
        void read(GLenum textureUnit);

        ~OmniShadowMap();

    private:
};
