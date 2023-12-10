#include "Material.h"

Material::Material()
{
    specularIntensity = 0.0f;
    shininess = 0.0f;
}

Material::Material(GLfloat specularIntensity, GLfloat shine)
{
    this->specularIntensity = specularIntensity;
    this->shininess = shine;
}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation)
{
    glUniform1f(specularIntensityLocation, specularIntensity);
    glUniform1f(shininessLocation, shininess);
}

Material::~Material(){}