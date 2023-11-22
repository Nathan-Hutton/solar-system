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

const float toRadians = acos(-1) / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// vertex shader
static const char* vShader = "shaders/shader.vert";
// Fragment shader
static const char* fShader = "shaders/shader.frag";

void CreateObjects()
{
    // indexed draws lets us number the vertices then refer to them so we can reuse them
    unsigned int indices[] = {
        0,3,1,
        1,3,2,
        2,3,0,
        0,1,2
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // Create pyramids
    Mesh *obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj1);

    Mesh *obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 12, 12);
    meshList.push_back(obj2);

    // Create sphere
    std::vector<GLfloat> sphereVertices;
    std::vector<GLuint> sphereIndices;
    Sphere *sphere = new Sphere(sphereVertices, sphereIndices, 1.0f, 20, 20);
    sphere->getMeshPointer()->CreateMesh(sphereVertices.data(), sphereIndices.data(), sphereVertices.size(), sphereIndices.size());
    meshList.push_back(sphere->getMeshPointer());
}

void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->createFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main()
{
    mainWindow.initialize();

    CreateObjects();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.3f);

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

        // Move the camera based on input
        camera.keyControl(mainWindow.getKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // clear the color and depth buffer bit (z-buffer)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].useShader();
        uniformModel = shaderList[0].getModelLocation();
        uniformProjection = shaderList[0].getProjectionLocation();
        uniformView = shaderList[0].getViewLocation();

        glm::mat4 model = glm::mat4(1.0f);
        // We will only alter the model, not the shader, to do transformation. Model ID is then passed to the uniform variable in the shader
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        //model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.4f,0.4f,1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();

        // set model back to identity
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f,0.4f,1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[1]->RenderMesh();

        // set model back to identity
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
        model = glm::scale(model, glm::vec3(0.4f,0.4f,0.4f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        meshList[2]->RenderMesh();

        // Apply projection and view
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
