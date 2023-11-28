#include "Planet.h"

Planet::Planet() : Sphere()
{
    this->parentSun = NULL;
}

Planet::Planet(float radius, GLfloat density, Sun *parentSun, glm::vec3 position, int stacks, int slices) : Sphere(radius, density, position, stacks, slices)
{
    this->parentSun = parentSun;
}

Planet::~Planet()
{
    delete sphereMesh;
}
