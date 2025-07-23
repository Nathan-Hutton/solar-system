#include "Sphere.h"

#include <glm/gtc/matrix_transform.hpp>

Sphere::Sphere(GLfloat mass, GLfloat radius, int stacks, int slices, bool usingNormals, std::shared_ptr<Mesh> sphereMesh) 
    : SpaceObject{ mass }
    , m_radius{ radius }
	, m_sphereMesh{ sphereMesh }
{
    std::vector<GLfloat> vertices {};
    std::vector<GLuint> indices {};
	//m_sphereMesh = Mesh::getSphereMesh(static_cast<float>(radius), stacks, slices, usingNormals);
}

void Sphere::setWorldProperties(glm::mat4& model) const
{
    model = glm::translate(model, getPosition());
    model = glm::rotate(model, glm::radians(getAngle()), getRotation());
}
