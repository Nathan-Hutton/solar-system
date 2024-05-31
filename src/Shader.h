#pragma once

#include <string>
#include <array>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "SpotLight.h"

class Shader
{
    public:
        Shader() {};

        void createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3 = "");

        void validate();

        GLuint getShaderID() const { return m_shaderID; }

        GLuint getProjectionLocation() const { return m_uniformVariables.uniformProjection; }
        GLuint getModelLocation() const { return m_uniformVariables.uniformModel; }
        GLuint getViewLocation() const { return m_uniformVariables.uniformView; }

        GLuint getEyePositionLocation() const { return m_uniformVariables.uniformEyePosition; }
        GLuint getSpecularIntensityLocation() const { return m_uniformVariables.uniformSpecularIntensity; }
        GLuint getShininessLocation() const { return m_uniformVariables.uniformShininess; }

        GLuint getOmniLightPosLocation() const { return m_uniformVariables.uniformOmniLightPos; }
        GLuint getFarPlaneLocation() const { return m_uniformVariables.uniformFarPlane; }

        void setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount) const;
        void setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;

        void setTexture(GLuint textureUnit) const { glUniform1i(m_uniformVariables.uniformTexture, textureUnit); }
        void setLightMatrices(const std::array<glm::mat4, 6>& lightMatrices) const;


        // Set the active shader program for subsequent rendering operations
        void useShader() const { glUseProgram(m_shaderID); }

        ~Shader();

    private:
        int m_pointLightCount;
        GLuint m_uniformLightMatrices[6] {};
        GLuint m_shaderID {}; 

        struct {
            GLuint uniformProjection {}; 
            GLuint uniformModel {};
            GLuint uniformView {}; 
            GLuint uniformEyePosition {};
            GLuint uniformSpecularIntensity {};
            GLuint uniformShininess {};
            GLuint uniformTexture {}; 
            GLuint uniformPointLightCount {};
            GLuint uniformFlashLightOn {};
            GLuint uniformOmniLightPos {};
            GLuint uniformFarPlane {};
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

        void compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code = "");
        void compileProgram();
};
