#pragma once

#include "Sphere.h"

class Sun: public Sphere
{
    public:
        Sun();
        Sun(float radius, GLfloat density, glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f), int stacks=20, int slices=20);
        ~Sun();
};