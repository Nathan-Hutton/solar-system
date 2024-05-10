#define STB_IMAGE_IMPLEMENTATION

#include <cstdlib>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SolarSystemRenderer.h"
#include "CommonValues.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Sun.h"
#include "Planet.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneHandler.h"
#include "OrbitalPhysics.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"

void handleTimeChange(GLfloat yScrollOffset, GLfloat* timeChange)
{
    if (yScrollOffset == 0.0f) return;

    *timeChange += (yScrollOffset * 0.1f);
    if (*timeChange > 3.0f)
    {
        *timeChange = 3.0f;
        return;
    }
    if (*timeChange < 0.0f)
        *timeChange = 0.0f;
}

int main()
{
    SolarSystemRenderer renderer {};
    std::vector<SpaceObject*> stars {};
    std::vector<SpaceObject*> satellites {};
    GLuint pointLightCount {};
    Skybox skybox {};
    Camera camera {};

    // Print out the controls
    std::cout << "**********\n";
    std::cout << "\033[92m" << "Controls" << "\033[0m\n";
    std::cout << "**********\n";
    std::cout << "\nMouse: Look around\n\n";
    std::cout << "W: Move forward\n";
    std::cout << "S: Move backwards\n";
    std::cout << "A: Move left\n";
    std::cout << "D: Move right\n\n";
    std::cout << "Q: Roll left\n";
    std::cout << "E: Roll right\n\n";
    std::cout << "Space: Move up\n";
    std::cout << "Shift: Move down\n\n";
    std::cout << "F: Toggle flashlight\n";
    std::cout << "L: Toggle shadows\n\n\n";

    // Promp user to select a numerical integration scheme
    std::cout << "\033[92m" << "Choose a numerical integration method" << "\033[0m\n";
    std::cout << "0: Euler method\n1: Verlet method \n>";
    std::cin >> OrbitalPhysics::verlet;

    // Prompt user to select scene
    int selectedScene {};
    std::cout << "\033[92m" << "\nChoose a scene" << "\033[0m\n";
    std::cout << "1: 1 planet 1 sun\n2: Lots of objects\n3: Figure eight\n4: Final release scene\n> ";
    std::cin >> selectedScene;

    Window mainWindow {1920, 1200};
    mainWindow.initialize();

    PointLight* pointLights[MAX_POINT_LIGHTS] {};

    // Build scene based on user input
    switch (selectedScene)
    {
        case (1):
            SceneFunctions::createObjects1Sun1Planet(stars, satellites, pointLights, &pointLightCount, &camera);
            break;
        case (2):
            SceneFunctions::createObjectsDefault(stars, satellites, pointLights, &pointLightCount, &camera);
            break;
        case (3):
            SceneFunctions::createObjectsFigureEight(stars, satellites, pointLights, &pointLightCount, &camera);
            break;
        case (4):
            SceneFunctions::createObjectsFancy(stars, satellites, pointLights, &pointLightCount, &camera);
            break;
        default:
            break;
    }

    //// Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix
    glm::mat4 projection {glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 1.0f, 400.0f)};
    SceneFunctions::setupSkybox(&skybox, projection);
    renderer.createShaders(pointLights, pointLightCount, projection, camera.getSpotLight());
    renderer.setupPostProcessingObjects();
    renderer.setLightUniformVariables(satellites);

    // Loop until window is closed
    GLfloat now {};
    unsigned int counter {0};
    double lastFPSUpdateTime {glfwGetTime()};
    GLfloat lastFrame {0.0f};
    GLfloat deltaTime {0.0f};
    GLfloat timeChange {1.0f};
    GLfloat timeStep {0.0f};
    GLfloat timeSinceLastVerlet {0.0f};
    while(!mainWindow.getShouldClose())
    {
        now         = glfwGetTime();
        deltaTime   = now - lastFrame;
        lastFrame   = now;
        timeStep    = deltaTime * timeChange;
        counter++;

        // Update FPS counter
        if (counter == 30)
        {
            double timeElapsed {now - lastFPSUpdateTime};
            std::string FPS {std::to_string(30.0 / timeElapsed)};
            std::string newTitle {"Solar System - " + FPS + " FPS"};
            mainWindow.setWindowTitle(newTitle);
            lastFPSUpdateTime = now;
            counter = 0;
        }

        // Update our object's positions based on our chosen numerical scheme
        if (OrbitalPhysics::verlet)
        {
            OrbitalPhysics::updateCelestialBodyAngles(stars, satellites, timeStep);
            OrbitalPhysics::updatePositionsVerlet(stars, satellites, &timeSinceLastVerlet);
        }
        else
        {
            OrbitalPhysics::updateCelestialBodyAngles(stars, satellites, timeStep);
            OrbitalPhysics::updatePositionsEuler(stars, satellites, timeStep);
        }

        // Get + handle user input
        glfwPollEvents();
        bool* keys {mainWindow.getKeys()};
        camera.keyControl(keys, deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset(), &timeChange);

        // Check for flashlight toggle
        if (keys[GLFW_KEY_L])
        {
            renderer.toggleShadows(satellites, stars);

            // Setting this to false means it won't trigger multiple times when we press it once
            keys[GLFW_KEY_L] = false;
        }

        if (renderer.getShadowsEnabled())
        {
            // These needs to be index based loops so that we don't make a copy of the lights each time
            for (size_t i {0}; i < pointLightCount; i++)
                renderer.omniShadowMapPass(pointLights[i], satellites);
            renderer.omniShadowMapPass(camera.getSpotLight(), satellites);
        }

        renderer.renderPass(camera.calculateViewMatrix(), camera.getSpotLight(), pointLightCount, camera.getPosition(), satellites, stars, &skybox);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
