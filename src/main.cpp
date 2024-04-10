#define STB_IMAGE_IMPLEMENTATION

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
#include "Skybox.h"

GLuint hdrFBO;
GLuint postProcessingTexture;
GLuint bloomTexture;
GLuint binaryTexture;
Mesh* framebufferQuad;

unsigned int pingPongFBO[2];
unsigned int pingPongBuffer[2];

const float toRadians = M_PI / 180.0f;

GLuint uniformModelPlanets = 0, uniformViewPlanets = 0,
uniformEyePositionPlanets = 0, uniformSpecularIntensityPlanets = 0, uniformShininessPlanets = 0,
uniformOmniLightPos = 0, uniformFarPlane = 0;

GLuint uniformModelSuns = 0, uniformViewSuns = 0;
GLuint uniformModelDirectionalShadowMap = 0;
GLuint uniformModelOmniShadowMap = 0;
GLuint uniformBlurTexture = 0;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<SpaceObject*> satellites;

Shader* mainShader;
Shader* sunShader;
Shader* omniShadowShader;
Shader* hdrShader;
Shader* bloomShader;

bool shadowsEnabled = false;

Camera camera;

unsigned int pointLightCount = 0;
bool flashLightOn = false;

PointLight* pointLights[MAX_POINT_LIGHTS];

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat timeChange = 1.0f;

GLfloat gForce = -100.0f;

static const char* vShaderShadows = "../assets/shaders/planetShaderShadows.vert";
static const char* fShaderShadows = "../assets/shaders/planetShaderShadows.frag";
static const char* vShaderNoShadows = "../assets/shaders/planetShaderNoShadows.vert";
static const char* fShaderNoShadows = "../assets/shaders/planetShaderNoShadows.frag";
static const char* vSunShader = "../assets/shaders/sunShader.vert";
static const char* fSunShader = "../assets/shaders/sunShader.frag";
static const char* vHdrShader = "../assets/shaders/hdrShader.vert";
static const char* fHdrShader = "../assets/shaders/hdrShader.frag";
static const char* fBloomShader = "../assets/shaders/bloomShader.frag";

void createSecondaryShaders()
{
    // Shader for the suns (no lighting or shadows)
    sunShader = new Shader();
    sunShader->createFromFiles(vSunShader, fSunShader);
    sunShader->useShader();
	sunShader->setTexture(2);
    sunShader->validate();

    omniShadowShader = new Shader();
	omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
		"../assets/shaders/omni_shadow_map.geom",
	  	"../assets/shaders/omni_shadow_map.frag");

    hdrShader = new Shader();
    hdrShader->createFromFiles(vHdrShader, fHdrShader);
    hdrShader->useShader();
    glUniform1i(glGetUniformLocation(hdrShader->getShaderID(), "theTexture"), 0);
    glUniform1i(glGetUniformLocation(hdrShader->getShaderID(), "blurTexture"), 1);
    glUniform1i(glGetUniformLocation(hdrShader->getShaderID(), "shouldGammaCorrectTexture"), 2);

    bloomShader = new Shader();
    bloomShader->createFromFiles(vHdrShader, fBloomShader);
    bloomShader->useShader();
    bloomShader->setTexture(0);
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

void renderSatellites(GLuint uniformModel)
{
    // Apply rotations, transformations, and render objects
    // Objects vertices are first transformed by the model matrix, then the view matrix
    // to bring them into camera space, positioning them relative to the camera,
    // then the projection matrix is applied to the view space coordinates to project them
    // onto our 2D screen. 
    // In the shader: gl_Position = projection * view * model * vec4(pos, 1.0);

    // Model matrix positions and orients objects in the world.
    // Takes coordinates local to the ojbect and transforms them into coordinates relative to world space.
    glm::mat4 model;

    // They'll all use GL_TEXTURE2
    glActiveTexture(GL_TEXTURE2);
    for (SpaceObject *satellite : satellites)
    {
        model = glm::mat4(1.0f);
        satellite->setWorldProperties(&model);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        satellite->getMaterialPointer()->useMaterial(uniformSpecularIntensityPlanets, uniformShininessPlanets);
        satellite->render();
    }
}

