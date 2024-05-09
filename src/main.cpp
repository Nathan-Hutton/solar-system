#define STB_IMAGE_IMPLEMENTATION

#include <cstdlib>
#include <iostream>
#include "SolarSystemRenderer.h"

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
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneHandler.h"
#include "OrbitalPhysics.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"

struct PostProcessingResources {
    GLuint halfFBO {};
    GLuint halfTexture {};
    GLuint postProcessingFBO {};
    GLuint postProcessingTexture {};
    GLuint textureToBlur {};
    Mesh* framebufferQuad {};
    unsigned int pingPongFBO[2] {};
    unsigned int pingPongBuffer[2] {};
};
PostProcessingResources postProcessingResources;

struct UniformVariables {
    GLuint uniformModelPlanets {};
    GLuint uniformViewPlanets {};
    GLuint uniformEyePositionPlanets {};
    GLuint uniformSpecularIntensityPlanets {};
    GLuint uniformShininessPlanets {};
    GLuint uniformOmniLightPos {};
    GLuint uniformFarPlane {};
    GLuint uniformModelSuns {};
    GLuint uniformViewSuns {};
    GLuint uniformModelOmniShadowMap {};
    GLuint uniformHorizontal {};
};
UniformVariables uniformVariables;

struct Shaders {
    Shader* mainShader {};
    Shader* mainShaderWithoutShadows {};
    Shader* mainShaderWithShadows {};
    Shader* sunShader {};
    Shader* omniShadowShader {};
    Shader* hdrShader {};
    Shader* bloomShader {};
    Shader* halfShader {};
};
Shaders shaders;

std::vector<SpaceObject*> stars {};
std::vector<SpaceObject*> satellites {};

Skybox skybox {};
Camera camera {};

bool shadowsEnabled {false};
GLuint pointLightCount {};

