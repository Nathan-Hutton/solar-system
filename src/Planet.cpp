#include "Planet.h"

#include <vector>

Planet::Planet() : Sphere() {}

Planet::Planet(float radius, GLfloat mass, int stacks, int slices) : Sphere(radius, mass, stacks, slices) {}

void Planet::render() const
{
    m_material->useMaterial(m_specularIntensityLocation, m_specularIntensityLocation);
    m_texture->useTexture();
    m_sphereMesh->render();
}

void Planet::setUniformVariables(GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
    m_specularIntensityLocation   = uniformSpecularIntensity;
    m_shininessLocation           = uniformShininess;
}

Planet::~Planet()
{
    delete m_sphereMesh, m_material;
}
