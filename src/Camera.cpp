#include "Camera.h"
#include <stdio.h>

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = 90.0f;
    oldYaw = 90.0f;
    pitch = 0.0f;
    oldPitch = 0.0f;
    roll = 0.0f;
    oldRoll = 0.0f;
    
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

    moveSpeed = 5.0f;
    turnSpeed = 1.0f;

    spotLightOn = true;

    spotLight = NULL;
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
    position = startPosition;
    worldUp = startUp;
    yaw = startYaw;
    oldYaw = startYaw;
    pitch = startPitch;
    oldPitch = startPitch;
    oldRoll = 0.0f;
    roll = 0.0f;
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

    moveSpeed = startMoveSpeed;
    turnSpeed = startTurnSpeed;

    spotLightOn = false;

    spotLight = NULL;
}

void Camera::keyControl(bool* keys, GLfloat deltaTime, unsigned int *sLightCount)
{
    GLfloat velocity = moveSpeed * deltaTime;

    if (keys[GLFW_KEY_W])
        position += front * velocity;
    if (keys[GLFW_KEY_A])
        position -= right * velocity;
    if (keys[GLFW_KEY_S])
        position -= front * velocity;
    if (keys[GLFW_KEY_D])
        position += right * velocity;

    // Up and down
    if (keys[GLFW_KEY_LEFT_SHIFT])
        position -= up * velocity;
    if (keys[GLFW_KEY_SPACE])
        position += up * velocity;

    // Roll
    if (keys[GLFW_KEY_E])
        roll += velocity * 15;
    if (keys[GLFW_KEY_Q])
        roll -= velocity * 15;

    // Check for flashlight toggle
    if (!keys[GLFW_KEY_F])
        return;

    // Setting this to false means we won't trigger it multiple times when we press it once
    keys[GLFW_KEY_F] = false;

    // If flashlight is disabled, don't put it in the shader (it's the last spotLight in our array)
    if (spotLightOn)
        (*sLightCount)--;
    else
        (*sLightCount)++;

    spotLightOn = !spotLightOn;
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    update();
}

void Camera::setSpotLight(GLfloat red, GLfloat green, GLfloat blue, 
                GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                GLfloat xPos, GLfloat yPos, GLfloat zPos,
                GLfloat xDir, GLfloat yDir, GLfloat zDir,
                GLfloat exponential, GLfloat linear, GLfloat constant,
                GLfloat edge)
{
    spotLight = new SpotLight(red, green, blue,
                        ambientIntensity, diffuseIntensity,
                        xPos, yPos, zPos,
                        xDir, yDir, zDir,
                        exponential, linear, constant,
                        edge);
}

SpotLight* Camera::getSpotLight()
{
    return spotLight;
}

glm::mat4 Camera::calculateViewMatrix()
{
    // Args are where the camera is, what the camera is looking at, and what its up is
    return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getPosition()
{
    return position;
}

glm::vec3 Camera::getDirection()
{
    return front;
}

void Camera::update()
{
    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(oldYaw - yaw), up);
    front = glm::normalize(glm::vec3(yawMatrix * glm::vec4(front, 0.0f)));
    oldYaw = yaw;
    right = glm::normalize(glm::cross(front, up));

    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(pitch - oldPitch), right);
    front = glm::normalize(glm::vec3(pitchMatrix * glm::vec4(front, 0.0f)));
    oldPitch = pitch;
    up = glm::normalize(glm::cross(right, front));

    glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll - oldRoll), front);
    up = glm::normalize(glm::vec3(rollMatrix * glm::vec4(up, 0.0f)));
    oldRoll = roll;
    right = glm::normalize(glm::cross(front, up));

    // Update the flashlight position and direction
    spotLight->setFlash(position, front);
}

Camera::~Camera()
{
}
