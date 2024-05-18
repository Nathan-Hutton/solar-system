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

        ShadowMap* getShadowMap() { return shadowMap; }

        ~Light();

    protected:
        glm::vec3 color {};
        GLfloat ambientIntensity {};
        GLfloat diffuseIntensity {};

        // How the light can see. This will be different for directional and point lights
        glm::mat4 lightProj {};

        ShadowMap* shadowMap {};
};
