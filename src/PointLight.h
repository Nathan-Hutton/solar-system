#pragma once

#include <array>
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

#include "OmniShadowMap.h"

class PointLight 
{
    public:
        PointLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
					glm::vec3 color,
                    GLfloat ambientIntensity, GLfloat diffuseIntensity,
                    glm::vec3 position,
                    GLfloat exponential, GLfloat linear, GLfloat constant);

        void useLight(GLuint ambientIntensityLocation, GLuint diffuseIntensityLocation,
                    GLuint colorLocation, GLuint positionLocation,
                    GLuint exponentialLocation, GLuint linearLocation, GLuint constantLocation) const;

        std::array<glm::mat4, 6> calculateLightTransform() const;

        GLfloat getFarPlane() const { return m_farPlane; }

        const glm::vec3& getPosition() const { return m_position; }

        GLfloat getShadowMapWidth() const { return m_shadowMap->getShadowWidth(); }
        GLfloat getShadowMapHeight() const { return m_shadowMap->getShadowHeight(); }
        void shadowMapWrite() const { m_shadowMap->write(); }
        void shadowMapRead(GLenum textureUnit) const { m_shadowMap->read(textureUnit); }

    protected:
        std::unique_ptr<OmniShadowMap> m_shadowMap {};

        glm::vec3 m_color {};
        GLfloat m_ambientIntensity {};
        GLfloat m_diffuseIntensity {};

        glm::vec3 m_position {};
        GLfloat m_exponential {}, m_linear {}, m_constant {};
        
        // How far we want shadows to go. The farther they go, the lower resolution they are.
        GLfloat m_farPlane {};

        // How the light can see. This will be different for directional and point lights
        glm::mat4 m_lightProj {};

};
