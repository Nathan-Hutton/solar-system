#pragma once

#include "Sphere.h"
#include "PointLight.h"

class Sun final : public Sphere
{
    public:
        explicit Sun(GLfloat mass=2.0f, std::shared_ptr<Mesh> sphereMesh=nullptr, std::shared_ptr<Texture> texture=nullptr)
			: Sphere{ mass, sphereMesh, texture, nullptr } 
		{}

        void setPointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
            GLfloat ambientIntensity, GLfloat diffuseIntensity,
            GLfloat exponential, GLfloat linear, GLfloat constant)
		{
			m_light = new PointLight{shadowWidth, shadowHeight,
					near, far,
					red, green, blue, 
					ambientIntensity, diffuseIntensity,
					getPosition(),
					exponential, linear, constant};
		}

        PointLight* getPointLight() const { return m_light; }
        
        // TODO: Make the destructor decrease pointLightCount
        ~Sun() override { delete m_light; }

    private:
        PointLight *m_light {};
};
