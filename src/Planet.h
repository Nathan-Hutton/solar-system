#pragma once

#include "Sphere.h"

class Planet : public Sphere
{
    public:
        explicit Planet(float radius=0.5f, GLfloat mass=1.0f, int stacks=20, int slices=20);
        void render() const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        ~Planet() { delete m_sphereMesh, m_material; }

    private:
        GLuint m_specularIntensityLocation {}, m_shininessLocation {};
};
