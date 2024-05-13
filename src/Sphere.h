#pragma once

#include "SpaceObject.h"
#include <cmath>
#include <memory>

class Sphere : public SpaceObject
{
    public:
        Sphere();
        Sphere(float radius, GLfloat mass, int stacks=20, int slices=20);

        virtual void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        void setMeshPointer(Mesh* sphereMesh);
        Texture* getTexturePointer();
        void setTexturePointer(Texture* texture);
        virtual void render() override = 0;
        void setWorldProperties(glm::mat4* model) override;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override = 0;

        ~Sphere();

    protected:
        GLfloat radius {};
        Mesh *sphereMesh {};
        Texture *texture {};
};
