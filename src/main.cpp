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
Mesh* framebufferQuad;

unsigned int pingPongFBO[2];
unsigned int pingPongBuffer[2];

const float toRadians = M_PI / 180.0f;

GLuint uniformModelPlanets = 0, uniformProjectionPlanets = 0, uniformViewPlanets = 0,
uniformEyePositionPlanets = 0, uniformSpecularIntensityPlanets = 0, uniformShininessPlanets = 0,
uniformOmniLightPos = 0, uniformFarPlane = 0;

GLuint uniformModelPlanetsShadows = 0, uniformProjectionPlanetsShadows = 0, uniformViewPlanetsShadows = 0,
uniformEyePositionPlanetsShadows = 0, uniformSpecularIntensityPlanetsShadows = 0, uniformShininessPlanetsShadows = 0;
GLuint uniformModelPlanetsNoShadows = 0, uniformProjectionPlanetsNoShadows = 0, uniformViewPlanetsNoShadows = 0,
uniformEyePositionPlanetsNoShadows = 0, uniformSpecularIntensityPlanetsNoShadows = 0, uniformShininessPlanetsNoShadows = 0;

GLuint uniformModelSuns = 0, uniformProjectionSuns = 0, uniformViewSuns = 0;
GLuint uniformModelDirectionalShadowMap = 0;
GLuint uniformModelOmniShadowMap = 0;
GLuint uniformBlurTexture = 0;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<Planet*> planets;
std::vector<Model*> complexModels;

Shader* mainShaderWithShadows;
Shader* mainShaderWithoutShadows;
Shader* sunShader;
Shader* omniShadowShader;
Shader* hdrShader;
Shader* bloomShader;

bool shadowsEnabled = false;

Camera camera;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

PointLight* pointLights[MAX_POINT_LIGHTS];
SpotLight* spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat timeChange = 1.0f;

GLfloat gravitationalForce = -100.0f;

static const char* vShaderShadows = "../assets/shaders/planetShaderShadows.vert";
static const char* fShaderShadows = "../assets/shaders/planetShaderShadows.frag";
static const char* vShaderNoShadows = "../assets/shaders/planetShaderNoShadows.vert";
static const char* fShaderNoShadows = "../assets/shaders/planetShaderNoShadows.frag";
static const char* vSunShader = "../assets/shaders/sunShader.vert";
static const char* fSunShader = "../assets/shaders/sunShader.frag";
static const char* vHdrShader = "../assets/shaders/hdrShader.vert";
static const char* fHdrShader = "../assets/shaders/hdrShader.frag";
static const char* fBloomShader = "../assets/shaders/bloomShader.frag";

