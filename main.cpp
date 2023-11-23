#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Sphere.h"

const float toRadians = M_PI / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Sphere*> sphereList;
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat gravitationalForce = -100.0f;

// vertex shader
static const char* vShader = "shaders/shader.vert";
// Fragment shader
static const char* fShader = "shaders/shader.frag";

void CreateObjects()
{
    Sphere *sphere1 = new Sphere(1.0f, 20, 20, 0.0f, -10.0f, -2.5f);
    sphereList.push_back(sphere1);
    meshList.push_back(sphere1->getMeshPointer());

    Sphere *sphere2 = new Sphere(1.0f, 20, 20, 0.0f, 10.0f, -2.5f);
    sphereList.push_back(sphere2);
    meshList.push_back(sphere2->getMeshPointer());
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

glm::vec3 GetForceVector(Sphere *sphere1, Sphere *sphere2)
{
    /**
     * sphere1 is the sphere being affected by the force vector
    */
    // Vector going from sphere2 to sphere1
    glm::vec3 displacementVector = sphere1->getPositionVector() - sphere2->getPositionVector();
    glm::vec3 directionVector = glm::normalize(displacementVector);
    float displacementVectorLength = glm::length(displacementVector);
    
    if (sphere1->getRadius() + sphere2->getRadius() + 0.27 >= displacementVectorLength)
        return glm::vec3(0.0f,0.0f,0.0f);

    return ((gravitationalForce * sphere1->getMass() * sphere2->getMass()) / (float)pow(displacementVectorLength, 2)) * directionVector;
}

glm::vec3 getAccelerationVector(Sphere *sphere, glm::vec3 forceVector)
{
    return forceVector / sphere->getMass();
}

glm::vec3 getNewPositionVector(Sphere *sphere, glm::vec3 accelerateVector, GLfloat deltaTime)
{
    return sphere->getPositionVector() + accelerateVector * deltaTime;
}

int main()
{
    mainWindow.initialize();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 17.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);

    // All the uniform objects are uniform IDs
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;
    glm::mat4 projection = glm::perspective(45.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    // Loop until window closed
    while(!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;
        
        // Get + Handle user input events
        glfwPollEvents();

        glm::vec3 sphere1ForceVector = GetForceVector(sphereList[0], sphereList[1]);
        glm::vec3 sphere2ForceVector = GetForceVector(sphereList[1], sphereList[0]);
        glm::vec3 sphere1AccelerationVector = getAccelerationVector(sphereList[0], sphere1ForceVector);
        glm::vec3 sphere2AccelerationVector = getAccelerationVector(sphereList[1], sphere2ForceVector);
        glm::vec3 sphere1NewPositionVector = getNewPositionVector(sphereList[0], sphere1AccelerationVector, deltaTime);
        glm::vec3 sphere2NewPositionVector = getNewPositionVector(sphereList[1], sphere2AccelerationVector, deltaTime);

        sphereList[0]->setPositionVector(sphere1NewPositionVector);
        sphereList[1]->setPositionVector(sphere2NewPositionVector);

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
        glm::vec3 myVector = GetForceVector(sphereList[0], sphereList[1]);
        //printf("%f %f %f\n", myVector.x, myVector.y, myVector.z);

        // We will only alter the model, not the shader, to do transformation. Model ID is then passed to the uniform variable in the shader
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sphereList[0]->getPositionVector());
        //model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f,0.4f,0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();

        // set model back to identity
        model = glm::mat4(1.0f);
        model = glm::translate(model, sphereList[1]->getPositionVector());
        //model = glm::scale(model, glm::vec3(0.4f,0.4f,0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();
        
        // Apply projection and view
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
