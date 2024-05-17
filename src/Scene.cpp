#include "Scene.h"

namespace scene
{
    std::vector<SpaceObject*> satellites {};
    std::vector<SpaceObject*> stars {};
    PointLight* pointLights[MAX_POINT_LIGHTS] {};
    GLint pointLightCount {};
}