void renderSuns()
{
    glm::mat4 model;

    // They'll all use GL_TEXTURE2
    glActiveTexture(GL_TEXTURE2);
    for (Sun *star : stars)
    {
        model = glm::mat4(1.0f);
        star->setWorldProperties(&model);
        glUniformMatrix4fv(uniformModelSuns, 1, GL_FALSE, glm::value_ptr(model));
        star->getTexturePointer()->useTexture();
        star->render();
    }
}

void omniShadowMapPass(PointLight* light)
{
	omniShadowShader->useShader();

	// Make the viewport the same dimenstions as the FBO
	glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    // Bind our framebuffer so that the shader output goes to it
    // Every draw call after this will go to that framebuffer
	light->getShadowMap()->write();
	glClear(GL_DEPTH_BUFFER_BIT);

    // Get the uniformModel and lightTransform for the shader

	glUniformMatrix3fv(uniformOmniLightPos, 1, GL_FALSE, glm::value_ptr(light->getPosition()));
	glUniform1f(uniformFarPlane, light->getFarPlane());
	omniShadowShader->setLightMatrices(light->calculateLightTransform());

	omniShadowShader->validate();

	// Draw just to the depth buffer
	renderSatellites(uniformModelOmniShadowMap);

    // Bind the default framebuffer
    // If we called swapbuffers without doing this the image wouldn't change
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 view)
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Clear hdr buffer
	glViewport(0, 0, 1920, 1200);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLfloat binaryClearColor[4] = {0.5, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 2, binaryClearColor);

    // ====================================
    // RENDER SUNS
    // ====================================

    sunShader->useShader();
    glUniformMatrix4fv(uniformViewSuns, 1, GL_FALSE, glm::value_ptr(view));
    renderSuns();

    // ====================================
    // RENDER PLANETS, MOONS, and ASTEROIDS
    // ====================================

	mainShader->useShader();
    mainShader->setSpotLightDirAndPos(camera.getSpotLight(), shadowsEnabled, 4+pointLightCount, pointLightCount);

    //// Apply projection and view matrices.
    //// Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix.
    //// View matrix represents the camera's position and orientation in world.
    //// The world is actually rotated around the camera with the view matrix. The camera is stationary.
    glUniformMatrix4fv(uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(uniformEyePositionPlanets, 1, glm::value_ptr(camera.getPosition()));

 	//// Now we're not drawing just to the depth buffer but also the color buffer
	renderSatellites(uniformModelPlanets);

    // ====================================
    // RENDER SKYBOX
    // ====================================

    // Skybox goes last so that post-processing effects don't completely overwrite the skybox texture
    skybox.drawSkybox(view);

    // ====================================
    // BLOOM EFFECT
    // ====================================
    bool horizontal = false;
    int amount = 4;
    bloomShader->useShader();
    glActiveTexture(GL_TEXTURE0);

    // First iteration of the bloom effect. This means we don't need if conditions in the for loop
    glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[!horizontal]);
    glUniform1i(glGetUniformLocation(bloomShader->getShaderID(), "horizontal"), !horizontal);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    framebufferQuad->render();

    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
        glUniform1i(glGetUniformLocation(bloomShader->getShaderID(), "horizontal"), horizontal);

        // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
        // Move the data between pingPong FBOs
        glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);
        framebufferQuad->render();
        horizontal = !horizontal;
    }

    // ====================================
    // RENDER TO SCREEN
    // ====================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrShader->useShader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, binaryTexture);

    framebufferQuad->render();
}