void createShaders()
{
    // Shader for the planets, moons, and models. Includes shadows
    mainShaderWithShadows = new Shader();
    mainShaderWithShadows->createFromFiles(vShaderShadows, fShaderShadows);
    mainShaderWithShadows->useShader();
	mainShaderWithShadows->setTexture(2);
    mainShaderWithShadows->setDirectionalShadowMap(3);
    mainShaderWithShadows->validate();


    // Shader for the planets, moons, and models. Doesn't use shadows
    mainShaderWithoutShadows = new Shader();
    mainShaderWithoutShadows->createFromFiles(vShaderNoShadows, fShaderNoShadows);
    mainShaderWithoutShadows->useShader();
	mainShaderWithoutShadows->setTexture(2);
    mainShaderWithoutShadows->validate();


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
    hdrShader->setTexture(0);
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

void renderPlanets(GLuint uniformModel)
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
    for (Planet *satellite : planets)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, satellite->getPosition());
        model = glm::rotate(model, satellite->getAngle() * toRadians, satellite->getRotation());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        satellite->getMaterialPointer()->useMaterial(uniformSpecularIntensityPlanets, uniformShininessPlanets);
        satellite->getTexturePointer()->useTexture();
        satellite->renderMesh();
    }

    for (Model *complexModel : complexModels)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, complexModel->getPosition());
        model = glm::rotate(model, complexModel->getAngle() * toRadians, complexModel->getRotation());
        model = glm::scale(model, complexModel->getScaleFactorVector());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        complexModel->getMaterialPointer()->useMaterial(uniformSpecularIntensityPlanets, uniformShininessPlanets);
        complexModel->renderModel();
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
        model = glm::translate(model, star->getPosition());
        model = glm::rotate(model, star->getAngle() * toRadians, star->getRotation());
        glUniformMatrix4fv(uniformModelSuns, 1, GL_FALSE, glm::value_ptr(model));
        star->getTexturePointer()->useTexture();
        star->renderMesh();
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

	glm::vec3 position = light->getPosition();
	glUniform3f(uniformOmniLightPos, position.x, position.y, position.z);
	glUniform1f(uniformFarPlane, light->getFarPlane());
	omniShadowShader->setLightMatrices(light->calculateLightTransform());

	omniShadowShader->validate();

	// Draw just to the depth buffer
	renderPlanets(uniformModelOmniShadowMap);

    // Bind the default framebuffer
    // If we called swapbuffers without doing this the image wouldn't change
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPassWithoutShadows(glm::mat4 projection, glm::mat4 view)
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Clear hdr buffer
	glViewport(0, 0, 1920, 1200);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sunShader->useShader();
    glUniformMatrix4fv(uniformProjectionSuns, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewSuns, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

    renderSuns();

    // ====================================
    // RENDER PLANETS, MOONS, and ASTEROIDS
    // ====================================

	mainShaderWithoutShadows->useShader();

    //// Apply projection and view matrices.
    //// Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix.
    //// View matrix represents the camera's position and orientation in world.
    //// The world is actually rotated around the camera with the view matrix. The camera is stationary.
    glUniformMatrix4fv(uniformProjectionPlanets, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(uniformEyePositionPlanets, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

	mainShaderWithoutShadows->setPointLightsWithoutShadows(pointLights, pointLightCount);
	mainShaderWithoutShadows->setSpotLightsWithoutShadows(spotLights, spotLightCount);

 	//// Now we're not drawing just to the depth buffer but also the color buffer
	renderPlanets(uniformModelPlanets);

    // ====================================
    // RENDER SKYBOX
    // ====================================

    // Skybox goes last so that post-processing effects don't completely overwrite the skybox texture
    skybox.drawSkybox(view, projection);

    bool horizontal = true, first_iteration = true;
    int amount = 5;
    bloomShader->useShader();
    glActiveTexture(GL_TEXTURE0);
    // TODO: Move the first iteration out of the for loop so it doesn't have to check every time
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
        glUniform1i(glGetUniformLocation(bloomShader->getShaderID(), "horizontal"), horizontal);

        // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
        if (first_iteration)
        {
            glBindTexture(GL_TEXTURE_2D, bloomTexture);
            first_iteration = false;
        }
        // Move the data between pingPong FBOs
        else
        {
            glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);
        }
        framebufferQuad->renderMesh();
        horizontal = !horizontal;
    }

    // Now that we've rendered everything to a texture, we'll render
    // it to the screen with some post-processing effects
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrShader->useShader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);
    glUniform1i(glGetUniformLocation(hdrShader->getShaderID(), "blurTexture"), 1);

    framebufferQuad->renderMesh();
}

void renderPassWithShadows(glm::mat4 projection, glm::mat4 view)
{
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Set viewport. Clear window, color, and depth buffer bit. Make the image black
	glViewport(0, 0, 1920, 1200);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ====================================
    // RENDER SUNS
    // ====================================

    sunShader->useShader();
    glUniformMatrix4fv(uniformProjectionSuns, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewSuns, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

    renderSuns();

    // ====================================
    // RENDER PLANETS, MOONS, and ASTEROIDS
    // ====================================

	mainShaderWithShadows->useShader();

    // Apply projection and view matrices.
    // Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix.
    // View matrix represents the camera's position and orientation in world.
    // The world is actually rotated around the camera with the view matrix. The camera is stationary.
    glUniformMatrix4fv(uniformProjectionPlanets, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(uniformEyePositionPlanets, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

    // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
    // texture, and the third is the texture(s) of the objects we're rendering
	mainShaderWithShadows->setPointLights(pointLights, pointLightCount, 4, 0);
	mainShaderWithShadows->setSpotLights(spotLights, spotLightCount, 4 + pointLightCount, pointLightCount);

	// We need to be able to see whatever fragment we're trying to render from the perspective of the light
 	// Handle/bind the shadow map texture to texture unit 1
 	// Now we're not drawing just to the depth buffer but also the color buffer
	renderPlanets(uniformModelPlanets);

    // ====================================
    // RENDER SKYBOX
    // ====================================

    // Skybox goes last so that post-processing effects don't completely overwrite the skybox texture
    skybox.drawSkybox(view, projection);

    bool horizontal = true, first_iteration = true;
    int amount = 5;
    bloomShader->useShader();
    glActiveTexture(GL_TEXTURE0);
    // TODO: Move the first iteration out of the for loop so it doesn't have to check every time
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal]);
        glUniform1i(glGetUniformLocation(bloomShader->getShaderID(), "horizontal"), horizontal);

        // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
        if (first_iteration)
        {
            glBindTexture(GL_TEXTURE_2D, bloomTexture);
            first_iteration = false;
        }
        // Move the data between pingPong FBOs
        else
        {
            glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);
        }
        framebufferQuad->renderMesh();
        horizontal = !horizontal;
    }

    // Now that we've rendered everything to a texture, we'll render
    // it to the screen with some post-processing effects
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrShader->useShader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingPongBuffer[!horizontal]);
    glUniform1i(glGetUniformLocation(hdrShader->getShaderID(), "blurTexture"), 1);

    framebufferQuad->renderMesh();
}

