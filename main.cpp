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
#include "Planet.h"
#include "Moon.h"

const float toRadians = M_PI / 180.0f;

Window mainWindow;
std::vector<Sun*> stars;
std::vector<Sphere*> satellites;
Sun *sun;
Planet *planet;
Planet *planet1;
Moon *moon;
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat gravitationalForce = -100.0f;

float sphere1CurAngle = 0.0f;
float sphere2CurAngle = 90.0f;

// vertex shader
static const char* vShader = "shaders/shader.vert";
// Fragment shader
static const char* fShader = "shaders/shader.frag";

void CreateObjects()
{
    sun = new Sun(2.0f, 5.0f, glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-0.2f);
    stars.push_back(sun);

    planet = new Planet(1.0f, 2.0f, sun, glm::vec3(-15.0f, -20.0f, -2.5f));
    planet->setVelocity(glm::vec3(-30.0f, 6.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(1.0f);
    satellites.push_back(planet);

    planet1 = new Planet(1.0f, 2.0f, sun, glm::vec3(7.5f, 10.0f, -2.5f));
    planet1->setVelocity(glm::vec3(20.0f, -12.0f, 0.0f));
    planet1->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet1->setRotationSpeed(1.0f);
    satellites.push_back(planet1);

    moon = new Moon(0.5f, 1.0f, planet, glm::vec3(-15.0f, -15.0f, -2.5f));
    moon->setVelocity(glm::vec3(-30.0f, 2.0f, 1.0f));
    moon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    moon->setRotationSpeed(1.0f);
    satellites.push_back(moon);
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

// Get the force vector pointing to sphere2 from sphere1
glm::vec3 getForce(Sphere *sphere1, Sphere *sphere2)
{
    glm::vec3 displacementVector = sphere1->getPosition() - sphere2->getPosition();
    glm::vec3 directionVector = glm::normalize(displacementVector);
    float displacementVectorLength = glm::length(displacementVector);
    
    if (sphere1->getRadius() + sphere2->getRadius() + 0.27 >= displacementVectorLength)
        return glm::vec3(0.0f,0.0f,0.0f);

    return ((gravitationalForce * sphere1->getMass() * sphere2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
}

glm::vec3 getAcceleration(GLfloat mass, glm::vec3 force)
{
    return force / mass;
}

glm::vec3 getNewVelocity(glm::vec3 oldVelocity, glm::vec3 acceleration, GLfloat deltaTime)
{
    return oldVelocity + acceleration * deltaTime;
}

glm::vec3 getNewPosition(glm::vec3 oldPosition, glm::vec3 velocity, GLfloat deltaTime)
{
    return oldPosition + velocity * deltaTime;
}

// Update the positions of all moons and planets
void updateSatellitePositions()
{
    std::vector<glm::vec3> newPositions;
    for (int i = 0; i < satellites.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from all other objects
        for (Sun *star : stars)
            force += getForce(satellites[i], star);
        for (int j = 0; j < satellites.size(); j++) 
        {
            if (i == j) 
                continue;
            force += getForce(satellites[i], satellites[j]);
        }

        glm::vec3 acceleration = getAcceleration(satellites[i]->getMass(), force);
        glm::vec3 newVelocity = getNewVelocity(satellites[i]->getVelocity(), acceleration, deltaTime);
        glm::vec3 newPosition = getNewPosition(satellites[i]->getPosition(), newVelocity, deltaTime);
        satellites[i]->setVelocity(newVelocity);
        newPositions.push_back(newPosition);
    }

    // Update positions at the end of the loop so that no objects move before we get all of our data
    for (int i = 0; i < satellites.size(); i++)
    {
        satellites[i]->setPosition(newPositions[i]);
    }
}

void updateCelestialBodyAngles()
{
    for (Sphere *sphere : satellites) 
    {
        sphere->setAngle(sphere->getAngle() + sphere->getRotationSpeed());
        if (sphere->getAngle() >= 360)
            sphere->setAngle(sphere->getAngle() - 360);
        if (sphere->getAngle() <= -360)
            sphere->setAngle(sphere->getAngle() + 360);
    }
    for (Sphere *star : stars) 
    {
        star->setAngle(star->getAngle() + star->getRotationSpeed());
        if (star->getAngle() >= 360)
            star->setAngle(star->getAngle() - 360);
        if (star->getAngle() <= -360)
            star->setAngle(star->getAngle() + 360);
    }
}

void renderObjects(GLuint uniformModel)
{
    glm::mat4 model;
    for (Sun *star : stars)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, star->getPosition());
        model = glm::rotate(model, star->getAngle() * toRadians, star->getRotation());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        star->getMeshPointer()->RenderMesh();
    }
    for (Sphere *satellite : satellites)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, satellite->getPosition());
        model = glm::rotate(model, satellite->getAngle() * toRadians, satellite->getRotation());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        satellite->getMeshPointer()->RenderMesh();
    }
}

int main()
{
    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);

    // All the uniform objects are uniform IDs
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 200.0f);

    // Loop until window closed
    while(!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        updateCelestialBodyAngles();
        updateSatellitePositions();

        // Get + Handle user input events
        glfwPollEvents();

        // Move the camera based on input
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear window and color and depth buffer bit
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectionLocation();
        uniformView = shaderList[0].getViewLocation();
        renderObjects(uniformModel);

        // Apply projection and view
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
