#pragma once
#include "Light.h"

class DirectionalLight : public Light {
    public:
        DirectionalLight();
        DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
                        GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                        GLfloat xDir, GLfloat yDir, GLfloat zDir);

        void useLight(GLuint ambientIntensityLocation, GLuint colorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation);
        ~DirectionalLight();

    private:
        glm::vec3 direction;
};