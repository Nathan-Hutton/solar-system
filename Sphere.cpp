#include "Sphere.h"

Sphere::Sphere()
{
    this->radius = 0.5f;
    this->density = 1.0f;
    this->mass = 0;
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    this->angle = 0.0f;
    this->rotationSpeed = 0.0f;
    this->sphereMesh = new Mesh();
    this->texture = NULL;
    this->material = NULL;
}

Sphere::Sphere(float radius, GLfloat density, glm::vec3 position, int stacks, int slices)
{
    this->radius = radius;
    this->density = density;
    this->mass = (4.0f/3.0f) * M_PI * pow(radius, 3) * density;
    this->position = position;

    // These are default values we'll set with setters
    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    this->angle = 0.0f;
    this->rotationSpeed = 0.0f;
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
        indices.push_back(i + slices + 1);
        indices.push_back(i + slices);

        indices.push_back(i + slices + 1);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
}

void Sphere::renderMesh(GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
    sphereMesh->renderMesh(uniformSpecularIntensity, uniformShininess);
}

Mesh* Sphere::getMeshPointer()
{
    return sphereMesh;
}

Material* Sphere::getMaterialPointer()
{
    return material;
}

GLfloat Sphere::getMass()
{
    return mass;
}

GLfloat Sphere::getRadius()
{
    return radius;
}

glm::vec3 Sphere::getPosition() const
{
    return position;
}

void Sphere::setPosition(glm::vec3 position)
{
    this->position = position;
}

glm::vec3 Sphere::getVelocity() const
{
    return velocity;
}

void Sphere::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

glm::vec3 Sphere::getRotation() const
{
    return rotation;
}

void Sphere::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
}

GLfloat Sphere::getAngle()
{
    return angle;
}

void Sphere::setAngle(GLfloat angle)
{
    this->angle = angle;
}

GLfloat Sphere::getRotationSpeed()
{
    return rotationSpeed;
}

void Sphere::setRotationSpeed(GLfloat speed)
{
    this->rotationSpeed = speed;
}

Texture* Sphere::getTexturePointer()
{
    return texture;
}

void Sphere::setTexturePointer(Texture *texture)
{
    this->texture = texture;
    this->sphereMesh->setTexturePointer(texture);
}

void Sphere::setMaterialPointer(Material *material)
{
    this->material = material;
    this->sphereMesh->setMaterialPointer(material);
}

Sphere::~Sphere()
{
    delete sphereMesh;
}
