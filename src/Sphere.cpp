#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(GLfloat mass, GLfloat radius, std::shared_ptr<Mesh> sphereMesh) 
    : SpaceObject{ mass }
    , m_radius{ radius }
	, m_sphereMesh{ sphereMesh }
{
    std::vector<GLfloat> vertices {};
    std::vector<GLuint> indices {};
}

void Sphere::setWorldProperties(glm::mat4& model) const
{
    model = glm::translate(model, getPosition());
    model = glm::rotate(model, glm::radians(getAngle()), getRotation());
}
