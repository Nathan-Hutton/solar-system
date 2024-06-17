#include "Shader.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

namespace
{
    std::string getShaderTypeString(GLenum shaderType)
    {
        switch (shaderType)
        {
            case (GL_VERTEX_SHADER):   return "GL_VERTEX_SHADER";
            case (GL_FRAGMENT_SHADER): return "GL_FRAGMENT_SHADER";
            case (GL_GEOMETRY_SHADER): return "GL_GEOMETRY_SHADER";
        }
        return "";
    }

    std::string readFile(std::string_view fileLocation)
    {
        std::ifstream fileStream{std::filesystem::path{fileLocation}};

        if (!fileStream.is_open())
            throw std::runtime_error("Failed to read " + std::string{fileLocation} + " File doesn't exist");

        std::string line {};
        std::string fileContents {};
        while (getline(fileStream, line))
            fileContents += line + '\n';

        return fileContents;
    }

    void addShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType)
    {
        // Makes a shader object
        GLuint theShader {glCreateShader(shaderType)};

        // Sort the code in a way OpenGL can understand
        const GLchar* theCode[1];
        theCode[0] = shaderCode.c_str();
        GLint codeLength[1];
        codeLength[0] = std::ssize(shaderCode);

        // Give the source code to the shader 
        glShaderSource(theShader, 1, theCode, codeLength);
        // Compile the shader source code
        glCompileShader(theShader);

        GLint result {0};
        GLchar eLog[1024] { 0 };

        // Check for compilation errors
        glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
        if (!result) {
            glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
            throw std::runtime_error("Error compiling the " + getShaderTypeString(shaderType) + " shader: " + std::string{eLog});
        }

        // Attach the now compiled shader to the OpenGL program
        glAttachShader(theProgram, theShader);
    }
}

void Shader::createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3)
{
    compileShader(readFile(file1), readFile(file2), (file3 == "") ? "" : readFile(file3));
}

void Shader::compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code)
{
    // Create a new OpenGL program object. Is the final linked version of multiple
    // shaders combined. Shaders are written in GLSL (OpenGL shading language) and run on the GPU
    // For rendering graphics. Creates container which we attach shaders to.
    m_shaderID = glCreateProgram();

    if (!m_shaderID)
        throw std::runtime_error("Error creating shader program");

    // We'll always have a vertex shader
    addShader(m_shaderID, shader1Code, GL_VERTEX_SHADER);

    // If we included 3 files, that means we're using a geometry shader
    if (shader3Code != "")
    {
        addShader(m_shaderID, shader2Code, GL_GEOMETRY_SHADER);
        addShader(m_shaderID, shader3Code, GL_FRAGMENT_SHADER);
    }
    else
    {
        addShader(m_shaderID, shader2Code, GL_FRAGMENT_SHADER);
    }

    compileProgram();
}

void Shader::validate()
{
    GLint result {0};
    GLchar eLog[1024] {0};

    // Check if the shader program can execute on the current OpenGL state
    glValidateProgram(m_shaderID);
    glGetProgramiv(m_shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(m_shaderID, sizeof(eLog), NULL, eLog);
        throw std::runtime_error("Error validating program: " + std::string{eLog});
    }
}

void Shader::compileProgram()
{
    // Linking all the shaders together into a single program on the GPU.
    // Checks to make sure that the inputs and outputs of the shaders match up.
    // Creates an executable that runs on the programmable parts of the GPU
    glLinkProgram(m_shaderID);

    GLint result {0};
    GLchar eLog[1024] {0};

    // Check for errors
    glGetProgramiv(m_shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(m_shaderID, sizeof(eLog), NULL, eLog);
        throw std::runtime_error("Error linking program: " + std::string{eLog});
    }
}

Shader::~Shader()
{
    if (m_shaderID != 0)
    {
        glDeleteProgram(m_shaderID);
        m_shaderID = 0;
    }
}
