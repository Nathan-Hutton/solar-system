#include "OrbitalPhysics.h"

#include <vector>

#include <GLFW/glfw3.h>

#include "Scene.h"

namespace
{
    constexpr float MAX_TIME_STEP {0.005f};
    constexpr float gForce {-100.0f};

	void accumulateNetForces(std::vector<glm::vec3>& netForces)
	{
		for (size_t i{ 0 }; i < scene::movables.size(); ++i)
		{
			for (size_t j{ i + 1 }; j < scene::movables.size(); ++j)
			{
				const glm::vec3 force{ orbitalPhysics::getForce(scene::movables[i].get(), scene::movables[j].get()) };
				netForces[i] += force;
				netForces[j] -= force;
			}
		}
	}
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
    for (std::shared_ptr<SpaceObject>& sphere : scene::movables) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed() * timeStep);
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
}

void orbitalPhysics::updatePositionsEuler(GLfloat timeStep)
{
	const float tStep{ std::min(timeStep, MAX_TIME_STEP) };
	std::vector<glm::vec3> netForces(scene::movables.size(), glm::vec3{ 0.0f });
	accumulateNetForces(netForces);

	// Calculate new velocities and positions
	for (size_t i{ 0 }; i < scene::movables.size(); ++i)
	{
		const glm::vec3 acceleration{ netForces[i] / scene::movables[i]->getMass() };
		const glm::vec3 velocity{ scene::movables[i]->getVelocity() + acceleration * tStep };
		const glm::vec3 position{ scene::movables[i]->getPosition() + velocity * tStep };
		scene::movables[i]->setPosition(position);
		scene::movables[i]->setVelocity(velocity);
	}

    if (timeStep > MAX_TIME_STEP)
        updatePositionsEuler(timeStep - MAX_TIME_STEP);
}

void orbitalPhysics::updatePositionsVerlet(GLfloat& timeSinceLastUpdate)
{
    // Only do these calculations it's been a whole 0.005f seconds since the last time we ran this
    if (glfwGetTime() - timeSinceLastUpdate < MAX_TIME_STEP)
        return;

    timeSinceLastUpdate += MAX_TIME_STEP;

    // Apply forces to all planets and moons
	std::vector<glm::vec3> netForces(scene::movables.size(), glm::vec3{ 0.0f });
	accumulateNetForces(netForces);
	
	for (size_t i{ 0 }; i < scene::movables.size(); ++i)
	{
		const glm::vec3 acceleration{ netForces[i] / scene::movables[i]->getMass() };
		const glm::vec3 position{ 2.0f * scene::movables[i]->getPosition() - scene::movables[i]->getOldPosition() + acceleration * pow(MAX_TIME_STEP, 2.0f) };
		scene::movables[i]->setOldPosition(scene::movables[i]->getPosition());
		scene::movables[i]->setPosition(position);
	}

    // Keep doing these calculations until I can no longer do full 0.005f timesteps
    updatePositionsVerlet(timeSinceLastUpdate);
}
