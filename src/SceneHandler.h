#pragma once

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
#include "Material.h"
#include "OrbitalPhysics.h"
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"

namespace SceneHandler {
    void setOldPositions();
    void setupSkybox(const glm::mat4 projection);
    void createObjectsDefault();
    void createObjectsFigureEight();
    void createObjects1Sun1Planet();
    void createObjectsFancy();
}

