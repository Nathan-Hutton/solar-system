#pragma once

#include <glm/glm.hpp>

namespace sceneHandler {
    void setupSkybox(const glm::mat4& projection);
    void createObjectsDefault();
    void createObjectsFigureEight();
    void createObjects1Sun1Planet();
    void createObjectsFancy();
}

