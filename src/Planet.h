#pragma once

#include <memory>

#include "Sphere.h"

class Planet final : public Sphere
{
    public:
        explicit Planet(float mass=1.0f, std::shared_ptr<Material> material = std::make_shared<Material>(), GLfloat radius=0.5f, int stacks=20, int slices=20);
        void render() const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;

    private:
        std::shared_ptr<Material> m_material {};
        GLuint m_specularIntensityLocation {}, m_shininessLocation {};
};

using Moon = Planet;
