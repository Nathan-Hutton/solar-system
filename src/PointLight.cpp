#include "PointLight.h"

#include "OmniShadowMap.h"

PointLight::PointLight() : Light()
{
    m_position    = glm::vec3{0.0f, -1.0f, 0.0f};
    m_constant    = 1.0f; // Don't want to divide by 0 since we have reciprocal function
}

PointLight::PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat exponential, GLfloat linear, GLfloat constant) 
    : Light(1024, 1024, red, green, blue, ambientIntensity, diffuseIntensity)
{
    m_position      = glm::vec3{xPos, yPos, zPos};
    m_exponential   = exponential;
    m_linear        = linear;
    m_constant      = constant;
    m_farPlane      = far;

    const float aspectRatio {(float)shadowWidth / (float)shadowHeight};
    // A bunch of 90 degree angles together makes a cube
    m_lightProj = glm::perspective(glm::radians(90.0f), aspectRatio, near, far);

    m_shadowMap = new OmniShadowMap{};
    m_shadowMap->init(shadowWidth, shadowHeight);
}

void PointLight::useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                         GLuint colorLocation, GLuint positionLocation,
                         GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation) const
{
    glUniform1f(ambientIntensityLocation, m_ambientIntensity);
    glUniform1f(diffuseIntensityLocation, m_diffuseIntensity);
    glUniform3f(colorLocation, m_color.x, m_color.y, m_color.z);

    glUniform3f(positionLocation, m_position.x, m_position.y, m_position.z);
    glUniform1f(exponentialLocation, m_exponential);
    glUniform1f(linearLocation, m_linear);
    glUniform1f(constantLocation, m_constant);
}

std::vector<glm::mat4> PointLight::calculateLightTransform() const
{
    std::vector<glm::mat4> lightMatrices {};
    // +x, -x
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));

    // +y, -y
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f}));
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f}));

    // +z, -z
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));
    lightMatrices.push_back(m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f}));

    return lightMatrices;
}

GLfloat PointLight::getFarPlane() const
{
    return m_farPlane;
}

const glm::vec3& PointLight::getPosition() const
{
    return m_position;
}

PointLight::~PointLight()
{
}
