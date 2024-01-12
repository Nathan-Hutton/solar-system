#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader
{
    public:
        Shader();

        void createFromString(const char* vertexCode, const char* fragmentCode);
        void createFromFiles(const char* vertexLocation, const char* fragmentLocation);
        void createFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

        void validate();

        std::string readFile(const char* fileLocation);

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

        void setDirectionalLight(DirectionalLight *dLight);
        void setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount);
        void setSpotLightsWithoutShadows(SpotLight* sLights[], unsigned int lightCount);
        void setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
        void setSpotLights(SpotLight* sLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset);

        void setTexture(GLuint textureUnit);
        void setDirectionalShadowMap(GLuint textureUnit);
        void setDirectionalLightTransform(glm::mat4* lTransform);
        void setLightMatrices(std::vector<glm::mat4> lightMatrices);

        void useShader();
        void clearShader();

        ~Shader();

    private:
        int pointLightCount;
        int spotLightCount;

        GLuint shaderID, uniformProjection, uniformModel, uniformView, 
        uniformEyePosition, uniformSpecularIntensity, uniformShininess,
        uniformTexture, uniformPointLightCount, uniformSpotLightCount,
        uniformDirectionalLightTransform, uniformDirectionalShadowMap,
        uniformOmniLightPos, uniformFarPlane;

        GLuint uniformLightMatrices[6];

        struct {
            GLuint uniformColor;
            GLuint uniformAmbientIntensity;
            GLuint uniformDiffuseIntensity;

            GLuint uniformDirection;
        } uniformDirectionalLight;

        struct {
            GLuint uniformColor;
            GLuint uniformAmbientIntensity;
            GLuint uniformDiffuseIntensity;
            
            GLuint uniformPosition;
            GLuint uniformExponential;
            GLuint uniformLinear;
            GLuint uniformConstant;
        } uniformPointLights[MAX_POINT_LIGHTS];

        struct {
            GLuint uniformColor;
            GLuint uniformAmbientIntensity;
            GLuint uniformDiffuseIntensity;
            
            GLuint uniformPosition;
            GLuint uniformExponential;
            GLuint uniformLinear;
            GLuint uniformConstant;

            GLuint uniformDirection;
            GLuint uniformEdge;
        } uniformSpotLights[MAX_SPOT_LIGHTS];

        struct {
            GLuint shadowMap;
            GLuint farPlane;
        } uniformOmniShadowMaps[MAX_SPOT_LIGHTS + MAX_POINT_LIGHTS];

        void compileShader(const char* vertexCode, const char* fragmentCode);
        void compileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
        void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
        void compileProgram();
};
