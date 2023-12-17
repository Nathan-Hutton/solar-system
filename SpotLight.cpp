#include "SpotLight.h"

SpotLight::SpotLight() : PointLight()
{
    direction = glm::vec3(1.0f, 0.0f, 0.0f);
    edge = 1.0f;
    procEdge = cosf(glm::radians(edge));
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat xPos, GLfloat yPos, GLfloat zPos,
    GLfloat xDir, GLfloat yDir, GLfloat zDir,
    GLfloat exponential, GLfloat linear, GLfloat constant,
    GLfloat edge) 
    : PointLight(red, green, blue, ambientIntensity, diffuseIntensity, xPos, yPos, zPos, exponential, linear, constant)
{
    direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
    this->edge = edge;
    procEdge = cosf(glm::radians(edge));
}

void SpotLight::setFlash(glm::vec3 pos, glm::vec3 dir)
{
    position = pos;
    direction = dir;
}

void SpotLight::useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                         GLuint colorLocation, GLuint positionLocation, GLuint directionLocation,
                         GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation,
                         GLuint edgeLocation)
{
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
    glUniform3f(colorLocation, color.x, color.y, color.z);

    glUniform3f(positionLocation, position.x, position.y, position.z);
    glUniform1f(exponentialLocation, exponential);
    glUniform1f(linearLocation, linear);
    glUniform1f(constantLocation, constant);

    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(edgeLocation, procEdge);
}

glm::vec3 SpotLight::getPosition()
{
    return position;
}

SpotLight::~SpotLight()
{
}
