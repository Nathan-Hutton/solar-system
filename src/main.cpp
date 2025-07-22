#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "OrbitalPhysics.h"

namespace
{
    void handleTimeChange(GLfloat yScrollOffset, GLfloat& timeChange)
    {
		if (orbitalPhysics::verlet || yScrollOffset == 0.0f) return;

        const GLfloat amountChange = yScrollOffset * 0.1f;
        timeChange += (timeChange + amountChange > 3.0f || timeChange + amountChange < -0.1f) ? 0.0f : amountChange;
		timeChange = std::max(0.0f, timeChange);
    }

	// TODO: Just have a "controls" button in ImGui that shows all of these instead
    void printControls()
    {
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
    }

    bool clearFailedExtraction()
    {
        // User probably hit Ctrl D
        if (std::cin.eof())
            exit(0);

        if (!std::cin || std::cin.peek() != '\n')
        {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            return true;
        }

        return false;
    }

    void setNumericalScheme()
    {
        // Promp user to select a numerical integration scheme
        while (true)
        {
            std::cout << "\033[92m" << "Choose a numerical integration method" << "\033[0m\n";
            std::cout << "0: Euler method\n1: Verlet method \n>";
            int verletInput {};
            std::cin >> verletInput;

            if (clearFailedExtraction() || (verletInput != 0 && verletInput != 1))
            {
                std::cout << "\033[91m" << "Invalid input" << "\033[0m\n\n";
                continue;
            }

            orbitalPhysics::verlet = verletInput;
            return;
        }
    }

    void setupScene(const std::string& jsonFilePath)
    {
        printControls();
        setNumericalScheme();
		window::initialize();

		scene::readSceneJson(jsonFilePath);

        const glm::mat4 projection {glm::perspective(glm::radians(60.0f), static_cast<GLfloat>(window::windowWidth) / static_cast<GLfloat>(window::windowHeight), 1.0f, 400.0f)};
        scene::setupSkybox();
        renderer::setup(projection);
    }
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Must give 1 command line argument: json file name for setting up the scene\n";
		std::exit(EXIT_FAILURE);
	}
    setupScene(argv[1]);

    // Loop until window is closed
    int counter {0};
    double lastFPSUpdateTime {glfwGetTime()};
    GLfloat lastFrame {0.0f};
    GLfloat timeChange {1.0f};
    GLfloat timeSinceLastVerlet {0.0f}; // Not relevant if we're using Euler for physics
    while(!glfwWindowShouldClose(window::mainWindow))
    {
        GLfloat now         = glfwGetTime();
        GLfloat deltaTime   = now - lastFrame;
        lastFrame           = now;
        GLfloat timeStep    = deltaTime * timeChange;
        ++counter;

        // Update FPS counter
        if (counter == 30)
        {
            std::string frameRateStr {"Solar System - " + std::to_string(30.0 / (now - lastFPSUpdateTime)) + " FPS"};
            glfwSetWindowTitle(window::mainWindow, frameRateStr.c_str());
            lastFPSUpdateTime = now;
            counter = 0;
        }

        orbitalPhysics::updateCelestialBodyAngles(timeStep);

        // Update our object's positions based on our chosen numerical scheme
        if (orbitalPhysics::verlet)
            orbitalPhysics::updatePositionsVerlet(timeSinceLastVerlet);
        else
            orbitalPhysics::updatePositionsEuler(timeStep);

        // Get + handle user input
        glfwPollEvents();
        bool* keys {window::keys};
        camera::keyControl(keys, deltaTime);
        camera::mouseControl(window::getXChange(), window::getYChange());
        handleTimeChange(window::getYScrollOffset(), timeChange);

        // Check for flashlight toggle
        if (keys[GLFW_KEY_L])
        {
            // Setting this to false means it won't trigger multiple times when we press it once
            keys[GLFW_KEY_L] = false;

            renderer::toggleShadows();
        }

        renderer::omniShadowMapPasses();
        renderer::renderPass();

        glUseProgram(0);
        glfwSwapBuffers(window::mainWindow);
    }

    return 0;
}
