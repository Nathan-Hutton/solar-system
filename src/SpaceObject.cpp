#include "SpaceObject.h"

SpaceObject::SpaceObject()
{
    this->mass = 1.0f;
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    this->angle = 0.0f;
    this->rotationSpeed = 0.0f;
    this->greatestDistanceBetweenVertices = 0.0f;
}

SpaceObject::SpaceObject(GLfloat mass)
{
    this->mass = mass;

    // These are default values we'll set with setters
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    this->angle = 0.0f;
    this->rotationSpeed = 0.0f;
    this->greatestDistanceBetweenVertices = 0.0f;
}

GLfloat SpaceObject::getMass()
{
    return mass;
}

glm::vec3 SpaceObject::getPosition() const
{
    return position;
}

void SpaceObject::setPosition(glm::vec3 position)
{
    this->position = position;
}

glm::vec3 SpaceObject::getVelocity() const
{
    return velocity;
}

void SpaceObject::setVelocity(glm::vec3 velocity)
{
    this->velocity = velocity;
}

glm::vec3 SpaceObject::getRotation() const
{
    return rotation;
}

void SpaceObject::setRotation(glm::vec3 rotation)
{
    this->rotation = rotation;
}

GLfloat SpaceObject::getAngle()
{
    return angle;
}

void SpaceObject::setAngle(GLfloat angle)
{
    this->angle = angle;
}

GLfloat SpaceObject::getRotationSpeed()
{
    return rotationSpeed;
}

void SpaceObject::setRotationSpeed(GLfloat speed)
{
    this->rotationSpeed = speed;
}

GLfloat SpaceObject::getGreatestDistanceBetweenVertices()
{
    return greatestDistanceBetweenVertices;
}

SpaceObject::~SpaceObject()
{}
