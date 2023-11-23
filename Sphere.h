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
        Sphere(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices, GLfloat x=0, GLfloat y=0, GLfloat z=0, GLfloat density=1.0f);

        void generateSphereData(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, float radius, int stacks, int slices);
        Mesh* getMeshPointer();
        GLfloat getMass();
        GLfloat getRadius();
        GLfloat getX();
        GLfloat getY();
        GLfloat getZ();
        glm::vec3 getPositionVector() const;
        void setX(GLfloat x);
        void setY(GLfloat y);
        void setZ(GLfloat z);

        ~Sphere();

    private:
        Mesh *sphereMesh;
        GLfloat radius;
        GLfloat density;
        GLfloat mass;
        GLfloat x, y, z;
        glm::vec3 positionVector;
};