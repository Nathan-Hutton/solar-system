#pragma once

#include <vector>
#include <memory>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(GLfloat mass=1.0f, std::shared_ptr<Mesh> mesh=nullptr, std::shared_ptr<Texture> texture=nullptr, std::shared_ptr<Material> material=nullptr);

        void render() const;
        void setWorldProperties(glm::mat4& model) const;

        ~Sphere() override {}
};
