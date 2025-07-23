#pragma once

#include <vector>
#include <memory>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(GLfloat mass=1.0f, GLfloat radius=0.5f, std::shared_ptr<Mesh> sphereMesh=std::shared_ptr<Mesh>());

        void setTexturePointer(std::shared_ptr<Texture> texture) { m_texture = texture; }
        void render() const override = 0;
        void setWorldProperties(glm::mat4& model) const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;
        GLfloat getCollisionDistance() const override { return m_radius; }

        //~Sphere() override { delete m_sphereMesh; }
        ~Sphere() override {}

    protected:
        const GLfloat m_radius {};
		std::shared_ptr<Mesh> m_sphereMesh {};
        std::shared_ptr<Texture> m_texture {};
};
