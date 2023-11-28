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
//std::vector<Sphere*> sphereList;
Planet *planet;
Sun *sun;
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
    sun = new Sun(2.0f, 0.5f, glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-0.2f);
    //sphereList.push_back(sphere2);

    planet = new Planet(1.0f, 1.0f, sun, glm::vec3(0.0f, -10.0f, -2.5f));
    planet->setVelocity(glm::vec3(10.0f, 0.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(1.0f);
    //sphereList.push_back(sphere1);
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

glm::vec3 getForce(Sphere *sphere1, Sphere *sphere2)
{
    /**
     * sphere1 is the sphere being affected by the force vector
    */
    // Vector going from sphere2 to sphere1
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

int main()
{
    mainWindow = Window(1920, 1200);
    mainWindow.initialize();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);

    // All the uniform objects are uniform IDs
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while(!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        // Update rotation angle
        planet->setAngle(planet->getAngle() + planet->getRotationSpeed());
        if (planet->getAngle() >= 360)
            planet->setAngle(planet->getAngle() - 360);
        if (planet->getAngle() <= -360)
            planet->setAngle(planet->getAngle() + 360);

        sun->setAngle(sun->getAngle() + sun->getRotationSpeed());
        if (sun->getAngle() >= 360)
            sun->setAngle(sun->getAngle() - 360);
        if (sun->getAngle() <= -360)
            sun->setAngle(sun->getAngle() + 360);
        
        // Get + Handle user input events
        glfwPollEvents();

        // Update sphere velocity and position
        glm::vec3 sphere1Force = getForce(planet, sun);
        glm::vec3 sphere1Acceleration = getAcceleration(planet->getMass(), sphere1Force);
        glm::vec3 sphere1NewVelocity = getNewVelocity(planet->getVelocity(), sphere1Acceleration, deltaTime);
        glm::vec3 sphere1NewPosition = getNewPosition(planet->getPosition(), sphere1NewVelocity, deltaTime);
        planet->setVelocity(sphere1NewVelocity);
        planet->setPosition(sphere1NewPosition);

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

        // We will only alter the model, not the shader, to do transformation. Model ID is then passed to the uniform variable in the shader
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, planet->getPosition());
        model = glm::rotate(model, planet->getAngle() * toRadians, glm::vec3(1.0f, 0.0f, 2.0f));
        //model = glm::scale(model, glm::vec3(0.4f,0.4f,0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        planet->getMeshPointer()->RenderMesh();

        // set model back to identity
        model = glm::mat4(1.0f);
        model = glm::translate(model, sun->getPosition());
        model = glm::rotate(model, sun->getAngle() * toRadians, glm::vec3(1.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f,0.4f,0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        sun->getMeshPointer()->RenderMesh();
        
        // Apply projection and view
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
