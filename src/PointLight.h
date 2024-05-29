#pragma once

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "OmniShadowMap.h"

class PointLight 
{
    public:

        PointLight();
        PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat exponential, GLfloat linear, GLfloat constant);

        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation) const;

        std::vector<glm::mat4> calculateLightTransform() const;

        GLfloat getFarPlane() const;

        const glm::vec3& getPosition() const;

        GLfloat getShadowMapWidth() const { return m_shadowMap->getShadowWidth(); }
        GLfloat getShadowMapHeight() const { return m_shadowMap->getShadowHeight(); }
        void shadowMapWrite() const { m_shadowMap->write(); }
        void shadowMapRead(GLenum textureUnit) const { m_shadowMap->read(textureUnit); }

        ~PointLight();

    protected:
        glm::vec3 m_position {};
        GLfloat m_exponential {}, m_linear {}, m_constant {};
        
        // How far we want shadows to go. The farther they go, the lower resolution they are.
        GLfloat m_farPlane {};

        glm::vec3 m_color {};
        GLfloat m_ambientIntensity {};
        GLfloat m_diffuseIntensity {};

        // How the light can see. This will be different for directional and point lights
        glm::mat4 m_lightProj {};

        OmniShadowMap* m_shadowMap {};
};
