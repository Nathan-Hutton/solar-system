
#ifndef ORBITALPHYSICSFUNCTIONS_H
#define ORBITALPHYSICSFUNCTIONS_H

#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "Sun.h"
#include "Planet.h"

namespace OrbitalPhysicsFunctions {
    glm::vec3 getForce(SpaceObject *object1, SpaceObject *object2, GLfloat graviationalForce);
    glm::vec3 getAcceleration(GLfloat mass, glm::vec3 force);
    glm::vec3 getNewVelocity(glm::vec3 oldVelocity, glm::vec3 acceleration, GLfloat timeStep);
    glm::vec3 getNewPosition(glm::vec3 oldPosition, glm::vec3 velocity, GLfloat timeStep);
    void updateSatellitePositions(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat graviationalForce, GLfloat timeStep);
    void updateCelestialBodyAngles(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat timeStep);
}

#endif
