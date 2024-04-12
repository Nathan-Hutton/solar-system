#include "Planet.h"

Planet::Planet() : Sphere() {}

Planet::Planet(float radius, GLfloat mass, int stacks, int slices) : Sphere(radius, mass, stacks, slices) {}

void Planet::render()
{
    material->useMaterial(specularIntensityLocation, specularIntensityLocation);
    texture->useTexture();
    sphereMesh->render();
}

void Planet::setUniformVariables(GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
    specularIntensityLocation = uniformSpecularIntensity;
    shininessLocation = uniformShininess;
}

Planet::~Planet()
{
    delete sphereMesh, material;
}
