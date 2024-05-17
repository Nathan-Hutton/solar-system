#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace window
{
    extern GLFWwindow *mainWindow;

    extern GLint width;
    extern GLint height;
    extern GLint bufferWidth;
    extern GLint bufferHeight;

    // For keyboard input
    extern bool keys[1024];

    // For mouse movement
    // Every time we move the mouse, we want to see how much it's changed since it's last location
    extern GLfloat lastX;
    extern GLfloat lastY;
    extern GLfloat xChange;
    extern GLfloat yChange;
    extern GLfloat yScrollOffset;
    // When we start the scene, the mouse will go somewhere and we don't want that to be counted as movement
    extern bool mouseFirstMoved;

    void createCallbacks();
    // This is our callback function
    // Key is number between 0 and 1024, action is the action performed on the key. We'll ignore code and mode for now
    // The callback function needs to have all 4 of these params or it won't work
    void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
    void handleMouse(GLFWwindow* window, double xPos, double yPos);
    void handleScroll(GLFWwindow* window, double xPos, double yPos);
    
    void setDimensions(GLint windowWidth, GLint windowHeight);
    int initialize();

    GLfloat getXChange();
    GLfloat getYChange();
    GLfloat getYScrollOffset();
};
