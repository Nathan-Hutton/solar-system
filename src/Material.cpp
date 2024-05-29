#include "Material.h"

Material::Material()
    : m_specularIntensity { 0.0f }
    , m_shininess { 0.0f }
{}

Material::Material(GLfloat specularIntensity, GLfloat shine)
    : m_specularIntensity { specularIntensity }
    , m_shininess { shine }
{}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) const
{
    glUniform1f(specularIntensityLocation, m_specularIntensity);
    glUniform1f(shininessLocation, m_shininess);
}

Material::~Material(){}
