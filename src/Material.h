#pragma once

#include <GL/glew.h>

class Material
{
    public:
        explicit Material(GLfloat specularIntensity=0.0f, GLfloat shine=0.0f);

        void useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) const;

    private:
        GLfloat m_specularIntensity {}; // How much specular light there should be on the object
        GLfloat m_shininess {}; // How smooth the surface looks
};
