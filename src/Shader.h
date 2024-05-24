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

        GLuint getShaderID();

        GLuint getProjectionLocation();
        GLuint getModelLocation();
        GLuint getViewLocation();

        GLuint getEyePositionLocation();
        GLuint getSpecularIntensityLocation();
        GLuint getShininessLocation();

        GLuint getOmniLightPosLocation();
        GLuint getFarPlaneLocation();

        void setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount);
        void setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset);
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset);

        void setTexture(GLuint textureUnit);
        void setLightMatrices(const std::vector<glm::mat4>& lightMatrices);

        void useShader();

        ~Shader();

    private:
        int pointLightCount;

        GLuint shaderID {}, uniformProjection {}, uniformModel {}, uniformView {}, 
        uniformEyePosition {}, uniformSpecularIntensity {}, uniformShininess {},
        uniformTexture {}, uniformPointLightCount {}, uniformFlashLightOn {},
        uniformOmniLightPos {}, uniformFarPlane {};

        GLuint uniformLightMatrices[6] {};

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

        std::string readFile(std::string_view fileLocation);
        void compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code = "");
        void addShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType);
        void compileProgram();
        std::string getShaderTypeString(GLenum shaderType);
        void clearShader();
};
