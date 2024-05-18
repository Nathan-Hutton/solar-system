#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include "SpotLight.h"

namespace camera {
    extern glm::vec3 position;
    extern glm::vec3 front;
    extern glm::vec3 up;
    extern glm::vec3 right;
    extern glm::vec3 worldUp;

    extern GLfloat yaw;
    extern GLfloat pitch;
    extern GLfloat roll;

    extern GLfloat moveSpeed;
    extern GLfloat turnSpeed;

    extern SpotLight *spotLight;

    void keyControl(bool* keys, GLfloat deltaTime);
    void mouseControl(GLfloat xChange, GLfloat yChange);
    void setSpotLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat xDir, GLfloat yDir, GLfloat zDir,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge);

    glm::mat4 calculateViewMatrix();
};