void createShaders(PointLight* pointLights[], glm::mat4 projection)
{
    // Shader for the suns (no lighting or shadows)
    shaders.sunShader = new Shader{};
    shaders.sunShader->createFromFiles("../assets/shaders/sunShader.vert", "../assets/shaders/sunShader.frag");
    shaders.sunShader->useShader();
    glUniformMatrix4fv(shaders.sunShader->getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
	shaders.sunShader->setTexture(2);
    shaders.sunShader->validate();
    // For the sun shaders we don't do any light or shadow calculations
    uniformVariables.uniformModelSuns    = shaders.sunShader->getModelLocation();
    uniformVariables.uniformViewSuns     = shaders.sunShader->getViewLocation();

    shaders.omniShadowShader = new Shader{};
	shaders.omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
		"../assets/shaders/omni_shadow_map.geom",
	  	"../assets/shaders/omni_shadow_map.frag");

    // The shadow map shader will record the depth values of all objects except suns
    uniformVariables.uniformModelOmniShadowMap   = shaders.omniShadowShader->getModelLocation();
	uniformVariables.uniformOmniLightPos         = shaders.omniShadowShader->getOmniLightPosLocation();
	uniformVariables.uniformFarPlane             = shaders.omniShadowShader->getFarPlaneLocation();

    shaders.hdrShader = new Shader{};
    shaders.hdrShader->createFromFiles("../assets/shaders/hdrShader.vert", "../assets/shaders/hdrShader.frag");
    shaders.hdrShader->useShader();
    glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "theTexture"), 0);
    glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "blurTexture"), 1);

    shaders.bloomShader = new Shader{};
    shaders.bloomShader->createFromFiles("../assets/shaders/bloomShader.vert",  "../assets/shaders/bloomShader.frag");
    shaders.bloomShader->useShader();
    shaders.bloomShader->setTexture(0);
    uniformVariables.uniformHorizontal = glGetUniformLocation(shaders.bloomShader->getShaderID(), "horizontal");
    
    // Shader for the satellites, moons, and models. Includes shadows
    shaders.mainShaderWithShadows = new Shader{};
    shaders.mainShaderWithShadows->createFromFiles("../assets/shaders/planetShaderShadows.vert", "../assets/shaders/planetShaderShadows.frag");
    shaders.mainShaderWithShadows->useShader();
	shaders.mainShaderWithShadows->setTexture(2);
    shaders.mainShaderWithShadows->validate();
    shaders.mainShaderWithShadows->setSpotLight(camera.getSpotLight(), true, 4+pointLightCount, pointLightCount);
    glUniformMatrix4fv(glGetUniformLocation(shaders.mainShaderWithShadows->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
    // texture, and the third is the texture(s) of the objects we're rendering
	shaders.mainShaderWithShadows->setPointLights(pointLights, pointLightCount, 4, 0);
    glUniform1i(glGetUniformLocation(shaders.mainShaderWithShadows->getShaderID(), "pointLightCount"), pointLightCount);

    shaders.halfShader = new Shader{};
    shaders.halfShader->createFromFiles("../assets/shaders/half.vert", "../assets/shaders/half.frag");
    shaders.halfShader->useShader();
    shaders.halfShader->setTexture(0);
    
    // Shader for the satellites, moons, and models. Doesn't use shadows
    shaders.mainShaderWithoutShadows = new Shader{};
    shaders.mainShaderWithoutShadows->createFromFiles("../assets/shaders/planetShaderNoShadows.vert", "../assets/shaders/planetShaderNoShadows.frag");
    shaders.mainShaderWithoutShadows->useShader();
	shaders.mainShaderWithoutShadows->setTexture(2);
    shaders.mainShaderWithoutShadows->validate();
    shaders.mainShaderWithoutShadows->setSpotLight(camera.getSpotLight(), false, 4+pointLightCount, pointLightCount);
    glUniformMatrix4fv(glGetUniformLocation(shaders.mainShaderWithoutShadows->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	shaders.mainShaderWithoutShadows->setPointLightsWithoutShadows(pointLights, pointLightCount);
    glUniform1i(glGetUniformLocation(shaders.mainShaderWithoutShadows->getShaderID(), "pointLightCount"), pointLightCount);

    // This is so we can disable shadows
    // By default, shadows will be turned off
    shaders.mainShader                                  = shaders.mainShaderWithoutShadows;
    uniformVariables.uniformModelPlanets                = shaders.mainShaderWithoutShadows->getModelLocation();
    uniformVariables.uniformViewPlanets                 = shaders.mainShaderWithoutShadows->getViewLocation();
    uniformVariables.uniformEyePositionPlanets          = shaders.mainShaderWithoutShadows->getEyePositionLocation();
    uniformVariables.uniformSpecularIntensityPlanets    = shaders.mainShaderWithoutShadows->getSpecularIntensityLocation();
    uniformVariables.uniformShininessPlanets            = shaders.mainShaderWithoutShadows->getShininessLocation();
}

void setupPostProcessingObjects()
{
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
    postProcessingResources.framebufferQuad = new Mesh{};
    postProcessingResources.framebufferQuad->createMesh(quadVertices, quadIndices, 16, 6, false, false);
    // HDR
    glGenFramebuffers(1, &postProcessingResources.postProcessingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);
	glViewport(0, 0, 1920, 1200);

    // Make the main framebuffer texture
    glGenTextures(1, &postProcessingResources.postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.postProcessingTexture, 0);

    // Make texture for bright spots so we cacn have bloom
    glGenTextures(1, &postProcessingResources.textureToBlur);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, postProcessingResources.textureToBlur, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned int RBO {};
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1200);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer error: " << status << '\n';
        std::exit(EXIT_FAILURE);
    }

    unsigned int attachments[2] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    // Setup the ping pong framebuffers to take in half-sized textures and output half-sized textures
    glGenFramebuffers(2, postProcessingResources.pingPongFBO);
    glGenTextures(2, postProcessingResources.pingPongBuffer);
    for (unsigned int i {0}; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[i], 0);

        GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer Error: " << status << '\n';
            std::exit(EXIT_FAILURE);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaders.hdrShader->useShader();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[1]);
    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "theTexture"), 3);
    glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "blurTexture"), 4);

    glGenFramebuffers(1, &postProcessingResources.halfFBO);
    glGenTextures(1, &postProcessingResources.halfTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.halfFBO);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.halfTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.halfTexture, 0);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer Error: " << status << '\n';
        std::exit(EXIT_FAILURE);
    }
}

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

void renderObjects(std::vector<SpaceObject*> objects, GLuint uniformModel)
{
    // Apply rotations, transformations, and render objects
    // Objects vertices are first transformed by the model matrix, then the view matrix
    // to bring them into camera space, positioning them relative to the camera,
    // then the projection matrix is applied to the view space coordinates to project them
    // onto our 2D screen. 
    // In the shader: gl_Position = projection * view * model * vec4(pos, 1.0);

    // Model matrix positions and orients objects in the world.
    // Takes coordinates local to the ojbect and transforms them into coordinates relative to world space.
    glm::mat4 model {};

    // They'll all use GL_TEXTURE2
    glActiveTexture(GL_TEXTURE2);
    for (SpaceObject *object : objects)
    {
        model = glm::mat4{1.0f};
        object->setWorldProperties(&model);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        object->render();
    }
}

