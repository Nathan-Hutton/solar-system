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
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"

namespace SceneFunctions {
    void createObjectsDefault(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
    void createObjectsFigureEight(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
    void create1Sun1Planet(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
}

#endif

