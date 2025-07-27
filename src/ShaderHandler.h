#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

namespace  ShaderHandler
{
    GLuint compileShader(const std::vector<std::string>& shaderPaths);
}

