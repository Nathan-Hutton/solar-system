#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Sun.h"
#include "DirectionalLight.h"
#include "SceneHandler.h"
#include "OrbitalPhysics.h"
#include "Material.h"

const float toRadians = M_PI / 180.0f;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<Sphere*> satellites;
std::vector<Shader*> shaderList;
Camera camera;

DirectionalLight mainLight;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat timeChange = 1.0f;

GLfloat gravitationalForce = -100.0f;

// vertex shader
static const char* vShader = "shaders/shader.vert";
// Fragment shader
static const char* fShader = "shaders/shader.frag";
static const char* fShader2 = "shaders/shader.frag";

void createShaders()
{
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    Shader *shader2 = new Shader();
    shader2->createFromFiles(vShader, fShader2);
    shaderList.push_back(shader2);
}

void handleTimeChange(GLfloat yScrollOffset)
{
    if (yScrollOffset == 0.0f) return;

    timeChange += (yScrollOffset * 0.1f);
    if (timeChange > 3.0f)
    {
        timeChange = 3.0f;
        return;
    }
    if (timeChange < 0.0f)
        timeChange = 0.0f;
}

int main()
{
    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    //SceneFunctions::create1Sun1Planet(stars, satellites, &camera);
    //SceneFunctions::createObjectsDefault(stars, satellites, &camera);
    SceneFunctions::createObjectsFigureEight(stars, satellites, &camera);
    createShaders();

    GLfloat now;
    GLfloat timeStep = 0.0f;
    glm::mat4 model;

    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);
    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
                                0.2f, 1.0f,
                                1.0f, 0.0f, 0.0f);

    // These uniform objectd IDs connect us with the values in the shaders in the GPU
    GLuint uniformModel = shaderList[0]->getModelLocation();
    GLuint uniformProjection = shaderList[0]->getProjectionLocation();
    GLuint uniformView = shaderList[0]->getViewLocation();

    GLuint uniformAmbientIntensity = shaderList[0]->getAmbientIntensityLocation();
    GLuint uniformLightColor = shaderList[0]->getLightColorLocation();
    GLuint uniformDiffuseIntensity = shaderList[0]->getDiffuseIntensityLocation();
    GLuint uniformDirection = shaderList[0]->getDirectionLocation();

    GLuint uniformEyePosition = shaderList[0]->getEyePositionLocation();
    GLuint uniformSpecularIntensity = shaderList[0]->getSpecularIntensityLocation();
    GLuint uniformShininess = shaderList[0]->getShininessLocation();

    // Uncomment out this code when we want different shaders for planets and stars
    // Apply shaders to satellites
    //shaderList[1]->useShader();
    //GLuint uniformModel1 = shaderList[1]->getModelLocation();
    //GLuint uniformProjection1 = shaderList[1]->getProjectionLocation();
    //GLuint uniformView1 = shaderList[1]->getViewLocation();
    //GLuint uniformAmbientIntensity1 = shaderList[1]->getAmbientIntensityLocation();
    //GLuint uniformLightColor1 = shaderList[1]->getLightColorLocation();
    //GLuint uniformDiffuseIntensity1 = shaderList[1]->getDiffuseIntensityLocation();
    //GLuint uniformDirection1 = shaderList[1]->getDirectionLocation();
    //GLuint uniformEyePosition1 = shaderList[1]->getEyePositionLocation();
    //GLuint uniformSpecularIntensity1 = shaderList[1]->getSpecularIntensityLocation();
    //GLuint uniformShininess1 = shaderList[1]->getShininessLocation();

    // Loop until window is closed
    while(!mainWindow.getShouldClose())
    {
        now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        timeStep = deltaTime * timeChange;

        // This loop ensures that we follow a curve even when the framerate sucks
        OrbitalPhysicsFunctions::updateCelestialBodyAngles(stars, satellites, timeStep);
        while (timeStep > 0.005f)
        {
            OrbitalPhysicsFunctions::updateSatellitePositions(stars, satellites, gravitationalForce, 0.005f);
            timeStep -= 0.005f;
        }
        OrbitalPhysicsFunctions::updateSatellitePositions(stars, satellites, gravitationalForce, timeStep);

        // Get + Handle user input events
        glfwPollEvents();
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset());

        // Clear window and color and depth buffer bit
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderList[0]->useShader();

        // Apply projection and view matrices
        mainLight.useLight(uniformAmbientIntensity, uniformLightColor, uniformDiffuseIntensity, uniformDirection);
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

        // Apply rotations, transformations, and render objects
        for (Sun *star : stars)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, star->getPosition());
            model = glm::rotate(model, star->getAngle() * toRadians, star->getRotation());
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            star->renderMesh(uniformSpecularIntensity, uniformShininess);
        }
        for (Sphere *satellite : satellites)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, satellite->getPosition());
            model = glm::rotate(model, satellite->getAngle() * toRadians, satellite->getRotation());
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            satellite->renderMesh(uniformSpecularIntensity, uniformShininess);
        }
        
        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
