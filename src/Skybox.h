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

class Skybox
{
    public:
        Skybox();

        explicit Skybox(std::vector<std::string> faceLocations);

        void setProjectionMatrix(glm::mat4 projection);
        void drawSkybox(glm::mat4 viewMatrix);

        ~Skybox();

    private:
        Mesh* skyMesh {};
        Shader* skyShader {};

        GLuint textureID {};
        GLuint uniformProjection {}, uniformView {};
};
