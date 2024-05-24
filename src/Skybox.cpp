#include "Skybox.h"

#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"


namespace
{
    Mesh* skyMesh {};
    GLuint textureID {};
    Shader* skyShader {};
    GLuint uniformView {};
    GLuint uniformProjection {};
}

namespace skybox
{
    void setup(std::vector<std::string> faceLocations)
    {
        // Shader setup
        skyShader = new Shader{};
        skyShader->createFromFiles("../assets/shaders/skybox.vert", "../assets/shaders/skybox.frag");

        uniformProjection   = skyShader->getProjectionLocation();
        uniformView         = skyShader->getViewLocation();

        // Texture setup
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width {}, height {}, bitDepth {};

        for (size_t i {0}; i < 6; ++i)
        {
            unsigned char *textData {stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, STBI_rgb)};
            if (!textData)
                throw std::runtime_error("Failed to find " + faceLocations[i]);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textData);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Mesh setup

        unsigned int skyboxIndices[] {
            // front
            2, 1, 0,
            3, 1, 2,
            // right
            5, 3, 2,
            7, 3, 5,
            // back
            4, 7, 5,
            6, 7, 4,
            // left
            0, 6, 4,
            1, 6, 0,
            // top
            5, 0, 4,
            2, 0, 5,
            // bottom
            3, 6, 1,
            7, 6, 3
        };

        float skyboxVertices[] {
            -1.0f, 1.0f, -1.0f,		0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,	0.0f, 0.0f,
            1.0f, 1.0f, -1.0f,		0.0f, 0.0f,
            1.0f, -1.0f, -1.0f,		0.0f, 0.0f,

            -1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,		0.0f, 0.0f,
            1.0f, -1.0f, 1.0f,		0.0f, 0.0f
        };

        skyMesh = new Mesh{};
        skyMesh->createMesh(skyboxVertices, skyboxIndices, 64, 36, false);
    }

    void setProjectionMatrix(const glm::mat4& projection)
    {
        skyShader->useShader();
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    }

    void drawSkybox(glm::mat4& viewMatrix)
    {
        // We only want to use the rotation values. All of the transformation values
        // are stored in the 4th column of the viewMatrix, so we'll set that column to all zeros
        viewMatrix[3][0] = 0.0f;
        viewMatrix[3][1] = 0.0f;
        viewMatrix[3][2] = 0.0f;
        skyShader->useShader();

        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // We can set this to GL_TEXTURE0 since it's independent of the other shaders
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        skyShader->validate();

        skyMesh->render();
    }
}
