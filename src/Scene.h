#pragma once

#include <vector>

#include "SpaceObject.h"
#include "PointLight.h"

namespace scene
{
    inline constexpr int MAX_POINT_LIGHTS {3};
    extern std::vector<SpaceObject*> satellites;
    extern std::vector<SpaceObject*> stars;
    extern PointLight* pointLights[MAX_POINT_LIGHTS];
    extern GLint pointLightCount;
}
