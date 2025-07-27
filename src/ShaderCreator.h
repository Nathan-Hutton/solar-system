#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

namespace  shaderCreator
{
    GLuint compileShader(const std::vector<std::string>& shaderPaths);
}

