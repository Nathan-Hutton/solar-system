#pragma once

#include <utility>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "PointLight.h"

class SpaceObject
{
    public:
        explicit SpaceObject(
				GLfloat mass=1.0f,
				bool objectGlows=false,
				std::shared_ptr<Mesh> mesh=nullptr,
				std::shared_ptr<Texture> texture=nullptr,
				std::shared_ptr<Material> material=nullptr,
				std::shared_ptr<PointLight> pLight=nullptr
			) 
            : m_mass{ mass }
			, m_mesh{ mesh }
			, m_objectGlows{ objectGlows }
			, m_texture{ texture }
			, m_material{ material }
            , m_position{ 0.0f }
            , m_velocity{ 0.0f }
            , m_rotation{ 0.0f }
			, m_pLight{ pLight }
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
		std::shared_ptr<Material> getMaterial() { return m_material; }
		std::shared_ptr<PointLight> getPointLight() const { return m_pLight; }
        void render() const 
		{ 
			m_texture->useTexture();
			m_mesh->render();
		}
        void setWorldProperties(glm::mat4& model) const
		{
			model = glm::translate(model, getPosition());
			model = glm::rotate(model, glm::radians(m_angle), m_rotation);
		}

		void setCollisionDistance(float collisionDistance) { m_collisionDistance = collisionDistance; }
        GLfloat getCollisionDistance() const { return m_collisionDistance; }

        ~SpaceObject() {}

    private:
        const GLfloat m_mass{};
		std::shared_ptr<Mesh> m_mesh{};
		const bool m_objectGlows{};
        std::shared_ptr<Texture> m_texture{};
        std::shared_ptr<Material> m_material{};
        glm::vec3 m_position{};
        glm::vec3 m_velocity{};
        glm::vec3 m_rotation{};
        glm::vec3 m_oldPosition{};
        GLfloat m_angle{};
        GLfloat m_rotationSpeed{};
		float m_collisionDistance{};
		std::shared_ptr<PointLight> m_pLight{ nullptr };
};
