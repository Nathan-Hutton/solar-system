#pragma once

#include <vector>
#include <array>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "SpaceObject.h"
#include "PointLight.h"

namespace scene
{
    inline constexpr int MAX_POINT_LIGHTS {3};
    extern std::vector<std::shared_ptr<SpaceObject>> movables;
    extern std::vector<std::shared_ptr<SpaceObject>> glowingObjects;
    extern std::vector<std::shared_ptr<SpaceObject>> shadedObjects;
    extern std::array<std::shared_ptr<PointLight>, MAX_POINT_LIGHTS> pointLights;
    extern GLint pointLightCount;

	void readSceneJson(std::string filePath);
    void setupSkybox();
}
