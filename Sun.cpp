#include "Sun.h"

Sun::Sun() : Sphere() {}

Sun::Sun(float radius, GLfloat mass, glm::vec3 position, int stacks, int slices) : Sphere(radius, mass, position, stacks, slices) {}

void Sun::setPointLightPointer(PointLight *light)
{
    this->light = light;
}

PointLight* Sun::getPointLightPointer()
{
    return light;
}

Sun::~Sun()
{
    delete sphereMesh, position, light;
}
