#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun: public Sphere
{
    public:
        explicit Sun(GLfloat mass=2.0f, GLfloat radius=1.5f, int stacks=15, int slices=15);
        void render() const override;
        void setPointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity,
            GLfloat exponential, GLfloat linear, GLfloat constant);
        PointLight* getPointLight() const { return m_light; }
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        
        // TODO: Make the destructor decrease pointLightCount
        ~Sun() { delete m_light; }

    private:
        PointLight *m_light {};
};
