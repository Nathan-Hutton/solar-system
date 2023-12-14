#include "Camera.h"

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
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
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
    if (keys[GLFW_KEY_E])
        roll += 1;
    if (keys[GLFW_KEY_Q])
        roll -= 1;
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    update();
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
}

Camera::~Camera()
{
}
