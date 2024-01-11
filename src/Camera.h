#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include "SpotLight.h"

class Camera 
{
public:
    Camera();
    Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

    void keyControl(bool* keys, GLfloat deltaTime, unsigned int *sLightCount);
    void mouseControl(GLfloat xChange, GLfloat yChange);
    void setSpotLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    GLfloat xPos, GLfloat yPos, GLfloat zPos,
                    GLfloat xDir, GLfloat yDir, GLfloat zDir,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge);
    SpotLight* getSpotLight();

    glm::vec3 getPosition();
    glm::vec3 getDirection();
    glm::mat4 calculateViewMatrix();
    void update();

    ~Camera();

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat oldYaw;
    GLfloat pitch;
    GLfloat oldPitch;
    GLfloat oldRoll;
    GLfloat roll;

    GLfloat moveSpeed;
    GLfloat turnSpeed;

    bool spotLightOn;

    SpotLight *spotLight;
};
