#pragma once
#include "Light.h"

class PointLight : Light {
    public:
        PointLight();

        PointLight(GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat exponential, GLfloat linear, GLfloat constant);
        
        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation);
        glm::vec3 getPosition();

        ~PointLight();

    private:
        glm::vec3 position;
        GLfloat exponential, linear, constant;
};