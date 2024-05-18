#pragma once

#include <GL/glew.h>

class Material
{
    public:
        Material();
        Material(GLfloat specularIntensity, GLfloat shine);

        void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

        ~Material();

    private:
        GLfloat specularIntensity {}; // How much specular light there should be on the object
        GLfloat shininess {}; // How smooth the surface looks
};
