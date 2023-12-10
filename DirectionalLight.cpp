#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat xDir, GLfloat yDir, GLfloat zDir) 
    : Light(red, green, blue, ambientIntensity, diffuseIntensity)
{
    direction = glm::vec3(xDir, yDir, zDir);
}

void DirectionalLight::useLight(GLuint ambientIntensityLocation, GLuint colorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation)
{
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
    glUniform3f(colorLocation, color.x, color.y, color.z);
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

DirectionalLight::~DirectionalLight()
{
}