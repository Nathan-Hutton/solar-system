#pragma once

#include <vector>
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

class Sphere
{
    public:
        Sphere();
        Sphere(float radius, GLfloat density, glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f), int stacks=20, int slices=20);

        void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        void renderMesh(GLuint uniformSpecularIntensity, GLuint uniformShininiess);
        Mesh* getMeshPointer();
        GLfloat getMass();
        GLfloat getRadius();
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
        Texture* getTexturePointer();
        void setTexturePointer(Texture* texturePointer);
        void setMaterialPointer(Material* materialPointer);
        Material* getMaterialPointer();

        ~Sphere();

    protected:
        Mesh *sphereMesh;
        Texture *texture;
        Material *material;
        GLfloat radius;
        GLfloat density;
        GLfloat mass;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotation;
        GLfloat angle;
        GLfloat rotationSpeed;
};
