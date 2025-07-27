#pragma once

#include <string>
#include <array>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "SpotLight.h"

class MainShader
{
    public:
        MainShader() {};

        void createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3 = "");
        void validate();

        void setLightsUniformVariables();
        void setPointLightsWithoutShadows(const std::array<std::shared_ptr<PointLight>, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount) const;
        void setPointLights(const std::array<std::shared_ptr<PointLight>, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;

        GLuint getShaderID() const { return m_shaderID; }

        // Set the active shader program for subsequent rendering operations
        void useShader() const { glUseProgram(m_shaderID); }

        void setTexture(GLuint textureUnit) const { glUniform1i(m_uniformTexture, textureUnit); }

        ~MainShader();

    private:
        void compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code = "");
        void compileProgram();

        int m_pointLightCount;

        GLuint m_shaderID {}; 
        GLuint m_uniformTexture {}; 

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