void omniShadowMapPass(PointLight* light)
{
	shaders.omniShadowShader->useShader();

	// Make the viewport the same dimenstions as the FBO
	glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    // Bind our framebuffer so that the shader output goes to it
    // Every draw call after this will go to that framebuffer
	light->getShadowMap()->write();
	glClear(GL_DEPTH_BUFFER_BIT);

    // Get the uniformModel and lightTransform for the shader

	glUniform3fv(uniformVariables.uniformOmniLightPos, 1, glm::value_ptr(light->getPosition()));
	glUniform1f(uniformVariables.uniformFarPlane, light->getFarPlane());
	shaders.omniShadowShader->setLightMatrices(light->calculateLightTransform());

	shaders.omniShadowShader->validate();

	// Draw just to the depth buffer
	renderObjects(satellites, uniformVariables.uniformModelOmniShadowMap);

    // Bind the default framebuffer
    // If we called swapbuffers without doing this the image wouldn't change
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderPass(glm::mat4 view)
{
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);

    // Clear hdr buffer
	glViewport(0, 0, 1920, 1200);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // ====================================
    // RENDER SUNS
    // ====================================

    shaders.sunShader->useShader();
    glUniformMatrix4fv(uniformVariables.uniformViewSuns, 1, GL_FALSE, glm::value_ptr(view));
    renderObjects(stars, uniformVariables.uniformModelSuns);

    // ====================================
    // RENDER PLANETS, MOONS, and ASTEROIDS
    // ====================================

	shaders.mainShader->useShader();
    shaders.mainShader->setSpotLightDirAndPos(camera.getSpotLight(), shadowsEnabled, 4+pointLightCount, pointLightCount);

    //// Apply view matrix.
    //// View matrix represents the camera's position and orientation in world.
    //// The world is actually rotated around the camera with the view matrix. The camera is stationary.
    glUniformMatrix4fv(uniformVariables.uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(uniformVariables.uniformEyePositionPlanets, 1, glm::value_ptr(camera.getPosition()));

 	//// Now we're not drawing just to the depth buffer but also the color buffer
	renderObjects(satellites, uniformVariables.uniformModelPlanets);

    // ====================================
    // RENDER SKYBOX
    // ====================================

    // Skybox goes last so that post-processing effects don't completely overwrite the skybox texture
    skybox.drawSkybox(view);

    // ====================================
    // BLOOM EFFECT
    // ====================================

    // Half the bloom texture size before the pingPongFBOs can actually start bluring it
	glViewport(0, 0, 960, 600);
    shaders.halfShader->useShader();
    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.halfFBO);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
    postProcessingResources.framebufferQuad->render();

    // Ping-pong bloom effect. Performs horiztonal and vertical bluring.
    // First iteration of the bloom effect. This means we don't need if conditions in the for loop
    bool horizontal {false};
    shaders.bloomShader->useShader();
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[!horizontal]);
    glUniform1i(uniformVariables.uniformHorizontal, !horizontal);
    glBindTexture(GL_TEXTURE_2D, postProcessingResources.halfTexture);
    postProcessingResources.framebufferQuad->render();

    int amount {4};
    for (unsigned int i {0}; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[horizontal]);
        glUniform1i(uniformVariables.uniformHorizontal, horizontal);

        // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
        // Move the data between pingPong FBOs
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[!horizontal]);
        postProcessingResources.framebufferQuad->render();
        horizontal = !horizontal;
    }
	glViewport(0, 0, 1920, 1200);

    // ====================================
    // RENDER TO SCREEN
    // ====================================
    
    // The HDR shader uses the texture of the entire scene + the bloom texture to render to the screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaders.hdrShader->useShader();
    postProcessingResources.framebufferQuad->render();
}

int main()
{
    SolarSystemRenderer renderer {};

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

    //// Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix
    glm::mat4 projection {glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 1.0f, 400.0f)};
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

    SceneFunctions::setupSkybox(&skybox, projection);
    createShaders(pointLights, projection);
    setupPostProcessingObjects();
    for (SpaceObject *satellite : satellites)
        satellite->setUniformVariables(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);

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
        camera.keyControl(keys, deltaTime, &renderer);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset(), &timeChange);

        if (renderer.getShadowsEnabled())
        {
            // These needs to be index based loops so that we don't make a copy of the lights each time
            for (size_t i {0}; i < pointLightCount; i++)
                omniShadowMapPass(pointLights[i]);
            omniShadowMapPass(camera.getSpotLight());
        }

        renderPass(camera.calculateViewMatrix());

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
