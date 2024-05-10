#pragma once

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
#include "OrbitalPhysics.h"
#include "Skybox.h"
#include "Scene.h"

namespace SceneHandler {
    void setOldPositions();
    void setupSkybox(glm::mat4 projection);
    void createObjectsDefault();
    void createObjectsFigureEight();
    void createObjects1Sun1Planet();
    void createObjectsFancy();
}

#endif

