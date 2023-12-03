#include "Light.h"
#include <stdio.h>

Light::Light()
{
    this->color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity)
{
    // We might wanna clamp the red green and blue to 1 at some point
    this->color = glm::vec3(red, green, blue);
    this->ambientIntensity = ambientIntensity;
}

void Light::useLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation)
{
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform3f(ambientColorLocation, color.x, color.y, color.z);
}

Light::~Light()
{
}