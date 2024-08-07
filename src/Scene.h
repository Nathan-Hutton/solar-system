#pragma once

#include <vector>
#include <array>
#include <memory>

#include <glm/glm.hpp>

#include "SpaceObject.h"
#include "PointLight.h"

namespace scene
{
    inline constexpr int MAX_POINT_LIGHTS {3};
    extern std::vector<std::unique_ptr<SpaceObject>> satellites;
    extern std::vector<std::unique_ptr<SpaceObject>> stars;
    extern std::array<PointLight*, MAX_POINT_LIGHTS> pointLights;
    extern GLint pointLightCount;

    void setupSkybox();
    void createObjectsDefault();
    void createObjectsFigureEight();
    void createObjects1Sun1Planet();
    void createObjectsFancy();
}
