#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace window
{
    extern GLFWwindow* mainWindow;

	extern GLint windowWidth;
	extern GLint windowHeight;

    // For keyboard input
    extern bool keys[1024];

    int initialize();
    GLfloat getXChange();
    GLfloat getYChange();
    GLfloat getYScrollOffset();
};
