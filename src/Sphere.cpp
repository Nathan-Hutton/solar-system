#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(GLfloat mass, std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, std::shared_ptr<Material> material) 
    : SpaceObject{ mass, mesh, texture, material }
{}

void Sphere::setWorldProperties(glm::mat4& model) const
{
    model = glm::translate(model, getPosition());
    model = glm::rotate(model, glm::radians(getAngle()), getRotation());
}
