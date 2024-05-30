#pragma once

#include <vector>

#include "SpaceObject.h"
#include "Mesh.h"
#include "Texture.h"

class Sphere : public SpaceObject
{
    public:
        explicit Sphere(GLfloat mass=1.0f, GLfloat radius=0.5f, int stacks=15, int slices=15, bool usingNormals=true);

        void setTexturePointer(Texture* texture) { m_texture = texture; }
        virtual void render() const override = 0;
        void setWorldProperties(glm::mat4& model) override;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;
        GLfloat getCollisionDistance() const override { return m_radius; }

        ~Sphere() { delete m_sphereMesh; }

    protected:
        const GLfloat m_radius {};
        Mesh *m_sphereMesh {};
        Texture *m_texture {};
};
