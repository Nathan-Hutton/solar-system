#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun final : public Sphere
{
    public:
        explicit Sun(GLfloat mass=2.0f, GLfloat radius=1.5f, std::shared_ptr<Mesh> sphereMesh=std::shared_ptr<Mesh>());

        void setPointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity,
            GLfloat exponential, GLfloat linear, GLfloat constant);
        PointLight* getPointLight() const { return m_light; }
        void render() const override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        
        // TODO: Make the destructor decrease pointLightCount
        ~Sun() override { delete m_light; }

    private:
        PointLight *m_light {};
};
