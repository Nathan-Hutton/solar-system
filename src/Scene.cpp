#include "Scene.h"

namespace scene
{
    std::vector<std::unique_ptr<SpaceObject>> satellites {};
    std::vector<std::unique_ptr<SpaceObject>> stars {};
    std::array<PointLight*, MAX_POINT_LIGHTS> pointLights;
    GLint pointLightCount {};
}
