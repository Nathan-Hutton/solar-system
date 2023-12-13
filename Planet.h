#pragma once

#include "Sphere.h"
#include "Material.h"

class Planet: public Sphere
{
    public:
        Planet();
        Planet(float radius, GLfloat mass, glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f), int stacks=20, int slices=20);
        void setMaterialPointer(Material* materialPointer);
        Material* getMaterialPointer();
        
        ~Planet();

    private:
        Material *material;
};
