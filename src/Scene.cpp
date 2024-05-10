#include "Scene.h"

namespace scene
{
    std::vector<SpaceObject*> satellites {};
    std::vector<SpaceObject*> stars {};
    Skybox skybox {};
    Camera camera {};
    PointLight* pointLights[commonVals::MAX_POINT_LIGHTS] {};
    GLuint pointLightCount {};
}
