#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "PointLight.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(
			GLfloat mass=1.0f,
			bool emitsLight=false,
			std::shared_ptr<Mesh> mesh=nullptr,
			std::shared_ptr<Texture> texture=nullptr,
			std::shared_ptr<Material> material=nullptr,
			PointLight* pLight=nullptr
		)
		: SpaceObject{ mass, emitsLight, mesh, texture, material, pLight }
		{}

        void setWorldProperties(glm::mat4& model) const
		{
			model = glm::translate(model, getPosition());
			model = glm::rotate(model, glm::radians(getAngle()), getRotation());
		}

        ~Sphere() override {}
};
