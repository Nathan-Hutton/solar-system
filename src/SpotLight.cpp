#include "SpotLight.h"

SpotLight::SpotLight(GLuint shadowWidth, GLuint shadowHeight,
    GLfloat near, GLfloat far,
	glm::vec3 color,
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    glm::vec3 position,
    glm::vec3 direction,
    GLfloat exponential, GLfloat linear, GLfloat constant,
    GLfloat edge) 
    : PointLight(shadowWidth, shadowHeight, near, far, color, ambientIntensity, diffuseIntensity, position, exponential, linear, constant)
    , m_direction { glm::normalize(direction) }
    , m_edge { edge }
    , m_procEdge { cosf(glm::radians(m_edge)) }
{}

void SpotLight::setFlash(glm::vec3 pos, glm::vec3 dir)
{
    m_position    = pos;
    m_direction   = dir;
}

void SpotLight::useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                         GLuint colorLocation, GLuint positionLocation, GLuint directionLocation,
                         GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation,
                         GLuint edgeLocation) const
{
    glUniform1f(ambientIntensityLocation, m_ambientIntensity);
    glUniform1f(diffuseIntensityLocation, m_diffuseIntensity);
    glUniform3f(colorLocation, m_color.x, m_color.y, m_color.z);

    glUniform3f(positionLocation, m_position.x, m_position.y, m_position.z);
    glUniform1f(exponentialLocation, m_exponential);
    glUniform1f(linearLocation, m_linear);
    glUniform1f(constantLocation, m_constant);

    glUniform3f(directionLocation, m_direction.x, m_direction.y, m_direction.z);
    glUniform1f(edgeLocation, m_procEdge);
}

void SpotLight::setPosAndDir(GLuint positionLocation, GLuint directionLocation) const
{
    glUniform3f(positionLocation, m_position.x, m_position.y, m_position.z);
    glUniform3f(directionLocation, m_direction.x, m_direction.y, m_direction.z);
}
