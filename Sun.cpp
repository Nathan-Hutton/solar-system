#include "Sun.h"

Sun::Sun() : Sphere() {}

Sun::Sun(float radius, GLfloat density, glm::vec3 position, int stacks, int slices) : Sphere(radius, density, position, stacks, slices) {}

Sun::~Sun()
{
    delete sphereMesh, position;
}
