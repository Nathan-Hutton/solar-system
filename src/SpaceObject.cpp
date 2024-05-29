#include "SpaceObject.h"

#include <vector>

SpaceObject::SpaceObject() : m_mass { 1.0f }
{
    m_material = nullptr;
    m_position = glm::vec3{0.0f};
    m_velocity = glm::vec3{0.0f};
    m_rotation = glm::vec3{0.0f};
}

SpaceObject::SpaceObject(GLfloat mass) : m_mass { mass }
{
    m_material = nullptr;
    m_position = glm::vec3{0.0f};
    m_velocity = glm::vec3{0.0f};
    m_rotation = glm::vec3{0.0f};
}

GLfloat SpaceObject::getMass() const
{
    return m_mass;
}

const glm::vec3& SpaceObject::getOldPosition() const
{
    return m_oldPosition;
}

void SpaceObject::setOldPosition(glm::vec3 oldPos)
{
    m_oldPosition = oldPos;
}

const glm::vec3& SpaceObject::getPosition() const
{
    return m_position;
}

void SpaceObject::setPosition(glm::vec3 position)
{
    m_position = position;
}

const glm::vec3& SpaceObject::getVelocity() const
{
    return m_velocity;
}

void SpaceObject::setVelocity(glm::vec3 velocity)
{
    m_velocity = velocity;
}

const glm::vec3& SpaceObject::getRotation() const
{
    return m_rotation;
}

void SpaceObject::setRotation(glm::vec3 rotation)
{
    m_rotation = rotation;
}

GLfloat SpaceObject::getAngle() const
{
    return m_angle;
}

void SpaceObject::setAngle(GLfloat angle)
{
    m_angle = angle;
}

GLfloat SpaceObject::getRotationSpeed() const
{
    return m_rotationSpeed;
}

void SpaceObject::setRotationSpeed(GLfloat speed)
{
    m_rotationSpeed = speed;
}

void SpaceObject::setMaterialPointer(Material *material)
{
    m_material = material;
}

SpaceObject::~SpaceObject()
{}
