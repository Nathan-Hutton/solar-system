#include "Sun.h"

Sun::Sun() : Sphere() {}

Sun::Sun(float radius, GLfloat mass, int stacks, int slices) : Sphere(radius, mass, stacks, slices) 
{
    this->light = NULL;
}

void Sun::setPointLight(GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat exponential, GLfloat linear, GLfloat constant)
{
    this->light = new PointLight(red, green, blue, 
            ambientIntensity, diffuseIntensity,
            position.x, position.y, position.z, 
            exponential, linear, constant);
}

PointLight* Sun::getPointLight()
{
    return light;
}

Sun::~Sun()
{
    delete sphereMesh, light;
}
