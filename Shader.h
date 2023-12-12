#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"

class Shader
{
    public:
        Shader();

        void createFromString(const char* vertexCode, const char* fragmentCode);
        void createFromFiles(const char* vertexLocation, const char* fragmentLocation);

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

        void setDirectionalLight(DirectionalLight *dLight);
        void setPointLights(PointLight *pLights, unsigned int lightCount);

        void useShader();
        void clearShader();

        ~Shader();

    private:
        int pointLightCount;

        GLuint shaderID, uniformProjection, uniformModel, uniformView, 
        uniformEyePosition, uniformSpecularIntensity, uniformShininess,
        uniformPointLightCount;

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

        void compileShader(const char* vertexCode, const char* fragmentCode);
        void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};
