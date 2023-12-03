// This file will handle setting up the scene (creating the spheres) and calling their render methods
#ifndef SCENEFUNCTIONS_H
#define SCENEFUNCTIONS_H

#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sun.h"

namespace SceneFunctions {
    void CreateObjectsDefault(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
    void CreateObjectsFigureEight(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
    void Create1Sun1Planet(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
    void renderObjects(GLuint uniformModel, std::vector<Sun*>& stars, std::vector<Sphere*>& satellites);
}

#endif

