#pragma once

#include <memory>

#include "Sphere.h"

class Planet final : public Sphere
{
	public:
		explicit Planet(float mass=1.0f, std::shared_ptr<Material> material=std::make_shared<Material>(), std::shared_ptr<Mesh> sphereMesh=std::shared_ptr<Mesh>());
        void render() const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;

    private:
        std::shared_ptr<Material> m_material {};
        GLuint m_specularIntensityLocation {}, m_shininessLocation {};
};

using Moon = Planet;
