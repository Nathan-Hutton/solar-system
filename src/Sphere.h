#pragma once

#include <vector>
#include <memory>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(GLfloat mass=1.0f, std::shared_ptr<Mesh> sphereMesh=std::shared_ptr<Mesh>(), std::shared_ptr<Material> material=nullptr);

        void setTexturePointer(std::shared_ptr<Texture> texture) { m_texture = texture; }
        void render() const;
        void setWorldProperties(glm::mat4& model) const;

        ~Sphere() override {}

    protected:
		std::shared_ptr<Mesh> m_sphereMesh {};
        std::shared_ptr<Texture> m_texture {};
};
