#pragma once

#include <vector>

#include "Light.h"

class PointLight : public Light {
    public:
        PointLight();

        PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat exponential, GLfloat linear, GLfloat constant);
        
        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation) const;

        std::vector<glm::mat4> calculateLightTransform() const;

        GLfloat getFarPlane() const;

        glm::vec3 getPosition() const;

        ~PointLight();

    protected:
        glm::vec3 position {};
        GLfloat exponential {}, linear {}, constant {};
        
        // How far we want shadows to go. The farther they go, the lower resolution they are.
        GLfloat farPlane {};
};
