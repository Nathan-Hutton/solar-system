#pragma once

#include "Sphere.h"

class Planet : public Sphere
{
    public:
        Planet();
        Planet(float radius, GLfloat mass, int stacks=20, int slices=20);
        void render() const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        ~Planet();

    private:
        GLuint m_specularIntensityLocation {}, m_shininessLocation {};
};
