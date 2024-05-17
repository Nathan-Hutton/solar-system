#pragma once

#include "SpaceObject.h"
#include <cmath>
#include <memory>

class Sphere : public SpaceObject
{
    public:
        Sphere();
        Sphere(GLfloat radius, GLfloat mass, int stacks, int slices);

        virtual void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int stacks, int slices);
        void setTexturePointer(Texture* texture);
        virtual void render() override = 0;
        void setWorldProperties(glm::mat4* model) override;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;

        ~Sphere();

    protected:
        const GLfloat radius {};
        Mesh *sphereMesh {};
        Texture *texture {};
};
