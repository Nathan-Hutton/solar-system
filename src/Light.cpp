#include "Light.h"
#include <stdio.h>

Light::Light()
{
    this->color = glm::vec3{1.0f};
    this->ambientIntensity = 1.0f;
    this->diffuseIntensity = 0.0f;
}

Light::Light(GLuint shadowWidth, GLuint shadowHeight,
    GLfloat red, GLfloat green, GLfloat blue,
    GLfloat ambientIntensity, GLfloat diffuseIntensity)
{
    shadowMap = new ShadowMap();
    shadowMap->init(shadowWidth, shadowHeight);

    this->color = glm::vec3{red, green, blue};
    this->ambientIntensity = ambientIntensity;
    this->diffuseIntensity = diffuseIntensity;
}

Light::~Light()
{
}
