#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(GLfloat mass, std::shared_ptr<Mesh> sphereMesh, std::shared_ptr<Material> material) 
    : SpaceObject{ mass, material }
	, m_sphereMesh{ sphereMesh }
{}

void Sphere::setWorldProperties(glm::mat4& model) const
{
    model = glm::translate(model, getPosition());
    model = glm::rotate(model, glm::radians(getAngle()), getRotation());
}
