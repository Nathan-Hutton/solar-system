#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun: public Sphere
{
    public:
        Sun();
        Sun(float radius, GLfloat mass, glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f), int stacks=20, int slices=20);
        void setPointLight(GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity,
            GLfloat exponential, GLfloat linear, GLfloat constant);
        PointLight* getPointLight();
        
        ~Sun();

    private:
        PointLight *light;
};
