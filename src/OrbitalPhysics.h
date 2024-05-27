#pragma once

#include <math.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "SpaceObject.h"

namespace OrbitalPhysics {
    extern bool verlet;
    glm::vec3 getForce(const SpaceObject* const object1, const SpaceObject* const object2);
    glm::vec3 getAcceleration(GLfloat mass, glm::vec3 force);
    glm::vec3 getNewVelocity(glm::vec3 oldVelocity, glm::vec3 acceleration, GLfloat timeStep);
    glm::vec3 getNewPosition(glm::vec3 oldPosition, glm::vec3 velocity, GLfloat timeStep);
    void updatePositionsEuler(GLfloat timeStep);
    void updatePositionsVerlet(GLfloat& timeSinceLastUpdate);
    void updateCelestialBodyAngles(GLfloat timeStep);
}