int main()
{
    // Promp user to select a numerical integration scheme
    bool verlet;
    std::cout << "0: Euler method\n1: Verlet method \n>";
    std::cin >> verlet;

    // Prompt user to select scene
    int selectedScene;
    std::cout << "\n1: 1 planet 1 sun\n2: Lots of objects\n3: figure eight\n> ";
    std::cin >> selectedScene;

    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);

    // Build scene based on user input
    switch (selectedScene)
    {
        case (1):
            SceneFunctions::createObjects1Sun1Planet(stars, satellites, pointLights, &pointLightCount, &camera, verlet);
            break;
        case (2):
            SceneFunctions::createObjectsDefault(stars, satellites, pointLights, &pointLightCount, &camera, verlet);
            break;
        case (3):
            SceneFunctions::createObjectsFigureEight(stars, satellites, pointLights, &pointLightCount, &camera, verlet);
            break;
        default:
            break;
    }
    if (verlet)
        SceneFunctions::setOldPositions(satellites, stars, gForce);
    
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("../assets/textures/skybox/rightImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/leftImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/upImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/downImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");

	skybox = Skybox(skyboxFaces);
    skybox.setProjectionMatrix(projection);
    
    // Setup the OpenGL program
    createSecondaryShaders();

    // Shader for the satellites, moons, and models. Includes shadows
    Shader* mainShaderWithShadows = new Shader();
    mainShaderWithShadows->createFromFiles(vShaderShadows, fShaderShadows);
    mainShaderWithShadows->useShader();
	mainShaderWithShadows->setTexture(2);
    mainShaderWithShadows->setDirectionalShadowMap(3);
    mainShaderWithShadows->validate();
    
    // Shader for the satellites, moons, and models. Doesn't use shadows
    Shader* mainShaderWithoutShadows = new Shader();
    mainShaderWithoutShadows->createFromFiles(vShaderNoShadows, fShaderNoShadows);
    mainShaderWithoutShadows->useShader();
	mainShaderWithoutShadows->setTexture(2);
    mainShaderWithoutShadows->validate();

    // This is so we can disable shadows
    // By default, shadows will be turned off
    mainShader = mainShaderWithoutShadows;

    GLfloat now;
    GLfloat lastVerletUpdate = 0.0f;
    GLfloat timeStep = 0.0f;

    // Uniform object IDs let us pass values from the CPU to the GPU.
    // We use things like glUniform1f (for one value) to set these values
    // on the GPU
    GLuint uniformModelPlanetsShadows = mainShaderWithShadows->getModelLocation();
    GLuint uniformViewPlanetsShadows = mainShaderWithShadows->getViewLocation();
    GLuint uniformEyePositionPlanetsShadows = mainShaderWithShadows->getEyePositionLocation();
    GLuint uniformSpecularIntensityPlanetsShadows = mainShaderWithShadows->getSpecularIntensityLocation();
    GLuint uniformShininessPlanetsShadows = mainShaderWithShadows->getShininessLocation();

    mainShaderWithShadows->useShader();
    mainShaderWithShadows->setSpotLight(camera.getSpotLight(), true, 4+pointLightCount, pointLightCount);
    glUniformMatrix4fv(glGetUniformLocation(mainShaderWithShadows->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
    // texture, and the third is the texture(s) of the objects we're rendering
	mainShaderWithShadows->setPointLights(pointLights, pointLightCount, 4, 0);

    GLuint uniformModelPlanetsNoShadows = mainShaderWithoutShadows->getModelLocation();
    GLuint uniformViewPlanetsNoShadows = mainShaderWithoutShadows->getViewLocation();
    GLuint uniformEyePositionPlanetsNoShadows = mainShaderWithoutShadows->getEyePositionLocation();
    GLuint uniformSpecularIntensityPlanetsNoShadows = mainShaderWithoutShadows->getSpecularIntensityLocation();
    GLuint uniformShininessPlanetsNoShadows = mainShaderWithoutShadows->getShininessLocation();

    mainShaderWithoutShadows->useShader();
    mainShaderWithoutShadows->setSpotLight(camera.getSpotLight(), false, 4+pointLightCount, pointLightCount);
    glUniformMatrix4fv(glGetUniformLocation(mainShaderWithoutShadows->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	mainShaderWithoutShadows->setPointLightsWithoutShadows(pointLights, pointLightCount);


    uniformModelPlanets = uniformModelPlanetsNoShadows;
    uniformViewPlanets = uniformViewPlanetsNoShadows;
    uniformEyePositionPlanets = uniformEyePositionPlanetsNoShadows;
    uniformSpecularIntensityPlanets = uniformSpecularIntensityPlanetsNoShadows;
    uniformShininessPlanets = uniformShininessPlanetsNoShadows;

    // For the sun shaders we don't do any light or shadow calculations
    uniformModelSuns = sunShader->getModelLocation();
    uniformViewSuns = sunShader->getViewLocation();
    sunShader->useShader();
    glUniformMatrix4fv(sunShader->getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));

    // The shadow map shader will record the depth values of all objects except suns
    uniformModelOmniShadowMap = omniShadowShader->getModelLocation();
	uniformOmniLightPos = omniShadowShader->getOmniLightPosLocation();
	uniformFarPlane = omniShadowShader->getFarPlaneLocation();

    float quadVertices[] = {
        // Positions     // Texture Coordinates
        1.0f,  1.0f,    1.0f, 1.0f,  // Top Right
        1.0f, -1.0f,    1.0f, 0.0f,  // Bottom Right
       -1.0f, -1.0f,    0.0f, 0.0f,  // Bottom Left
       -1.0f,  1.0f,    0.0f, 1.0f   // Top Left 
    };
    unsigned int quadIndices[] = {
        0, 1, 3,  // First Triangle (Top Right, Bottom Right, Top Left)
        1, 2, 3   // Second Triangle (Bottom Right, Bottom Left, Top Left)
    };
    framebufferQuad = new Mesh();
    framebufferQuad->createMesh(quadVertices, quadIndices, 16, 6, false, false);

    // HDR
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glViewport(0, 0, 1920, 1200);

    // Make the main framebuffer texture
    glGenTextures(1, &postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

    // Make texture for bright spots so we cacn have bloom
    glGenTextures(1, &bloomTexture);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

    // Make the binary texture which will store whether or not we apply gamma correction to a pixel
    glGenTextures(1, &binaryTexture);
    glBindTexture(GL_TEXTURE_2D, binaryTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1200, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, binaryTexture, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1200);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer Error: %i\n", status);
        return false;
    }

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenFramebuffers(2, pingPongFBO);
    glGenTextures(2, pingPongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingPongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongBuffer[i], 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Framebuffer Error: %i\n", status);
            return false;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Loop until window is closed
    unsigned int counter = 0;
    double lastUpdateTime = glfwGetTime();
    while(!mainWindow.getShouldClose())
    {
        now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        timeStep = deltaTime * timeChange;
        counter++;

        // Update FPS counter
        if (counter == 30)
        {
            double timeElapsed = now - lastUpdateTime;
            std::string FPS = std::to_string(30.0 / timeElapsed);
            std::string newTitle = "Solar System - " + FPS + " FPS";
            mainWindow.setWindowTitle(newTitle);
            lastUpdateTime = now;
            counter = 0;
        }

        if (verlet)
        {
            while (now - lastVerletUpdate >= 0.005f)
            {
                OrbitalPhysicsFunctions::updateCelestialBodyAngles(stars, satellites, 0.005f);
                OrbitalPhysicsFunctions::updatePositionsVerlet(stars, satellites, gForce, 0.005f);
                lastVerletUpdate += 0.005f;
            }
        }
        else
        {
            // This loop ensures that we follow a curve even when the framerate sucks
            OrbitalPhysicsFunctions::updateCelestialBodyAngles(stars, satellites, timeStep);
            while (timeStep > 0.005f)
            {
                OrbitalPhysicsFunctions::updatePositionsEuler(stars, satellites, gForce, 0.005f);
                timeStep -= 0.005f;
            }
            OrbitalPhysicsFunctions::updatePositionsEuler(stars, satellites, gForce, timeStep);
        }

        // Get + handle user input
        glfwPollEvents();
        bool* keys = mainWindow.getKeys();
        camera.keyControl(keys, deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset());

        if (keys[GLFW_KEY_L])
        {
            keys[GLFW_KEY_L] = false;
            shadowsEnabled = !shadowsEnabled;

            if (shadowsEnabled)
            {
                mainShader = mainShaderWithShadows;
                uniformModelPlanets = uniformModelPlanetsShadows;
                uniformViewPlanets = uniformViewPlanetsShadows;
                uniformEyePositionPlanets = uniformEyePositionPlanetsShadows;
                uniformSpecularIntensityPlanets = uniformSpecularIntensityPlanetsShadows;
                uniformShininessPlanets = uniformShininessPlanetsShadows;
            }
            else
            {
                mainShader = mainShaderWithoutShadows;
                uniformModelPlanets = uniformModelPlanetsNoShadows;
                uniformViewPlanets = uniformViewPlanetsNoShadows;
                uniformEyePositionPlanets = uniformEyePositionPlanetsNoShadows;
                uniformSpecularIntensityPlanets = uniformSpecularIntensityPlanetsNoShadows;
                uniformShininessPlanets = uniformShininessPlanetsNoShadows;
            }
        }

        if (shadowsEnabled)
        {
            // These needs to be index based loops so that we don't make a copy of the lights each time
            for (size_t i = 0; i < pointLightCount; i++)
                omniShadowMapPass(pointLights[i]);
            omniShadowMapPass(camera.getSpotLight());
        }

        renderPass(camera.calculateViewMatrix());

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
