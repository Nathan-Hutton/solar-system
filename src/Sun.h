#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun: public Sphere
{
    public:
        Sun();
        Sun(GLfloat radius, GLfloat mass, int stacks=20, int slices=20);
        void render() const override;
        void setPointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity,
            GLfloat exponential, GLfloat linear, GLfloat constant);
        PointLight* getPointLight() const;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        
        ~Sun();

    private:
        PointLight *light {};
};
