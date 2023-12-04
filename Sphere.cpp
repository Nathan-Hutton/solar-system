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

            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);

            // Calculate texture coordinates
            vertices.push_back(U);
            vertices.push_back(V);

            // Set placeholder normal values
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
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

    calcAverageNormals(indices.data(), indices.size(), vertices.data(), vertices.size(), 8, 5);
}

void Sphere::calcAverageNormals(unsigned int *indices, unsigned int indexCount, GLfloat *vertices, unsigned int vertexCount, unsigned int vLen, unsigned int normalOffset)
{
    // Go through all the triangles
    for (size_t i = 0; i < indexCount; i+=3)
    {
        // Get each index of the triangle. Multiply it by vLen (8) so that we're pointing at the right vertex
        unsigned int in0 = indices[i] * vLen;
        unsigned int in1 = indices[i + 1] * vLen;
        unsigned int in2 = indices[i + 2] * vLen;

        // Create 2 edges of a triangle. Each line is the equivalent to subtracting the 2 points to get a vector.
        // We are using all 3 points to make 2 edges of a triangle so we can get the surface normal
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        
        // Get the cross product of these 2 edges and normalize to get the surface normal. 
        // We want to normalize since we're only concerned with the direction
        glm::vec3 normal = cross(v1, v2);
        normal = glm::normalize(normal);

        // Now we just update the surface normal in the vertices list. 
        // This will accumulate for vertices that are connected to multiple triangles.
        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    // Normalize the vertex normals. 
    for (size_t i = 0; i < vertexCount / vLen; i++)
    {
        unsigned int nOffset = i * vLen + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}

void Sphere::renderMesh()
{
    sphereMesh->renderMesh();
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

Sphere::~Sphere()
{
    delete sphereMesh;
}
