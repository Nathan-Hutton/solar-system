#pragma once

#include "SpaceObject.h"
#include <cmath>

class Sphere : public SpaceObject
{
    public:
        Sphere();
        Sphere(float radius, GLfloat mass, int stacks=20, int slices=20);

        virtual void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        void setMeshPointer(Mesh* sphereMesh);
        Texture* getTexturePointer();
        void setTexturePointer(Texture* texture);
        void render() override;
        void setWorldProperties(glm::mat4* model) override;

        ~Sphere();

    protected:
        GLfloat radius;
        Mesh *sphereMesh;
        Texture *texture;
};
