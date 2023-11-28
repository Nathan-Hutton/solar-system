#pragma once

#include "Planet.h"

class Moon: public Sphere
{
    public:
        Moon();
        Moon(float radius, GLfloat density, Planet *parentPlanet, glm::vec3 position, int stacks=20, int slices=20);
        ~Moon();

    private:
        Planet *parentPlanet;
};