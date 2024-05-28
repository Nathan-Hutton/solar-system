#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere() : SpaceObject(), m_radius(0.5f)
{
    m_sphereMesh = new Mesh{};
}

Sphere::Sphere(GLfloat radius, GLfloat mass, int stacks, int slices, bool usingNormals) : SpaceObject(mass), m_radius(radius)
{
    m_sphereMesh = new Mesh{};
    std::vector<GLfloat> vertices {};
    std::vector<GLuint> indices {};
    generateSphereData(vertices, indices, stacks, slices, usingNormals);
    m_sphereMesh->createMesh(vertices.data(), indices.data(), vertices.size(), indices.size(), usingNormals);
}

void Sphere::generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int stacks, int slices, bool usingNormals)
{
    for (int i {0}; i <= stacks; ++i) {
        const float V {i / static_cast<float>(stacks)};
        const float phi {V * glm::pi<float>()};

        for (int j {0}; j <= slices; ++j) {
            const float U {j / static_cast<float>(slices)};
            const float theta {U * glm::pi<float>() * 2};

            const float x {cosf(theta) * sinf(phi) * m_radius};
            const float y {cosf(phi) * m_radius};
            const float z {sinf(theta) * sinf(phi) * m_radius};

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Calculate texture coordinates
            vertices.push_back(U);
            vertices.push_back(V);

            if (usingNormals)
            {
                glm::vec3 normal{x,y,z};
                normal = glm::normalize(normal);
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
            }
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

void Sphere::setWorldProperties(glm::mat4& model)
{
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_angle), m_rotation);
}

void Sphere::setTexturePointer(Texture *texture)
{
    m_texture = texture;
}

GLfloat Sphere::getCollisionDistance() const
{
    return m_radius;
}

Sphere::~Sphere()
{
    delete m_sphereMesh;
}
