#include "Moon.h"

Moon::Moon() : Sphere()
{
    this->parentPlanet = NULL;
}

Moon::Moon(float radius, GLfloat density, Planet *parentPlanet, glm::vec3 position, int stacks, int slices) : Sphere(radius, density, position, stacks, slices)
{
    this->parentPlanet = parentPlanet;
}

Moon::~Moon()
{
    delete sphereMesh;
}
