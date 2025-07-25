#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun final : public Sphere
{
    public:
        explicit Sun(
				GLfloat mass=2.0f,
				std::shared_ptr<Mesh> sphereMesh=nullptr,
				std::shared_ptr<Texture> texture=nullptr,
				PointLight* pLight=nullptr
			)
			: Sphere{ mass, true, sphereMesh, texture, nullptr, pLight } 
		{}
};
