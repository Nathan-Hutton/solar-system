#pragma once

#include <vector>
#include <memory>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(GLfloat mass=1.0f, GLfloat radius=0.5f, int stacks=15, int slices=15, bool usingNormals=true);

        void setTexturePointer(std::shared_ptr<Texture> texture) { m_texture = texture; }
        void render() const override = 0;
        void setWorldProperties(glm::mat4& model) override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;
        GLfloat getCollisionDistance() const override { return m_radius; }

        ~Sphere() { delete m_sphereMesh; }

    protected:
        const GLfloat m_radius {};
        Mesh* m_sphereMesh {};
        std::shared_ptr<Texture> m_texture {};
};
