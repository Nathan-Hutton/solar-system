#include "Scene.h"

namespace scene
{
    std::vector<SpaceObject*> satellites {};
    std::vector<SpaceObject*> stars {};
    std::array<PointLight*, MAX_POINT_LIGHTS> pointLights;
    GLint pointLightCount {};
}
