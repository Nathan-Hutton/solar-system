#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
    public:
        Light();
        Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity);

        void useLight(GLfloat ambientIntensityLocation, GLfloat colorLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation);

        ~Light();

    private:
        glm::vec3 color;
        GLfloat ambientIntensity;

        glm::vec3 direction;
        GLfloat diffuseIntensity;
};