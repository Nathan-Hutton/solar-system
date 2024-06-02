#include "OrbitalPhysics.h"

#include <vector>

#include <GLFW/glfw3.h>

#include "Scene.h"

namespace
{
    constexpr float MAX_TIME_STEP {0.005f};
    constexpr float gForce {-100.0f};
}

bool orbitalPhysics::verlet = false;

glm::vec3 orbitalPhysics::getForce(const SpaceObject* const object1, const SpaceObject* const object2)
{
    const glm::vec3 displacementVector {object1->getPosition() - object2->getPosition()};
    const float displacementVectorLength {glm::length(displacementVector)};
    
    // TODO: fine tune this
    if (object1->getCollisionDistance() + object2->getCollisionDistance() >= displacementVectorLength)
        return glm::vec3{0.0f};

    return ((gForce * object1->getMass() * object2->getMass()) / (float)pow(displacementVectorLength, 2)) * glm::normalize(displacementVector);
}

void orbitalPhysics::updateCelestialBodyAngles(GLfloat timeStep)
{
    // Add to angles with increments, adjust so that the numbers don't get too big and cause issues
    for (SpaceObject *sphere : scene::satellites) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed() * timeStep);
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
    for (SpaceObject *star : scene::stars) 
    {
        star->setAngle(star->getAngle() + star->getRotationSpeed() * timeStep);
        if (star->getAngle() >= 360)
            star->setAngle(star->getAngle() - 360);
        if (star->getAngle() <= -360)
            star->setAngle(star->getAngle() + 360);
    }
}

void orbitalPhysics::updatePositionsEuler(GLfloat timeStep)
{
    const float tStep { (timeStep > MAX_TIME_STEP) ? MAX_TIME_STEP : timeStep };
    std::vector<glm::vec3> newSatellitePositions {};
    glm::vec3 acceleration {};
    glm::vec3 velocity {};
    glm::vec3 position {};
    
    // Apply forces to all planets and moons
    for (SpaceObject* satellite1 : scene::satellites)
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (const SpaceObject* star : scene::stars)
            force += getForce(satellite1, star);

        // Add up forces for other satellites
        for (const SpaceObject* satellite2 : scene::satellites)
        {
            if (satellite1 == satellite2) continue;
            force += getForce(satellite1, satellite2);
        }

        acceleration    = force / satellite1->getMass();
        velocity        = satellite1->getVelocity() + acceleration * tStep;
        position        = satellite1->getPosition() + velocity * tStep;
        satellite1->setVelocity(velocity);
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (size_t i {0}; i < scene::satellites.size(); ++i)
        scene::satellites[i]->setPosition(newSatellitePositions[i]);

    if (timeStep > MAX_TIME_STEP)
        updatePositionsEuler(timeStep - MAX_TIME_STEP);
}

void orbitalPhysics::updatePositionsVerlet(GLfloat& timeSinceLastUpdate)
{
    // Only do these calculations it's been a whole 0.005f seconds since the last time we ran this
    if (glfwGetTime() - timeSinceLastUpdate < MAX_TIME_STEP)
        return;

    timeSinceLastUpdate += MAX_TIME_STEP;

    std::vector<glm::vec3> newSatellitePositions {};
    glm::vec3 acceleration {};
    glm::vec3 position {};
    
    // Apply forces to all planets and moons
    for (SpaceObject* satellite1 : scene::satellites)
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (const SpaceObject* star : scene::stars)
            force += getForce(satellite1, star);
            
        for (const SpaceObject* satellite2 : scene::satellites)
        {
            if (satellite1 == satellite2) continue;
            force += getForce(satellite1, satellite2);
        }

        acceleration    = force / satellite1->getMass();
        position        = 2.0f * satellite1->getPosition() - satellite1->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f);
        satellite1->setOldPosition(satellite1->getPosition());
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (size_t i {0}; i < scene::satellites.size(); ++i)
        scene::satellites[i]->setPosition(newSatellitePositions[i]);

    // Keep doing these calculations until I can no longer do full 0.005f timesteps
    updatePositionsVerlet(timeSinceLastUpdate);
}
