#include "PointLight.h"

PointLight::PointLight() : Light()
{
    position = glm::vec3(0.0f, -1.0f, 0.0f);
    linear = exponential = 0.0f;
    constant = 1.0f; // Don't want to divide by 0 since we have reciprocal function
}

PointLight::PointLight(GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat xPos, GLfloat yPos, GLfloat zPos,
    GLfloat exponential, GLfloat linear, GLfloat constant) 
    : Light(red, green, blue, ambientIntensity, diffuseIntensity)
{
    position = glm::vec3(xPos, yPos, zPos);
    this->exponential = exponential;
    this->linear = linear;
    this->constant = constant;
}

void PointLight::useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                         GLuint colorLocation, GLuint positionLocation,
                         GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation)
{
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
    glUniform3f(colorLocation, color.x, color.y, color.z);

    glUniform3f(positionLocation, position.x, position.y, position.z);
    glUniform1f(exponentialLocation, exponential);
    glUniform1f(linearLocation, linear);
    glUniform1f(constantLocation, constant);
}

glm::vec3 PointLight::getPosition()
{
    return position;
}

PointLight::~PointLight()
{
}