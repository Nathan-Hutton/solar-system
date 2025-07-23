#pragma once

#include <memory>

#include "Sphere.h"

class Planet final : public Sphere
{
	public:
		explicit Planet(float mass=1.0f, std::shared_ptr<Mesh> sphereMesh=std::shared_ptr<Mesh>(), std::shared_ptr<Material> material=std::make_shared<Material>());
		void setUniformVariables(GLuint uniformSpecularIntensity, GLuint uniformShininess);

	private:
		GLuint m_specularIntensityLocation, m_shininessLocation;
};

using Moon = Planet;
