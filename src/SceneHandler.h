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
#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "SpotLight.h"
#include "Material.h"
#include "Camera.h"
#include "OrbitalPhysics.h"
#include "Skybox.h"

namespace SceneFunctions {
    void setOldPositions(std::vector<SpaceObject*>& satellites, std::vector<SpaceObject*>& stars);
    void setupSkybox(Skybox* skybox, glm::mat4 projection);
    void createObjectsDefault(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet);
    void createObjectsFigureEight(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet);
    void createObjects1Sun1Planet(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet);
    void createObjectsFancy(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet);
}

#endif

