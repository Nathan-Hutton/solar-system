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
    extern GLfloat xChange;
    extern GLfloat yChange;
    extern GLfloat yScrollOffset;

    int initialize();

    GLfloat getXChange();
    GLfloat getYChange();
    GLfloat getYScrollOffset();
};
