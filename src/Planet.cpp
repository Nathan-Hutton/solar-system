#include "Planet.h"

Planet::Planet() : Sphere() {}

Planet::Planet(float radius, GLfloat mass, int stacks, int slices) : Sphere(radius, mass, stacks, slices) {}

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
    delete sphereMesh, material;
}
