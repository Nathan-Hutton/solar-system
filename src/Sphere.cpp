#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

namespace
{
    void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, int stacks, int slices, GLfloat radius, bool usingNormals)
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

                if (usingNormals)
                {
                    glm::vec3 normal{x,y,z};
                    normal = glm::normalize(-normal);
                    vertices.push_back(normal.x);
                    vertices.push_back(normal.y);
                    vertices.push_back(normal.z);
                }
            }
        }

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = (i * (slices + 1)) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(first + 1);
                indices.push_back(second);

                indices.push_back(second);
                indices.push_back(first + 1);
                indices.push_back(second + 1);
            }
        }
    }
}

Sphere::Sphere(GLfloat mass, GLfloat radius, int stacks, int slices, bool usingNormals) 
    : SpaceObject{mass}
    , m_radius(radius)
    , m_sphereMesh { new Mesh{} }
{
    std::vector<GLfloat> vertices {};
    std::vector<GLuint> indices {};
    generateSphereData(vertices, indices, stacks, slices, radius, usingNormals);
    m_sphereMesh->createMesh(vertices.data(), indices.data(), vertices.size(), indices.size(), usingNormals);
}

void Sphere::setWorldProperties(glm::mat4& model) const
{
    model = glm::translate(model, getPosition());
    model = glm::rotate(model, glm::radians(getAngle()), getRotation());
}
