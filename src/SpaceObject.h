#pragma once

#include <utility>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Material.h"

class SpaceObject
{
    public:
        explicit SpaceObject(GLfloat mass=1.0f, std::shared_ptr<Material> material=nullptr) 
            : m_mass{ mass }
			, m_material{ material }
            , m_position{ 0.0f }
            , m_velocity{ 0.0f }
            , m_rotation{ 0.0f }
        {}

        GLfloat getMass() const { return m_mass; }
        const glm::vec3& getOldPosition() const { return m_oldPosition; }
        void setOldPosition(glm::vec3 oldPos) { m_oldPosition = oldPos; }
        const glm::vec3& getPosition() const { return m_position; }
        void setPosition(glm::vec3 position) { m_position = position; }
        void setPositionMove(glm::vec3& position) { m_position = std::move(position); }
        const glm::vec3& getVelocity() const { return m_velocity; }
        void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }
        const glm::vec3& getRotation() const { return m_rotation; }
        void setRotation(glm::vec3 rotation) { m_rotation = rotation; }
        GLfloat getAngle() const { return m_angle; }
        void setAngle(GLfloat angle) { m_angle = angle; }
        GLfloat getRotationSpeed() const { return m_rotationSpeed; }
        void setRotationSpeed(GLfloat speed) { m_rotationSpeed = speed; }
        virtual void render() const = 0;
        virtual void setWorldProperties(glm::mat4& model) const = 0;
        virtual void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) = 0;
		void setCollisionDistance(float collisionDistance) { m_collisionDistance = collisionDistance; }
        GLfloat getCollisionDistance() const { return m_collisionDistance; }

        virtual ~SpaceObject() = default;

    private:
        const GLfloat m_mass{};

	protected:
        std::shared_ptr<Material> m_material;

	private:
        glm::vec3 m_position{};
        glm::vec3 m_velocity{};
        glm::vec3 m_rotation{};
        glm::vec3 m_oldPosition{};
        GLfloat m_angle{};
        GLfloat m_rotationSpeed{};
		float m_collisionDistance{};

};
