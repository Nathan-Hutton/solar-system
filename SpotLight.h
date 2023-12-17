#pragma once

#include "PointLight.h"

class SpotLight : public PointLight {
    public:
        SpotLight();

        SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat xDir, GLfloat yDir, GLfloat zDir,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge);

        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation, GLuint directionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation,
                    GLuint edgeLocation);
        glm::vec3 getPosition();

        void setFlash(glm::vec3 pos, glm::vec3 dir);

        ~SpotLight();

    private:
        glm::vec3 direction;
        GLfloat edge, procEdge;
};
