#pragma once

#include <vector>
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include "Mesh.h"

class Sphere
{
    public:
        Sphere();
        Sphere(float radius, int stacks, int slices, glm::vec3 position, GLfloat density=1.0f);

        void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        Mesh* getMeshPointer();
        GLfloat getMass();
        GLfloat getRadius();
        glm::vec3 getPosition() const;
        void setPosition(glm::vec3 newPositionVector);

        ~Sphere();

    private:
        Mesh *sphereMesh;
        GLfloat radius;
        GLfloat density;
        GLfloat mass;
        glm::vec3 position;
};