#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"

namespace skybox
{
    extern Mesh* skyMesh;
    extern Shader* skyShader;

    extern GLuint textureID;
    extern GLuint uniformView;
    extern GLuint uniformProjection;

    void setup(std::vector<std::string> faceLocations);
    void setProjectionMatrix(glm::mat4 projection);
    void drawSkybox(glm::mat4 viewMatrix);
};
