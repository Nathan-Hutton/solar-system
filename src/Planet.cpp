#include "Planet.h"

#include <vector>

Planet::Planet(float mass, std::shared_ptr<Mesh> sphereMesh, std::shared_ptr<Material> material) 
    : Sphere{ mass, sphereMesh, material } 
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
