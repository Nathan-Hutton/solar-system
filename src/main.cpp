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

const float toRadians = M_PI / 180.0f;

GLuint uniformModelPlanets = 0, uniformProjectionPlanets = 0, uniformViewPlanets = 0,
uniformEyePositionPlanets = 0, uniformSpecularIntensityPlanets = 0, uniformShininessPlanets = 0,
uniformOmniLightPos = 0, uniformFarPlane = 0;
GLuint uniformModelSuns = 0, uniformProjectionSuns = 0, uniformViewSuns = 0;
GLuint uniformModelDirectionalShadowMap = 0;
GLuint uniformModelOmniShadowMap = 0;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<Planet*> planets;
std::vector<Model*> complexModels;
std::vector<Shader*> shaderList;
Shader* directionalShadowShader;
Shader* omniShadowShader;

Camera camera;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

//DirectionalLight mainLight;
PointLight* pointLights[MAX_POINT_LIGHTS];
SpotLight* spotLights[MAX_SPOT_LIGHTS];

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
GLfloat timeChange = 1.0f;

GLfloat gravitationalForce = -100.0f;

static const char* vShader = "../assets/shaders/shader.vert";
static const char* fShader = "../assets/shaders/planetShader.frag";
static const char* vShader2 = "../assets/shaders/sunShader.vert";
static const char* fShader2 = "../assets/shaders/sunShader.frag";

void createShaders()
{
    // Shader for the planets and moons
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(shader1);

    // Shader for the suns (no lighting or shadows)
    Shader *shader2 = new Shader();
    shader2->createFromFiles(vShader2, fShader2);
    shaderList.push_back(shader2);

    // Shader for the render pass that will create the shadow map
    directionalShadowShader = new Shader();
	directionalShadowShader->createFromFiles("../assets/shaders/directional_shadow_map.vert", "../assets/shaders/directional_shadow_map.frag");

    omniShadowShader = new Shader();
	omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
		"../assets/shaders/omni_shadow_map.geom",
	  	"../assets/shaders/omni_shadow_map.frag");
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

void directionalShadowMapPass(DirectionalLight* light)
{
	directionalShadowShader->useShader();

	// Make the viewport the same dimenstions as the FBO
	glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    // Bind our framebuffer so that the shader output goes to it
    // Every draw call after this will go to that framebuffer
	light->getShadowMap()->write();
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 lightTransform = light->calculateLightTransform();
	directionalShadowShader->setDirectionalLightTransform(&lightTransform);
	
    directionalShadowShader->validate();

	// Draw just to the depth buffer
	renderPlanets(uniformModelDirectionalShadowMap);

    // Bind the default framebuffer
    // If we called swapbuffers without doing this the image wouldn't change
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void renderPass(glm::mat4 projection, glm::mat4 view)
{
    // ====================================
    // RENDER PLANETS, MOONS, and ASTEROIDS
    // ====================================

    // Set viewport. Clear window, color, and depth buffer bit. Make the image black
	glViewport(0, 0, 1920, 1200);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.drawSkybox(view, projection);

	shaderList[0]->useShader();

    // Apply projection and view matrices.
    // Projection defines how the 3D world is projected onto a 2D screen. We're using a perspective matrix.
    // View matrix represents the camera's position and orientation in world.
    // The world is actually rotated around the camera with the view matrix. The camera is stationary.
    glUniformMatrix4fv(uniformProjectionPlanets, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3f(uniformEyePositionPlanets, camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

	//shaderList[0]->setDirectionalLight(&mainLight);
	shaderList[0]->setPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0]->setSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);

	// We need to be able to see whatever fragment we're trying to render from the perspective of the light
	//glm::mat4 lightTransform = mainLight.calculateLightTransform();
	//shaderList[0]->setDirectionalLightTransform(&lightTransform);

 	// Handle/bind the shadow map texture to texture unit 1
	//mainLight.getShadowMap()->read(GL_TEXTURE2);
	shaderList[0]->setTexture(1);
    shaderList[0]->setDirectionalShadowMap(2);

	// Handle camera flashlight
	glm::vec3 lowerLight = camera.getPosition();
	lowerLight.y -= 0.3f;
	//spotLights[0]->setFlash(lowerLight, camera.getDirection());

    shaderList[0]->validate();

 	// Now we're not drawing just to the depth buffer but also the color buffer
	renderPlanets(uniformModelPlanets);

    // ====================================
    // RENDER SUNS
    // ====================================

    shaderList[1]->useShader();
	shaderList[1]->setTexture(1);
    glUniformMatrix4fv(uniformProjectionSuns, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformViewSuns, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

    shaderList[1]->validate();

    renderSuns();
}

int main()
{
    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    //SceneFunctions::createObjects1Sun1Planet(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    //SceneFunctions::createObjectsDefault(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    SceneFunctions::createObjectsFigureEight(stars, planets, complexModels, pointLights, &pointLightCount, spotLights, &spotLightCount, &camera);
    
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
    //mainLight = DirectionalLight(2048, 2048,
    //                            1.0f, 1.0f, 1.0f, 
    //                            0.0f, 1.0f,
    //                            0.0f, -15.0f, -1.0f);

    // Uniform object IDs let us pass values from the CPU to the GPU.
    // We use things like glUniform1f (for one value) to set these values
    // on the GPU
    uniformModelPlanets = shaderList[0]->getModelLocation();
    uniformProjectionPlanets = shaderList[0]->getProjectionLocation();
    uniformViewPlanets = shaderList[0]->getViewLocation();
    uniformEyePositionPlanets = shaderList[0]->getEyePositionLocation();
    uniformSpecularIntensityPlanets = shaderList[0]->getSpecularIntensityLocation();
    uniformShininessPlanets = shaderList[0]->getShininessLocation();

    // For the sun shaders we don't do any light or shadow calculations
    uniformModelSuns = shaderList[1]->getModelLocation();
    uniformProjectionSuns = shaderList[1]->getProjectionLocation();
    uniformViewSuns = shaderList[1]->getViewLocation();

    // The shadow map shader will record the depth values of all objects except suns
    uniformModelDirectionalShadowMap = directionalShadowShader->getModelLocation();
    uniformModelOmniShadowMap = omniShadowShader->getModelLocation();
	uniformOmniLightPos = omniShadowShader->getOmniLightPosLocation();
	uniformFarPlane = omniShadowShader->getFarPlaneLocation();

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

        // Get + handle user input
        glfwPollEvents();
        camera.keyControl(mainWindow.getKeys(), deltaTime, &spotLightCount);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        handleTimeChange(mainWindow.getYScrollOffset());

		// Render the scene to a framebuffer (only depth values) which will save it to a 
        // texture which we'll use in the main render pass
        //directionalShadowMapPass(&mainLight);

		// These needs to be index based loops so that we don't make a copy of the lights each time
		for (size_t i = 0; i < pointLightCount; i++)
			omniShadowMapPass(pointLights[i]);
		for (size_t i = 0; i < spotLightCount; i++)
			omniShadowMapPass(spotLights[i]);

        renderPass(projection, camera.calculateViewMatrix());

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
