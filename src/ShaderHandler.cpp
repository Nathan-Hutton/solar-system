#include "ShaderHandler.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <iostream>

namespace
{
    std::string readFile(const std::string& filePath)
    {
        std::ifstream file { filePath };
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLuint createShader(std::string shaderPath, GLenum shaderType)
    {
        const std::string shaderCode { readFile(shaderPath) };
        const GLchar* shaderAsGLchar { shaderCode.c_str() };

        GLuint shader { glCreateShader(shaderType) };
        glShaderSource(shader, 1, &shaderAsGLchar, NULL);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            throw std::runtime_error("ERROR: SHADER COMPILATION FAILED for " + shaderPath + '\n' + infoLog + '\n');
        }

        return shader;
    }

    void linkShaders(GLuint& shaderProgram, const std::vector<std::string>& shaderPaths, const std::vector<GLuint> shaders)
    {
        shaderProgram = glCreateProgram();
        for (GLuint shader : shaders)
            glAttachShader(shaderProgram, shader);
        glLinkProgram(shaderProgram);

        int success;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::string errorString{"ERROR: SHADER LINKING FAILED for "};
            for (std::string shaderPath : shaderPaths)
                errorString += shaderPath + ", ";
            errorString += '\n' + infoLog + '\n';
            throw std::runtime_error(errorString);
        }

        for (GLuint shader : shaders)
            glDeleteShader(shader);
    }
}

GLuint ShaderHandler::compileShader(const std::vector<std::string>& shaderPaths)
{
    GLuint shaderProgram;

    const GLuint vertexShader{ createShader(shaderPaths[0], GL_VERTEX_SHADER) };
    const GLuint fragmentShader{ createShader(shaderPaths[1], GL_FRAGMENT_SHADER) };

    // Just vert and frag shaders
    if (shaderPaths.size() == 2)
    {
        linkShaders(shaderProgram, shaderPaths, std::vector<GLuint>{vertexShader, fragmentShader});
        return shaderProgram;
    }

    // Vert, frag,and geometry shaders
    if (shaderPaths.size() == 3)
    {
        const GLuint geometryShader{ createShader(shaderPaths[2], GL_GEOMETRY_SHADER) };
        linkShaders(shaderProgram, shaderPaths, std::vector<GLuint>{vertexShader, fragmentShader, geometryShader});
        return shaderProgram;
    }

    // Since we'll have tessellation shaders if there's 4 or 5 shader paths, we'll just compile there here
    // Control shader
    const GLuint tessellationCtrlShader{ createShader(shaderPaths[2], GL_TESS_CONTROL_SHADER) };
    const GLuint tessellationEvalShader{ createShader(shaderPaths[3], GL_TESS_EVALUATION_SHADER) };

    // Vert, frag, and tessellation shaders
    if (shaderPaths.size() == 4)
    {
        linkShaders(shaderProgram, shaderPaths, std::vector<GLuint>{vertexShader, fragmentShader, tessellationCtrlShader, tessellationEvalShader});
        return shaderProgram;
    }

    const GLuint geometryShader{ createShader(shaderPaths[4], GL_GEOMETRY_SHADER) };
    linkShaders(shaderProgram, shaderPaths, std::vector<GLuint>{vertexShader, fragmentShader, tessellationCtrlShader, tessellationEvalShader, geometryShader});
    return shaderProgram;
}
