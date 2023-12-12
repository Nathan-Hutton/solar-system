#include "Sun.h"

Sun::Sun() : Sphere() {}

Sun::Sun(float radius, GLfloat mass, glm::vec3 position, int stacks, int slices) : Sphere(radius, mass, position, stacks, slices) {}

Sun::~Sun()
{
    delete sphereMesh, position;
}
