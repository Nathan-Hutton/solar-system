#include "Planet.h"

Planet::Planet() : Sphere() {}

Planet::Planet(float radius, GLfloat mass, glm::vec3 position, int stacks, int slices) : Sphere(radius, mass, position, stacks, slices) {}

void Planet::setMaterialPointer(Material *material)
{
    this->material = material;
}

Material* Planet::getMaterialPointer()
{
    return material;
}

Planet::~Planet()
{
    delete sphereMesh, position, material;
}
