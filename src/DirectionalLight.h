#pragma once
#include "Light.h"

class DirectionalLight : public Light {
    public:
        DirectionalLight();
        DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
                        GLfloat red, GLfloat green, GLfloat blue, 
                        GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                        GLfloat xDir, GLfloat yDir, GLfloat zDir);

        void useLight(GLuint ambientIntensityLocation, GLuint colorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation);
        glm::mat4 calculateLightTransform();

        ~DirectionalLight();

    private:
        glm::vec3 direction;
};
