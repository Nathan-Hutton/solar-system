#include "Material.h"

Material::Material(GLfloat specularIntensity, GLfloat shine)
    : m_specularIntensity { specularIntensity }
    , m_shininess { shine }
{}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) const
{
    glUniform1f(specularIntensityLocation, m_specularIntensity);
    glUniform1f(shininessLocation, m_shininess);
}
