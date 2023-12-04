#include "Light.h"
#include <stdio.h>

Light::Light()
{
    this->color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->ambientIntensity = 1.0f;
    this->direction = glm::vec3(0.0f, -1.0f, 0.0f);
    this->diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity)
{
    // We might wanna clamp the red green and blue to 1 at some point
    this->color = glm::vec3(red, green, blue);
    this->ambientIntensity = ambientIntensity;
    this->direction = glm::vec3(xDir, yDir, zDir);
    this->diffuseIntensity = diffuseIntensity;
}

void Light::useLight(GLfloat ambientIntensityLocation, GLfloat colorLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation)
{
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
    glUniform3f(colorLocation, color.x, color.y, color.z);
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

Light::~Light()
{
}