#pragma once

#include <GL/glew.h>

class Material
{
    public:
        Material();
        Material(GLfloat specularIntensity, GLfloat shine);

        void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) const;

        ~Material();

    private:
        GLfloat m_specularIntensity {}; // How much specular light there should be on the object
        GLfloat m_shininess {}; // How smooth the surface looks
};
