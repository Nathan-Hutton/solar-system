#include "OrbitalPhysics.h"

glm::vec3 OrbitalPhysicsFunctions::getForce(SpaceObject *object1, SpaceObject *object2, GLfloat gravitationalForce)
{
    glm::vec3 displacementVector = object1->getPosition() - object2->getPosition();
    glm::vec3 directionVector = glm::normalize(displacementVector);
    float displacementVectorLength = glm::length(displacementVector);
    
    // TODO: fine tune this
    if (object1->getGreatestDistanceBetweenVertices() + object2->getGreatestDistanceBetweenVertices() >= displacementVectorLength)
        return glm::vec3(0.0f,0.0f,0.0f);

    return ((gravitationalForce * object1->getMass() * object2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
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

// TODO: Use the functions in OrbitalPhsyics to make the oldPositions for all objects at once instead of
// one at a time
// REMEMBER
// REMEBER
// REMEMBER
// REMEMBER

void OrbitalPhysicsFunctions::updatePositionsEuler(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat gravitationalForce, GLfloat timeStep)
{
    std::vector<glm::vec3> newSatellitePositions;
    std::vector<glm::vec3> newModelPositions;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 position;
    
    // Apply forces to all planets and moons
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

        acceleration = force / satellites[i]->getMass();
        velocity = satellites[i]->getVelocity() + acceleration * timeStep;
        position = satellites[i]->getPosition() + velocity * timeStep;
        satellites[i]->setVelocity(velocity);
        newSatellitePositions.push_back(position);
    }

    // Apply forces to all models (.obj files)
    for (int i = 0; i < models.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (Sun *star : stars)
            force += getForce(models[i], star, gravitationalForce);

        for (Planet *satellite : satellites)
            force += getForce(models[i], satellite, gravitationalForce);
            
        // Add up forces for other satellites
        // For a less chaotic solar system, comment this loop out
        for (int j = 0; j < models.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(models[i], models[j], gravitationalForce);
        }

        acceleration = force / models[i]->getMass();
        velocity = models[i]->getVelocity() + acceleration * timeStep;
        position = models[i]->getPosition() + velocity * timeStep;
        models[i]->setVelocity(velocity);
        newModelPositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < satellites.size(); i++)
        satellites[i]->setPosition(newSatellitePositions[i]);
    
    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < models.size(); i++)
        models[i]->setPosition(newModelPositions[i]);
}

void OrbitalPhysicsFunctions::updatePositionsVerlet(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, std::vector<Model*>& models, GLfloat gravitationalForce, GLfloat timeStep)
{
    std::vector<glm::vec3> newSatellitePositions;
    std::vector<glm::vec3> newModelPositions;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 position;
    
    // Apply forces to all planets and moons
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

        acceleration = force / satellites[i]->getMass();
        position = 2.0f * satellites[i]->getPosition() - satellites[i]->getOldPosition() + acceleration * pow(timeStep, 2.0f);
        satellites[i]->setOldPosition(satellites[i]->getPosition());
        satellites[i]->setVelocity(velocity);
        newSatellitePositions.push_back(position);
    }

    // Apply forces to all models (.obj files)
    for (int i = 0; i < models.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (Sun *star : stars)
            force += getForce(models[i], star, gravitationalForce);

        for (Planet *satellite : satellites)
            force += getForce(models[i], satellite, gravitationalForce);
            
        // Add up forces for other satellites
        // For a less chaotic solar system, comment this loop out
        for (int j = 0; j < models.size(); j++) 
        {
            if (i == j) continue;
            force += getForce(models[i], models[j], gravitationalForce);
        }

        acceleration = force / models[i]->getMass();
        position = 2.0f * models[i]->getPosition() - models[i]->getOldPosition() + acceleration * pow(timeStep, 2.0f);
        models[i]->setOldPosition(models[i]->getPosition());
        models[i]->setVelocity(velocity);
        newModelPositions.push_back(position);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < satellites.size(); i++)
        satellites[i]->setPosition(newSatellitePositions[i]);
    
    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < models.size(); i++)
        models[i]->setPosition(newModelPositions[i]);
}
