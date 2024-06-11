#include "Scene.h"

namespace scene
{
    extern std::vector<std::unique_ptr<SpaceObject>> satellites {};
    extern std::vector<std::unique_ptr<SpaceObject>> stars {};
    std::array<PointLight*, MAX_POINT_LIGHTS> pointLights;
    GLint pointLightCount {};
}
