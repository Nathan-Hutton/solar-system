#pragma once

#include <vector>
#include "SpaceObject.h"
#include "PointLight.h"
#include "CommonValues.h"

namespace scene
{
    extern std::vector<SpaceObject*> satellites;
    extern std::vector<SpaceObject*> stars;
    extern PointLight* pointLights[MAX_POINT_LIGHTS];
    extern GLint pointLightCount;
}
