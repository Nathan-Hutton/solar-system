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

        // Set the active shader program for subsequent rendering operations
        void useShader() const { glUseProgram(m_shaderID); }

        ~Shader();

    private:
        GLuint m_shaderID {}; 

        void compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code = "");
        void compileProgram();
};
