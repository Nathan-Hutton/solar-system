// This file will handle setting up the scene (creating the spheres) and calling their render methods
#ifndef SCENEFUNCTIONS_H
#define SCENEFUNCTIONS_H

#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sun.h"
#include "Planet.h"
#include "Shader.h"
#include "Camera.h"
#include "SpotLight.h"
#include "Material.h"
#include "Camera.h"

namespace SceneFunctions {
    void createObjectsDefault(std::vector<Sun*>& stars, std::vector<Planet*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            SpotLight* sLights[], unsigned int *sLightCount,
            Camera *camera);
    void createObjectsFigureEight(std::vector<Sun*>& stars, std::vector<Planet*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            SpotLight* sLights[], unsigned int *sLightCount,
            Camera *camera);
    void createObjects1Sun1Planet(std::vector<Sun*>& stars, std::vector<Planet*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            SpotLight* sLights[], unsigned int *sLightCount,
            Camera *camera);
}

#endif

