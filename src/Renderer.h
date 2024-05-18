#pragma once

#include <glm/glm.hpp>

namespace renderer
{
    void toggleShadows();
    void setup(const glm::mat4 projection);
    void omniShadowMapPasses();
    void renderPass();
};
