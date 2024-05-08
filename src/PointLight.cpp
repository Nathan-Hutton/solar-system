#include "PointLight.h"

PointLight::PointLight() : Light()
{
    position = glm::vec3{0.0f, -1.0f, 0.0f};
    linear = exponential = 0.0f;
    constant = 1.0f; // Don't want to divide by 0 since we have reciprocal function
}

PointLight::PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat exponential, GLfloat linear, GLfloat constant) 
    : Light(1024, 1024, red, green, blue, ambientIntensity, diffuseIntensity)
{
    position = glm::vec3{xPos, yPos, zPos};
    this->exponential = exponential;
    this->linear = linear;
    this->constant = constant;

    farPlane = far;

    float aspectRatio {(float)shadowWidth / (float)shadowHeight};
    // A bunch of 90 degree angles together makes a cube
    lightProj = glm::perspective(glm::radians(90.0f), aspectRatio, near, far);

    shadowMap = new OmniShadowMap();
    shadowMap->init(shadowWidth, shadowHeight);
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

std::vector<glm::mat4> PointLight::calculateLightTransform()
{
    std::vector<glm::mat4> lightMatrices;
    // +x, -x
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));

    // +y, -y
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}));
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}));

    // +z, -z
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));
    lightMatrices.push_back(lightProj * glm::lookAt(position, position + glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));

    return lightMatrices;
}

GLfloat PointLight::getFarPlane()
{
    return farPlane;
}

glm::vec3 PointLight::getPosition()
{
    return position;
}

PointLight::~PointLight()
{
}
