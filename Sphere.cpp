#include "Sphere.h"

Sphere::Sphere()
{
    this->radius = 0;
    sphereMesh = new Mesh();
}

Sphere::Sphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices)
{
    this->radius = radius;
    sphereMesh = new Mesh();
    generateSphereData(vertices, indices, radius, stacks, slices);
    sphereMesh->CreateMesh(vertices.data(), indices.data(), vertices.size(), indices.size());
}

void Sphere::generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices)
{
    this->radius = radius;
    for (int i = 0; i <= stacks; ++i) {
        float V = i / (float) stacks;
        float phi = V * M_PI;

        for (int j = 0; j <= slices; ++j) {
            float U = j / (float) slices;
            float theta = U * M_PI * 2;

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
        }
    }

    for (int i = 0; i < slices * stacks + slices; ++i) {
        indices.push_back(i);
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

Mesh* Sphere::getMeshPointer()
{
    return sphereMesh;
}

float Sphere::getRadius()
{
    return radius;
}

Sphere::~Sphere()
{
    delete sphereMesh;
}
