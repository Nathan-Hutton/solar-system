#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "ShadowMap.h"

class Light
{
    public:
        Light();
        Light(GLuint shadowWidth, GLuint shadowHeight,
            GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity);

        ShadowMap* getShadowMap() const { return m_shadowMap; }

        ~Light();

    protected:
        glm::vec3 m_color {};
        GLfloat m_ambientIntensity {};
        GLfloat m_diffuseIntensity {};

        // How the light can see. This will be different for directional and point lights
        glm::mat4 m_lightProj {};

        ShadowMap* m_shadowMap {};
};
