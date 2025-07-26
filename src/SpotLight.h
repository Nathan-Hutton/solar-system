#pragma once

#include "PointLight.h"

class SpotLight : public PointLight {
    public:
        SpotLight();

        SpotLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
					glm::vec3 color,
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    glm::vec3 position,
                    glm::vec3 direction,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge);

        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation, GLuint directionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation,
                    GLuint edgeLocation) const;
        void setPosAndDir(GLuint positionLocation, GLuint directionLocation) const;
        const glm::vec3& getDirection() const { return m_direction; }

        void setFlash(glm::vec3 pos, glm::vec3 dir);
        bool isOn() const { return m_on; }
        void toggle() { m_on = !m_on; }

    private:
        glm::vec3 m_direction {};
        GLfloat m_edge {}, m_procEdge {};
        bool m_on {};
};
