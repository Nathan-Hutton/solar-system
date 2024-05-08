#pragma once

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
    public:
        Window();
        Window(GLint windowWidth, GLint windowHeight);

        int initialize();

        GLfloat getBufferWidth() { return bufferWidth; }
        GLfloat getBufferHeight() { return bufferHeight; }

        bool getShouldClose() { return glfwWindowShouldClose(mainWindow);}

        bool* getKeys() { return keys; }
        GLfloat getXChange();
        GLfloat getYChange();
        GLfloat getYScrollOffset();

        void setWindowTitle(std::string newTitle);
        
        void swapBuffers() { glfwSwapBuffers(mainWindow); }

        ~Window();

    private:
        GLFWwindow *mainWindow;

        GLint width {}, height {};
        GLint bufferWidth {}, bufferHeight {};

        // For keyboard input
        bool keys[1024] {};

        // For mouse movement
        // Every time we move the mouse, we want to see how much it's changed since it's last location
        GLfloat lastX {};
        GLfloat lastY {};
        GLfloat xChange {};
        GLfloat yChange {};
        GLfloat yScrollOffset {};
        // When we start the scene, the mouse will go somewhere and we don't want that to be counted as movement
        bool mouseFirstMoved {};

        void createCallbacks();
        // This is our callback function
        // Key is number between 0 and 1024, action is the action performed on the key. We'll ignore code and mode for now
        // The callback function needs to have all 4 of these params or it won't work
        static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
        static void handleMouse(GLFWwindow* window, double xPos, double yPos);
        static void handleScroll(GLFWwindow* window, double xPos, double yPos);
};
