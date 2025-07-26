#include "PointLight.h"

#include "OmniShadowMap.h"

PointLight::PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
					glm::vec3 color,
                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                    glm::vec3 position,
                    GLfloat exponential, GLfloat linear, GLfloat constant) 
    : m_shadowMap{ std::make_unique<OmniShadowMap>(shadowWidth, shadowHeight) }
    , m_color{ color }
    , m_ambientIntensity{ ambientIntensity }
    , m_diffuseIntensity{ diffuseIntensity }
    , m_position{ position }
    , m_exponential{ exponential }
    , m_linear{ linear }
    , m_constant{ constant }
    , m_farPlane{ far }
{
    m_shadowMap->init();

    const float aspectRatio {(float)shadowWidth / (float)shadowHeight};

    // A bunch of 90 degree angles together makes a cube
    m_lightProj = glm::perspective(glm::radians(90.0f), aspectRatio, near, far);
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

// TODO: Make the main expression after the equals sign a method so that we can return an unnamed array
// which will take advantage of copy elison
std::array<glm::mat4, 6> PointLight::calculateLightTransform() const
{
    std::array<glm::mat4, 6> lightMatrices {};
    // +x, -x
    lightMatrices[0] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f});
    lightMatrices[1] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f});

    // +y, -y
    lightMatrices[2] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 1.0f});
    lightMatrices[3] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, -1.0f});

    // +z, -z
    lightMatrices[4] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 0.0f, 1.0f}, glm::vec3{0.0f, -1.0f, 0.0f});
    lightMatrices[5] = m_lightProj * glm::lookAt(m_position, m_position + glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, -1.0f, 0.0f});

    return lightMatrices;
}
