#include "Planet.h"

Planet::Planet() : Sphere() {}

Planet::Planet(float radius, GLfloat mass, int stacks, int slices) : Sphere(radius, mass, stacks, slices) {}

Planet::~Planet()
{
    delete sphereMesh, material;
}
