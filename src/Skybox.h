#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace skybox
{
    void setup(std::vector<std::string> faceLocations);
    void setProjectionMatrix(glm::mat4 projection);
    void drawSkybox(glm::mat4 viewMatrix);
};
