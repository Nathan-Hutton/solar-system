#pragma once

#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"
#include "SpotLight.h"

class Shader
{
    public:
        Shader();

        void createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3 = "");

        void validate();

        std::string readFile(std::string_view fileLocation);

        GLuint getShaderID();

        GLuint getProjectionLocation();
        GLuint getModelLocation();
        GLuint getViewLocation();

        GLuint getAmbientIntensityLocation();
        GLuint getLightColorLocation();
        GLuint getDiffuseIntensityLocation();
        GLuint getDirectionLocation();

        GLuint getEyePositionLocation();
        GLuint getSpecularIntensityLocation();
        GLuint getShininessLocation();

        GLuint getExponentialLocation();
        GLuint getLinearLocation();
        GLuint getConstantLocation();

        GLuint getOmniLightPosLocation();
        GLuint getFarPlaneLocation();

        void setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount);
        void setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
        void setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset);
        void setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset);

        void setTexture(GLuint textureUnit);
        void setLightMatrices(std::vector<glm::mat4> lightMatrices);

        void useShader();
        void clearShader();

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
        } uniformPointLights[MAX_POINT_LIGHTS] {};

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
        } uniformOmniShadowMaps[1 + MAX_POINT_LIGHTS] {};

        void compileShader(const std::string shader1Code, const std::string shader2Code, const std::string shader3Code = "");
        void addShader(GLuint theProgram, const std::string shaderCode, GLenum shaderType);
        void compileProgram();
};
