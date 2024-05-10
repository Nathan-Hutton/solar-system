#include "OrbitalPhysics.h"

namespace OrbitalPhysics {
    bool verlet = false;
}

float MAX_TIME_STEP {0.005f};
float gForce {-100.0f};

glm::vec3 OrbitalPhysics::getForce(SpaceObject *object1, SpaceObject *object2)
{
    glm::vec3 displacementVector {object1->getPosition() - object2->getPosition()};
    glm::vec3 directionVector {glm::normalize(displacementVector)};
    float displacementVectorLength {glm::length(displacementVector)};
    
    // TODO: fine tune this
    if (object1->getGreatestDistanceBetweenVertices() + object2->getGreatestDistanceBetweenVertices() >= displacementVectorLength)
        return glm::vec3{0.0f};

    return ((gForce * object1->getMass() * object2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
}

void OrbitalPhysics::updateCelestialBodyAngles(GLfloat timeStep)
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

void OrbitalPhysics::updatePositionsEuler(GLfloat timeStep)
{
    float tStep {};
    if (timeStep > MAX_TIME_STEP)
        tStep = MAX_TIME_STEP;
    else
        tStep = timeStep;

    std::vector<glm::vec3> newSatellitePositions {};
    glm::vec3 acceleration {};
    glm::vec3 velocity {};
    glm::vec3 position {};
    
    // Apply forces to all planets and moons
    for (int i {0}; i < scene::satellites.size(); i++) 
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (SpaceObject *star : scene::stars)
            force += getForce(scene::satellites[i], star);

        // Add up forces for other satellites
        for (int j {0}; j < scene::satellites.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(scene::satellites[i], scene::satellites[j]);
        }

        acceleration    = force / scene::satellites[i]->getMass();
        velocity        = scene::satellites[i]->getVelocity() + acceleration * tStep;
        position        = scene::satellites[i]->getPosition() + velocity * tStep;
        scene::satellites[i]->setVelocity(velocity);
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i {0}; i < scene::satellites.size(); i++)
        scene::satellites[i]->setPosition(newSatellitePositions[i]);

    if (timeStep > MAX_TIME_STEP)
        updatePositionsEuler(timeStep - MAX_TIME_STEP);
}

void OrbitalPhysics::updatePositionsVerlet(GLfloat* timeSinceLastUpdate)
{
    // Only do these calculations it's been a whole 0.005f seconds since the last time we ran this
    if (glfwGetTime() - *timeSinceLastUpdate < MAX_TIME_STEP)
        return;

    (*timeSinceLastUpdate) += MAX_TIME_STEP;

    std::vector<glm::vec3> newSatellitePositions {};
    glm::vec3 acceleration {};
    glm::vec3 position {};
    
    // Apply forces to all planets and moons
    for (int i {0}; i < scene::satellites.size(); i++) 
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (SpaceObject *star : scene::stars)
            force += getForce(scene::satellites[i], star);
            
        // Add up forces for other scene::satellites
        for (int j {0}; j < scene::satellites.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(scene::satellites[i], scene::satellites[j]);
        }

        acceleration    = force / scene::satellites[i]->getMass();
        position        = 2.0f * scene::satellites[i]->getPosition() - scene::satellites[i]->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f);
        scene::satellites[i]->setOldPosition(scene::satellites[i]->getPosition());
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i {0}; i < scene::satellites.size(); i++)
        scene::satellites[i]->setPosition(newSatellitePositions[i]);

    // Keep doing these calculations until I can no longer do full 0.005f timesteps
    updatePositionsVerlet(timeSinceLastUpdate);
}
