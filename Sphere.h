#pragma once

#include <vector>
#include <GL/glew.h>
#include <cmath>
#include "Mesh.h"

class Sphere
{
    public:
        Sphere();
        Sphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);

        void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        Mesh* getMeshPointer();
        float getRadius();

        ~Sphere();

    private:
        Mesh *sphereMesh;
        float radius;
};