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
        Sphere(float radius, int stacks, int slices, GLfloat x=0, GLfloat y=0, GLfloat z=0, GLfloat density=1.0f);

        void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        Mesh* getMeshPointer();
        GLfloat getMass();
        GLfloat getRadius();
        glm::vec3 getPositionVector() const;
        void setPositionVector(glm::vec3 newPositionVector);

        ~Sphere();

    private:
        Mesh *sphereMesh;
        GLfloat radius;
        GLfloat density;
        GLfloat mass;
        glm::vec3 positionVector;
};