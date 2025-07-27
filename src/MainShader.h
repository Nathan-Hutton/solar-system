#pragma once

#include "Shader.h"

#include <string>
#include <array>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "SpotLight.h"

class MainShader : public Shader
{
    public:
        MainShader() {};

        void setLightsUniformVariables();
        void setPointLightsWithoutShadows(const std::array<std::shared_ptr<PointLight>, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount) const;
        void setPointLights(const std::array<std::shared_ptr<PointLight>, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;

        ~MainShader() {};

    private:
        int m_pointLightCount;

        struct {
            GLuint uniformFlashLightOn {};
        } m_uniformVariables {};

        struct {
            GLuint uniformColor {};
            GLuint uniformAmbientIntensity {};
            GLuint uniformDiffuseIntensity {};
            
            GLuint uniformPosition {};
            GLuint uniformExponential {};
            GLuint uniformLinear {};
            GLuint uniformConstant {};
        } m_uniformPointLights[scene::MAX_POINT_LIGHTS] {};

        struct {
            GLuint uniformColor {};
            GLuint uniformAmbientIntensity {};
            GLuint uniformDiffuseIntensity {};
            
            GLuint uniformPosition {};
            GLuint uniformExponential {};
            GLuint uniformLinear {};
            GLuint uniformConstant {};

            GLuint uniformDirection {};
            GLuint uniformEdge {};
        } m_uniformSpotLight {};

        struct {
            GLuint shadowMap {};
            GLuint farPlane {};
        } m_uniformOmniShadowMaps[1 + scene::MAX_POINT_LIGHTS] {};
};