int main()
{
    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    SceneFunctions::createObjects1Sun1Planet(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    //SceneFunctions::createObjectsDefault(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    //SceneFunctions::createObjectsFigureEight(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("../assets/textures/skybox/rightImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/leftImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/upImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/downImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");

	skybox = Skybox(skyboxFaces);
    
    // Setup the OpenGL program
    createShaders();

    GLfloat now;
    GLfloat timeStep = 0.0f;

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);

    // Uniform object IDs let us pass values from the CPU to the GPU.
    // We use things like glUniform1f (for one value) to set these values
    // on the GPU
    uniformModelPlanetsShadows = mainShaderWithShadows->getModelLocation();
    uniformProjectionPlanetsShadows = mainShaderWithShadows->getProjectionLocation();
    uniformViewPlanetsShadows = mainShaderWithShadows->getViewLocation();
    uniformEyePositionPlanetsShadows = mainShaderWithShadows->getEyePositionLocation();
    uniformSpecularIntensityPlanetsShadows = mainShaderWithShadows->getSpecularIntensityLocation();
    uniformShininessPlanetsShadows = mainShaderWithShadows->getShininessLocation();

    uniformModelPlanetsNoShadows = mainShaderWithoutShadows->getModelLocation();
    uniformProjectionPlanetsNoShadows = mainShaderWithoutShadows->getProjectionLocation();
    uniformViewPlanetsNoShadows = mainShaderWithoutShadows->getViewLocation();
    uniformEyePositionPlanetsNoShadows = mainShaderWithoutShadows->getEyePositionLocation();
    uniformSpecularIntensityPlanetsNoShadows = mainShaderWithoutShadows->getSpecularIntensityLocation();
    uniformShininessPlanetsNoShadows = mainShaderWithoutShadows->getShininessLocation();

    uniformModelPlanets = uniformModelPlanetsNoShadows;
    uniformProjectionPlanets = uniformProjectionPlanetsNoShadows;
    uniformViewPlanets = uniformViewPlanetsNoShadows;
    uniformEyePositionPlanets = uniformEyePositionPlanetsNoShadows;
    uniformSpecularIntensityPlanets = uniformSpecularIntensityPlanetsNoShadows;
    uniformShininessPlanets = uniformShininessPlanetsNoShadows;

    // For the sun shaders we don't do any light or shadow calculations
    uniformModelSuns = sunShader->getModelLocation();
    uniformProjectionSuns = sunShader->getProjectionLocation();
    uniformViewSuns = sunShader->getViewLocation();

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
    // 2.2f is the gamma number we're using since opengl wants to complain when I define
    // a "gamma" variable in this file
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

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

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

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

        // This loop ensures that we follow a curve even when the framerate sucks
        OrbitalPhysicsFunctions::updateCelestialBodyAngles(stars, planets, complexModels, timeStep);
        while (timeStep > 0.005f)
        {
            OrbitalPhysicsFunctions::updateSatellitePositions(stars, planets, complexModels, gravitationalForce, 0.005f);
            timeStep -= 0.005f;
        }
        OrbitalPhysicsFunctions::updateSatellitePositions(stars, planets, complexModels, gravitationalForce, timeStep);

        // Get + handle user input
        glfwPollEvents();
        bool* keys = mainWindow.getKeys();
        camera.keyControl(keys, deltaTime, &spotLightCount);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset());

        if (keys[GLFW_KEY_L])
        {
            keys[GLFW_KEY_L] = false;
            shadowsEnabled = !shadowsEnabled;

            if (shadowsEnabled)
            {
                uniformModelPlanets = uniformModelPlanetsShadows;
                uniformProjectionPlanets = uniformProjectionPlanetsShadows;
                uniformViewPlanets = uniformViewPlanetsShadows;
                uniformEyePositionPlanets = uniformEyePositionPlanetsShadows;
                uniformSpecularIntensityPlanets = uniformSpecularIntensityPlanetsShadows;
                uniformShininessPlanets = uniformShininessPlanetsShadows;
            }
            else
            {
                uniformModelPlanets = uniformModelPlanetsNoShadows;
                uniformProjectionPlanets = uniformProjectionPlanetsNoShadows;
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
            for (size_t i = 0; i < spotLightCount; i++)
                omniShadowMapPass(spotLights[i]);

            renderPassWithShadows(projection, camera.calculateViewMatrix());
        }
        else
        {
            renderPassWithoutShadows(projection, camera.calculateViewMatrix());
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
