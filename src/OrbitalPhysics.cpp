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
    for (std::unique_ptr<SpaceObject>& sphere : scene::satellites) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed() * timeStep);
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
    for (std::unique_ptr<SpaceObject>& star : scene::stars) 
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
    glm::vec3* newSatellitePositions { new glm::vec3[scene::satellites.size()] };
    
    // Apply forces to all planets and moons
    for (size_t i { 0 }; i < scene::satellites.size(); ++i)
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (size_t j { 0 }; j < scene::stars.size(); ++j)
            force += getForce(scene::satellites[i].get(), scene::stars[j].get());

        // Add up forces for other satellites
        for (size_t j { 0 }; j < scene::satellites.size(); ++j)
        {
            if (i == j) continue;
            force += getForce(scene::satellites[i].get(), scene::satellites[j].get());
        }

		const glm::vec3 acceleration{ force / scene::satellites[i]->getMass() };
		const glm::vec3 velocity{ scene::satellites[i]->getVelocity() + acceleration * tStep };
		const glm::vec3 position{ scene::satellites[i]->getPosition() + velocity * tStep };
        scene::satellites[i]->setVelocity(velocity);
        newSatellitePositions[i] = position;
    }

    glm::vec3* newStarPositions{ new glm::vec3[scene::stars.size()] };
    for (size_t i { 0 }; i < scene::stars.size(); ++i)
    {
        glm::vec3 force {0};

        // Add up forces for other stars
        for (size_t j { i+1 }; j < scene::stars.size(); ++j)
            force += getForce(scene::stars[i].get(), scene::stars[j].get());
        
        // Add up forces for other satellites
        for (size_t j { 0 }; j < scene::satellites.size(); ++j)
            force += getForce(scene::stars[i].get(), scene::satellites[j].get());

		const glm::vec3 acceleration{ force / scene::stars[i]->getMass() };
		const glm::vec3 velocity{ scene::stars[i]->getVelocity() + acceleration * tStep };
		const glm::vec3 position{ scene::stars[i]->getPosition() + velocity * tStep };
        scene::stars[i]->setVelocity(velocity);
        newStarPositions[i] = position;
    }

	// Double buffering
    for (size_t i{ 0 }; i < scene::satellites.size(); ++i)
        scene::satellites[i]->setPositionMove(newSatellitePositions[i]);
    for (size_t i{ 0 }; i < scene::stars.size(); ++i)
        scene::stars[i]->setPositionMove(newStarPositions[i]);

    if (timeStep > MAX_TIME_STEP)
        updatePositionsEuler(timeStep - MAX_TIME_STEP);

    delete[] newSatellitePositions;
	delete[] newStarPositions;
}

void orbitalPhysics::updatePositionsVerlet(GLfloat& timeSinceLastUpdate)
{
    // Only do these calculations it's been a whole 0.005f seconds since the last time we ran this
    if (glfwGetTime() - timeSinceLastUpdate < MAX_TIME_STEP)
        return;

    timeSinceLastUpdate += MAX_TIME_STEP;

    // Apply forces to all planets and moons
    glm::vec3* newSatellitePositions { new glm::vec3[scene::satellites.size()] };
    for (size_t i{ 0 }; i < scene::satellites.size(); ++i)
    {
        glm::vec3 force {0};
        
        // Add up forces from stars
        for (std::unique_ptr<SpaceObject>& star : scene::stars)
            force += getForce(scene::satellites[i].get(), star.get());
            
        for (size_t j{ 0 }; j < scene::satellites.size(); ++j)
        {
            if (scene::satellites[i] == scene::satellites[j]) continue;
            force += getForce(scene::satellites[i].get(), scene::satellites[j].get());
        }

        const glm::vec3 acceleration{ force / scene::satellites[i]->getMass() };
        const glm::vec3 position{ 2.0f * scene::satellites[i]->getPosition() - scene::satellites[i]->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f) };
        scene::satellites[i]->setOldPosition(scene::satellites[i]->getPosition());
        newSatellitePositions[i] = position;
    }

	// Apply forces to stars
    glm::vec3* newStarPositions{ new glm::vec3[scene::stars.size()] };
    for (size_t i{ 0 }; i < scene::stars.size(); ++i)
    {
        glm::vec3 force{ 0 };
        
        // Add up forces from stars
        for (size_t j{ i+1 }; j < scene::stars.size(); ++j)
            force += getForce(scene::stars[i].get(), scene::stars[j].get());
            
        // Add up forces from planets
        for (size_t j{ 0 }; j < scene::satellites.size(); ++j)
            force += getForce(scene::stars[i].get(), scene::satellites[j].get());

        const glm::vec3 acceleration{ force / scene::stars[i]->getMass() };
        const glm::vec3 position{ 2.0f * scene::stars[i]->getPosition() - scene::stars[i]->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f) };
        scene::stars[i]->setOldPosition(scene::stars[i]->getPosition());
        newStarPositions[i] = position;
    }

	// Double buffering
    for (size_t i {0}; i < scene::satellites.size(); ++i)
        scene::satellites[i]->setPositionMove(newSatellitePositions[i]);
    for (size_t i {0}; i < scene::stars.size(); ++i)
        scene::stars[i]->setPositionMove(newStarPositions[i]);

    // Keep doing these calculations until I can no longer do full 0.005f timesteps
    updatePositionsVerlet(timeSinceLastUpdate);

    delete[] newSatellitePositions;
	delete[] newStarPositions;
}
