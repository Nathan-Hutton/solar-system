#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

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

        void useShader();
        void clearShader();

        ~Shader();

    private:
        GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformAmbientIntensity, uniformLightColor;

        void compileShader(const char* vertexCode, const char* fragmentCode);
        void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};
