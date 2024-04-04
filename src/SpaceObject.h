#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class SpaceObject
{
    public:
        SpaceObject();
        SpaceObject(GLfloat mass);

        GLfloat getMass();
        glm::vec3 getOldPosition() const;
        void setOldPosition(glm::vec3 oldPos);
        glm::vec3 getPosition() const;
        void setPosition(glm::vec3 position);
        glm::vec3 getVelocity() const;
        void setVelocity(glm::vec3 velocity);
        glm::vec3 getRotation() const;
        void setRotation(glm::vec3 rotation);
        GLfloat getAngle();
        void setAngle(GLfloat angle);
        GLfloat getRotationSpeed();
        void setRotationSpeed(GLfloat speed);
        void setMaterialPointer(Material* material);
        Material* getMaterialPointer();
        virtual void render() = 0;
        virtual void setWorldProperties(glm::mat4* model) = 0;

        // This is a shitty solution for not letting the objects' force get too strong
        // since we have no collisions yet. Look at the getForce method for more info
        GLfloat getGreatestDistanceBetweenVertices();

        ~SpaceObject();

    protected:
        GLfloat mass;
        glm::vec3 oldPosition;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotation;
        GLfloat angle;
        GLfloat rotationSpeed;
        Material *material;

        GLfloat greatestDistanceBetweenVertices;
};
