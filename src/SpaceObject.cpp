#include "SpaceObject.h"

SpaceObject::SpaceObject() : mass(1.0f)
{
    this->position  = glm::vec3{0.0f};
    this->velocity  = glm::vec3{0.0f};
    this->rotation  = glm::vec3{0.0f};
}

SpaceObject::SpaceObject(GLfloat mass) : mass(mass)
{
    this->position  = glm::vec3{0.0f};
    this->velocity  = glm::vec3{0.0f};
    this->rotation  = glm::vec3{0.0f};
}

GLfloat SpaceObject::getMass()
{
    return mass;
}

glm::vec3 SpaceObject::getOldPosition() const
{
    return oldPosition;
}

void SpaceObject::setOldPosition(glm::vec3 oldPos)
{
    this->oldPosition = oldPos;
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

void SpaceObject::setMaterialPointer(Material *material)
{
    this->material = material;
}

Material* SpaceObject::getMaterialPointer()
{
    return material;
}

SpaceObject::~SpaceObject()
{}
