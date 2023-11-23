#include "Sphere.h"

Sphere::Sphere()
{
    this->radius = 0.5f;
    this->density = 1.0f;
    this->mass = 0;
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    positionVector = glm::vec3(x, y, z);
    sphereMesh = new Mesh();
}

Sphere::Sphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices, GLfloat x, GLfloat y, GLfloat z, GLfloat density)
{
    this->radius = radius;
    this->density = density;
    this->mass = (4.0f/3.0f) * M_PI * pow(radius, 3) * density;
    this->x = x;
    this->y = y;
    this->z = z;
    this->positionVector = glm::vec3(x, y, z);
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

GLfloat Sphere::getMass()
{
    return mass;
}

GLfloat Sphere::getRadius()
{
    return radius;
}

glm::vec3 Sphere::getPositionVector() const
{
    return positionVector;
}

GLfloat Sphere::getX()
{
    return x;
}

GLfloat Sphere::getY()
{
    return y;
}

GLfloat Sphere::getZ()
{
    return z;
}

void Sphere::setX(GLfloat x)
{
    this->x = x;
}

void Sphere::setY(GLfloat y)
{
    this->y = y;
}

void Sphere::setZ(GLfloat z)
{
    this->z = z;
}

Sphere::~Sphere()
{
    delete sphereMesh;
}
