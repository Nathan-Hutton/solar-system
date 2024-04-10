#include "Sphere.h"

Sphere::Sphere() : SpaceObject()
{
    this->radius = 0.5f;
    this->sphereMesh = new Mesh();
    this->texture = NULL;
}

Sphere::Sphere(float radius, GLfloat mass, int stacks, int slices) : SpaceObject(mass)
{
    this->radius = radius;

    // These are default values we'll set with setters
    this->texture = NULL;

    // Create the mesh
    this->sphereMesh = new Mesh();
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    generateSphereData(vertices, indices, radius, stacks, slices);
    this->sphereMesh->createMesh(vertices.data(), indices.data(), vertices.size(), indices.size());
}

void Sphere::generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices)
{
    this->radius = radius;
    for (int i = 0; i <= stacks; ++i) {
        float V = i / static_cast<float>(stacks);
        float phi = V * M_PI;

        for (int j = 0; j <= slices; ++j) {
            float U = j / static_cast<float>(slices);
            float theta = U * M_PI * 2;

            float x = cos(theta) * sin(phi) * radius;
            float y = cos(phi) * radius;
            float z = sin(theta) * sin(phi) * radius;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Calculate texture coordinates
            vertices.push_back(U);
            vertices.push_back(V);

            glm::vec3 normal(x,y,z);
            normal = glm::normalize(normal);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    for (int i = 0; i < slices * stacks + slices; ++i) {
        indices.push_back(i);
        indices.push_back(i + slices);
        indices.push_back(i + slices + 1);

        indices.push_back(i + slices + 1);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
}

void Sphere::render()
{
    texture->useTexture();
    sphereMesh->render();
}

void Sphere::setWorldProperties(glm::mat4* model)
{
    *model = glm::translate(*model, position);
    *model = glm::rotate(*model, glm::radians(angle), rotation);
}

void Sphere::setMeshPointer(Mesh *sphereMesh)
{
    this->sphereMesh = sphereMesh;
}

Texture* Sphere::getTexturePointer()
{
    return texture;
}

void Sphere::setTexturePointer(Texture *texture)
{
    this->texture = texture;
}

Sphere::~Sphere()
{
    delete sphereMesh;
}
