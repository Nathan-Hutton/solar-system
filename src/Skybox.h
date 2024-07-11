#pragma once

#include <array>
#include <string>
#include <glm/glm.hpp>

namespace skybox
{
    void setup(const std::array<std::string, 6>& faceLocations);
    void drawSkybox(glm::mat4& view, const glm::mat4& projection);
};
