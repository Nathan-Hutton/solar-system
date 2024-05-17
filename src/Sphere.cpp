#include "Sphere.h"

Sphere::Sphere() : SpaceObject(), radius(0.5f)
{
    this->sphereMesh = new Mesh{};
}

Sphere::Sphere(GLfloat radius, GLfloat mass, int stacks, int slices) : SpaceObject(mass), radius(radius)
{
    this->greatestDistanceBetweenVertices = radius * 2;
    this->sphereMesh = new Mesh{};
}

void Sphere::generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int stacks, int slices)
{
    for (int i {0}; i <= stacks; ++i) {
        const float V {i / static_cast<float>(stacks)};
        const float phi {V * glm::pi<float>()};

        for (int j {0}; j <= slices; ++j) {
            const float U {j / static_cast<float>(slices)};
            const float theta {U * glm::pi<float>() * 2};

            const float x {cosf(theta) * sinf(phi) * radius};
            const float y {cosf(phi) * radius};
            const float z {sinf(theta) * sinf(phi) * radius};

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Calculate texture coordinates
            vertices.push_back(U);
            vertices.push_back(V);

            glm::vec3 normal{x,y,z};
            normal = glm::normalize(normal);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    for (int i {0}; i < slices * stacks + slices; ++i) {
        indices.push_back(i);
        indices.push_back(i + slices);
        indices.push_back(i + slices + 1);

        indices.push_back(i + slices + 1);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
}

void Sphere::setWorldProperties(glm::mat4* model)
{
    *model = glm::translate(*model, position);
    *model = glm::rotate(*model, glm::radians(angle), rotation);
}

void Sphere::setTexturePointer(Texture *texture)
{
    this->texture = texture;
}

Sphere::~Sphere()
{
    delete sphereMesh;
}
