#pragma once

#include "Sphere.h"
#include "Material.h"

class Planet : public Sphere
{
    public:
        Planet();
        Planet(float radius, GLfloat mass, int stacks=20, int slices=20);
        void render() override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        ~Planet();

    private:
        GLuint specularIntensityLocation {}, shininessLocation {};
};
