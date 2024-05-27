#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Scene.h"
#include "SpotLight.h"

class Shader
{
    public:
        Shader();

        void createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3 = "");

        void validate();

        GLuint getShaderID() const;

        GLuint getProjectionLocation() const;
        GLuint getModelLocation() const;
        GLuint getViewLocation() const;

        GLuint getEyePositionLocation() const;
        GLuint getSpecularIntensityLocation() const;
        GLuint getShininessLocation() const;

        GLuint getOmniLightPosLocation() const;
        GLuint getFarPlaneLocation() const;

        void setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount) const;
        void setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const;

        void setTexture(GLuint textureUnit) const;
        void setLightMatrices(const std::vector<glm::mat4>& lightMatrices) const;

        void useShader() const;

        ~Shader();

    private:
        int pointLightCount;
        GLuint uniformLightMatrices[6] {};
        GLuint shaderID {}; 

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
        } uniformVariables {};

        struct {
            GLuint uniformColor {};
            GLuint uniformAmbientIntensity {};
            GLuint uniformDiffuseIntensity {};
            
            GLuint uniformPosition {};
            GLuint uniformExponential {};
            GLuint uniformLinear {};
            GLuint uniformConstant {};
        } uniformPointLights[scene::MAX_POINT_LIGHTS] {};

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
        } uniformSpotLight {};

        struct {
            GLuint shadowMap {};
            GLuint farPlane {};
        } uniformOmniShadowMaps[1 + scene::MAX_POINT_LIGHTS] {};

        std::string readFile(std::string_view fileLocation) const;
        void compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code = "");
        void addShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType) const;
        void compileProgram();
        std::string getShaderTypeString(GLenum shaderType) const;
        void clearShader();
};
