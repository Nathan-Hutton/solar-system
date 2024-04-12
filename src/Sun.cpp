#include "Sun.h"

// TODO: Consider removing normals from suns since we don't use them
Sun::Sun() : Sphere() {}

Sun::Sun(float radius, GLfloat mass, int stacks, int slices)
{
    this->mass = mass;
    this->radius = radius;

    // These are default values we'll set with setters
    this->texture = NULL;

    // Create the mesh
    this->sphereMesh = new Mesh();
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    generateSphereData(vertices, indices, radius, stacks, slices);
    this->sphereMesh->createMesh(vertices.data(), indices.data(), vertices.size(), indices.size(), false);
    this->light = NULL;
}

void Sun::generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices)
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

void Sun::render()
{
    texture->useTexture();
    sphereMesh->render();
}

void Sun::setPointLight(GLuint shadowWidth, GLuint shadowHeight,
    GLfloat near, GLfloat far,
    GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat exponential, GLfloat linear, GLfloat constant)
{
    this->light = new PointLight(shadowWidth, shadowHeight,
            near, far,
            red, green, blue, 
            ambientIntensity, diffuseIntensity,
            position.x, position.y, position.z, 
            exponential, linear, constant);
}

void Sun::setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess){}

PointLight* Sun::getPointLight()
{
    return light;
}

Sun::~Sun()
{
    delete sphereMesh, light;
}
