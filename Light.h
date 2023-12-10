#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
    public:
        Light();
        Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat diffuseIntensity);
        ~Light();

    protected:
        glm::vec3 color;
        GLfloat ambientIntensity;
        GLfloat diffuseIntensity;
};