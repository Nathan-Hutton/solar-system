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

#include "CommonValues.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Sun.h"
#include "Planet.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneHandler.h"
#include "OrbitalPhysics.h"
#include "Material.h"
#include "Model.h"

const float toRadians = M_PI / 180.0f;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<Planet*> planets;
std::vector<Model*> complexModels;
std::vector<Shader*> shaderList;
Camera camera;

DirectionalLight mainLight;
PointLight* pointLights[MAX_POINT_LIGHTS];
SpotLight* spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat timeChange = 1.0f;

GLfloat gravitationalForce = -100.0f;

static const char* vShader = "../assets/shaders/shader.vert";
static const char* fShader = "../assets/shaders/planetShader.frag";
static const char* fShader2 = "../assets/shaders/sunShader.frag";

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

    unsigned int pointLightCount = 0;
    unsigned int spotLightCount = 0;
    //SceneFunctions::createObjects1Sun1Planet(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    SceneFunctions::createObjectsDefault(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    //SceneFunctions::createObjectsFigureEight(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    createShaders();

    GLfloat now;
    GLfloat timeStep = 0.0f;
    glm::mat4 model;

    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);
    mainLight = DirectionalLight(0.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f,
                                1.0f, 0.0f, 0.0f);

    // Uniform object IDs connect us with the values in the shaders in the GPU
    GLuint uniformModelPlanets = shaderList[0]->getModelLocation();
    GLuint uniformProjectionPlanets = shaderList[0]->getProjectionLocation();
    GLuint uniformViewPlanets = shaderList[0]->getViewLocation();
    GLuint uniformEyePositionPlanets = shaderList[0]->getEyePositionLocation();
    GLuint uniformSpecularIntensityPlanets = shaderList[0]->getSpecularIntensityLocation();
    GLuint uniformShininessPlanets = shaderList[0]->getShininessLocation();

    GLuint uniformModelSuns = shaderList[1]->getModelLocation();
    GLuint uniformProjectionSuns = shaderList[1]->getProjectionLocation();
    GLuint uniformViewSuns = shaderList[1]->getViewLocation();

    // Loop until window is closed
    while(!mainWindow.getShouldClose())
    {
        now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        timeStep = deltaTime * timeChange;

        // This loop ensures that we follow a curve even when the framerate sucks
        OrbitalPhysicsFunctions::updateCelestialBodyAngles(stars, planets, complexModels, timeStep);
        while (timeStep > 0.005f)
        {
            OrbitalPhysicsFunctions::updateSatellitePositions(stars, planets, complexModels, gravitationalForce, 0.005f);
            timeStep -= 0.005f;
        }
        OrbitalPhysicsFunctions::updateSatellitePositions(stars, planets, complexModels, gravitationalForce, timeStep);

        glfwPollEvents();
        camera.keyControl(mainWindow.getKeys(), deltaTime, &spotLightCount);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset());

        // Clear window and color and depth buffer bit
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderList[0]->useShader();


        // Apply projection and view matrices
        // Project Converts 3D coordinates to 2D coordinates in rendering pipeline
        // View matrix represents the camera's position and orientation in world.
        // View matrix transforms world coordinates to camera/view coordinates
        glUniformMatrix4fv(uniformProjectionPlanets, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePositionPlanets, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

        //shaderList[0]->setDirectionalLight(&mainLight);
        shaderList[0]->setPointLights(pointLights, pointLightCount);
        shaderList[0]->setSpotLights(spotLights, spotLightCount);

        // Apply rotations, transformations, and render objects
        for (Planet *satellite : planets)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, satellite->getPosition());
            model = glm::rotate(model, satellite->getAngle() * toRadians, satellite->getRotation());
            glUniformMatrix4fv(uniformModelPlanets, 1, GL_FALSE, glm::value_ptr(model));
            satellite->getMaterialPointer()->useMaterial(uniformSpecularIntensityPlanets, uniformShininessPlanets);
            satellite->renderMesh();
        }

        for (Model *complexModel : complexModels)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, complexModel->getPosition());
            model = glm::rotate(model, complexModel->getAngle() * toRadians, complexModel->getRotation());
            glUniformMatrix4fv(uniformModelSuns, 1, GL_FALSE, glm::value_ptr(model));
            complexModel->renderModel();
        }

        shaderList[1]->useShader();
        glUniformMatrix4fv(uniformProjectionSuns, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformViewSuns, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        for (Sun *star : stars)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, star->getPosition());
            model = glm::rotate(model, star->getAngle() * toRadians, star->getRotation());
            glUniformMatrix4fv(uniformModelSuns, 1, GL_FALSE, glm::value_ptr(model));
            star->renderMesh();
        }
        
        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
