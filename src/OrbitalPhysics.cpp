#include "OrbitalPhysics.h"

glm::vec3 OrbitalPhysicsFunctions::getForce(Sphere *sphere1, Sphere *sphere2, GLfloat gravitationalForce)
{
    glm::vec3 displacementVector = sphere1->getPosition() - sphere2->getPosition();
    glm::vec3 directionVector = glm::normalize(displacementVector);
    float displacementVectorLength = glm::length(displacementVector);
    
    if (sphere1->getRadius() + sphere2->getRadius() + 0.27 >= displacementVectorLength)
        return glm::vec3(0.0f,0.0f,0.0f);

    return ((gravitationalForce * sphere1->getMass() * sphere2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
}

glm::vec3 OrbitalPhysicsFunctions::getAcceleration(GLfloat mass, glm::vec3 force)
{
    return force / mass;
}

glm::vec3 OrbitalPhysicsFunctions::getNewVelocity(glm::vec3 oldVelocity, glm::vec3 acceleration, GLfloat timeStep)
{
    return oldVelocity + acceleration * timeStep;
}

glm::vec3 OrbitalPhysicsFunctions::getNewPosition(glm::vec3 oldPosition, glm::vec3 velocity, GLfloat timeStep)
{
    return oldPosition + velocity * timeStep;
}
void OrbitalPhysicsFunctions::updateCelestialBodyAngles(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, GLfloat timeStep)
{
    // Add to angles with increments, adjust so that the numbers don't get too big and cause issues
    for (Sphere *sphere : satellites) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed() * timeStep);
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
    for (Sphere *star : stars) 
    {
        star->setAngle(star->getAngle() + star->getRotationSpeed() * timeStep);
        if (star->getAngle() >= 360)
            star->setAngle(star->getAngle() - 360);
        if (star->getAngle() <= -360)
            star->setAngle(star->getAngle() + 360);
    }
}
void OrbitalPhysicsFunctions::updateSatellitePositions(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, GLfloat gravitationalForce, GLfloat timeStep)
{
    std::vector<glm::vec3> newPositions;
    for (int i = 0; i < satellites.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (Sun *star : stars)
            force += getForce(satellites[i], star, gravitationalForce);
            
        // Add up forces for other satellites
        // For a less chaotic solar system, comment this loop out
        for (int j = 0; j < satellites.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(satellites[i], satellites[j], gravitationalForce);
        }

        glm::vec3 acceleration = getAcceleration(satellites[i]->getMass(), force);
        glm::vec3 newVelocity = getNewVelocity(satellites[i]->getVelocity(), acceleration, timeStep);
        glm::vec3 newPosition = getNewPosition(satellites[i]->getPosition(), newVelocity, timeStep);
        satellites[i]->setVelocity(newVelocity);
        newPositions.push_back(newPosition);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < satellites.size(); i++)
        satellites[i]->setPosition(newPositions[i]);
}
