#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SpotLight.h"

namespace camera {
    extern glm::vec3 position;
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
