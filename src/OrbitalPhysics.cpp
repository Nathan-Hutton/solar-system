#include "OrbitalPhysics.h"

float MAX_TIME_STEP = 0.005f;
float gForce = -100.0f;

glm::vec3 OrbitalPhysicsFunctions::getForce(SpaceObject *object1, SpaceObject *object2)
{
    glm::vec3 displacementVector {object1->getPosition() - object2->getPosition()};
    glm::vec3 directionVector {glm::normalize(displacementVector)};
    float displacementVectorLength {glm::length(displacementVector)};
    
    // TODO: fine tune this
    if (object1->getGreatestDistanceBetweenVertices() + object2->getGreatestDistanceBetweenVertices() >= displacementVectorLength)
        return glm::vec3{0.0f,0.0f,0.0f};

    return ((gForce * object1->getMass() * object2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
}

void OrbitalPhysicsFunctions::updateCelestialBodyAngles(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites, GLfloat timeStep)
{
    // Add to angles with increments, adjust so that the numbers don't get too big and cause issues
    for (SpaceObject *sphere : satellites) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed() * timeStep);
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
    for (SpaceObject *star : stars) 
    {
        star->setAngle(star->getAngle() + star->getRotationSpeed() * timeStep);
        if (star->getAngle() >= 360)
            star->setAngle(star->getAngle() - 360);
        if (star->getAngle() <= -360)
            star->setAngle(star->getAngle() + 360);
    }
}

void OrbitalPhysicsFunctions::updatePositionsEuler(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites, GLfloat timeStep)
{
    float tStep;
    if (timeStep > MAX_TIME_STEP)
        tStep = MAX_TIME_STEP;
    else
        tStep = timeStep;

    std::vector<glm::vec3> newSatellitePositions;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 position;
    
    // Apply forces to all planets and moons
    for (int i {0}; i < satellites.size(); i++) 
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (SpaceObject *star : stars)
            force += getForce(satellites[i], star);

        // Add up forces for other satellites
        for (int j {0}; j < satellites.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(satellites[i], satellites[j]);
        }

        acceleration = force / satellites[i]->getMass();
        velocity = satellites[i]->getVelocity() + acceleration * tStep;
        position = satellites[i]->getPosition() + velocity * tStep;
        satellites[i]->setVelocity(velocity);
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i {0}; i < satellites.size(); i++)
        satellites[i]->setPosition(newSatellitePositions[i]);

    if (timeStep > MAX_TIME_STEP)
        updatePositionsEuler(stars, satellites, timeStep - MAX_TIME_STEP);
}

void OrbitalPhysicsFunctions::updatePositionsVerlet(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites, GLfloat* timeSinceLastUpdate)
{
    // Only do these calculations it's been a whole 0.005f seconds since the last time we ran this
    if (glfwGetTime() - *timeSinceLastUpdate < MAX_TIME_STEP)
        return;

    (*timeSinceLastUpdate) += MAX_TIME_STEP;

    std::vector<glm::vec3> newSatellitePositions;
    glm::vec3 acceleration;
    glm::vec3 position;
    
    // Apply forces to all planets and moons
    for (int i {0}; i < satellites.size(); i++) 
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (SpaceObject *star : stars)
            force += getForce(satellites[i], star);
            
        // Add up forces for other satellites
        for (int j {0}; j < satellites.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(satellites[i], satellites[j]);
        }

        acceleration = force / satellites[i]->getMass();
        position = 2.0f * satellites[i]->getPosition() - satellites[i]->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f);
        satellites[i]->setOldPosition(satellites[i]->getPosition());
        newSatellitePositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i {0}; i < satellites.size(); i++)
        satellites[i]->setPosition(newSatellitePositions[i]);

    // Keep doing these calculations until I can no longer do full 0.005f timesteps
    updatePositionsVerlet(stars, satellites, timeSinceLastUpdate);
}
