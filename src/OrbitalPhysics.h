#pragma once

#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"

namespace OrbitalPhysics {
    extern bool verlet;
    glm::vec3 getForce(SpaceObject *object1, SpaceObject *object2);
    glm::vec3 getAcceleration(GLfloat mass, glm::vec3 force);
    glm::vec3 getNewVelocity(glm::vec3 oldVelocity, glm::vec3 acceleration, GLfloat timeStep);
    glm::vec3 getNewPosition(glm::vec3 oldPosition, glm::vec3 velocity, GLfloat timeStep);
    void updatePositionsEuler(GLfloat timeStep);
    void updatePositionsVerlet(GLfloat* timeSinceLastUpdate);
    void updateCelestialBodyAngles(GLfloat timeStep);
}
