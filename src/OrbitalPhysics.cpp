#include "OrbitalPhysics.h"

glm::vec3 OrbitalPhysicsFunctions::getForce(SpaceObject *object1, SpaceObject *object2, GLfloat gravitationalForce)
{
    glm::vec3 displacementVector = object1->getPosition() - object2->getPosition();
    glm::vec3 directionVector = glm::normalize(displacementVector);
    float displacementVectorLength = glm::length(displacementVector);
    
    // TODO: fine tune this
    if (object1->getGreatestDistanceBetweenVertices() - object2->getGreatestDistanceBetweenVertices() >= displacementVectorLength)
        return glm::vec3(0.0f,0.0f,0.0f);

    return ((gravitationalForce * object1->getMass() * object2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
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
void OrbitalPhysicsFunctions::updateCelestialBodyAngles(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat timeStep)
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
    for (Model *model : models) 
    {
        model->setAngle(model->getAngle() + model->getRotationSpeed() * timeStep);
        if (model->getAngle() >= 360)
            model->setAngle(model->getAngle() - 360);
        if (model->getAngle() <= -360)
            model->setAngle(model->getAngle() + 360);
    }
}
void OrbitalPhysicsFunctions::updateSatellitePositions(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat gravitationalForce, GLfloat timeStep)
{
    std::vector<glm::vec3> newSatellitePositions;
    std::vector<glm::vec3> newModelPositions;
    for (int i = 0; i < satellites.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (Sun *star : stars)
            force += getForce(satellites[i], star, gravitationalForce);

        for (Model *model : models)
            force += getForce(satellites[i], model, gravitationalForce);
            
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
        newSatellitePositions.push_back(newPosition);
    }

    for (int i = 0; i < models.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (Sun *star : stars)
            force += getForce(satellites[i], star, gravitationalForce);

        for (Planet *satellite : satellites)
            force += getForce(models[i], satellite, gravitationalForce);
            
        // Add up forces for other satellites
        // For a less chaotic solar system, comment this loop out
        for (int j = 0; j < models.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(models[i], models[j], gravitationalForce);
        }

        glm::vec3 acceleration = getAcceleration(models[i]->getMass(), force);
        glm::vec3 newVelocity = getNewVelocity(models[i]->getVelocity(), acceleration, timeStep);
        glm::vec3 newPosition = getNewPosition(models[i]->getPosition(), newVelocity, timeStep);
        models[i]->setVelocity(newVelocity);
        newModelPositions.push_back(newPosition);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < satellites.size(); i++)
        satellites[i]->setPosition(newSatellitePositions[i]);
    
    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < models.size(); i++)
        models[i]->setPosition(newModelPositions[i]);
}
