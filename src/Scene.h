#pragma once

#include <vector>
#include "SpaceObject.h"
#include "PointLight.h"
#include "Skybox.h"
#include "CommonValues.h"

namespace scene
{
    extern std::vector<SpaceObject*> satellites;
    extern std::vector<SpaceObject*> stars;
    extern Skybox skybox;
    extern PointLight* pointLights[MAX_POINT_LIGHTS];
    extern GLint pointLightCount;
}
