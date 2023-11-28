#pragma once

#include "Sun.h"

class Planet: public Sphere
{
    public:
        Planet();
        Planet(float radius, GLfloat density, Sun *parentSun, glm::vec3 position, int stacks=20, int slices=20);
        ~Planet();

    private:
        Sun *parentSun;
};