#include "Planet.h"

#include <vector>

Planet::Planet(float mass, Material* material, GLfloat radius, int stacks, int slices) 
    : Sphere { mass, radius, stacks, slices } 
    , m_material { material }
{}

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
