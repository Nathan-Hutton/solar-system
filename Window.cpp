#include "Window.h"

Window::Window()
{
    width = 800;
    height = 600;

    for (size_t i = 0; i < 1024; i++)
        keys[i] = 0;

    xChange = 0.0f;
    yChange = 0.0f;
    yScrollOffset = 0.0f;
    mouseFirstMoved = true;
}

Window::Window(GLint windowWidth, GLint windowHeight)
{
    width = windowWidth;
    height = windowHeight;

    for (size_t i = 0; i < 1024; i++)
    {
        keys[i] = 0;
    }
    
    xChange = 0.0f;
    yChange = 0.0f;
    yScrollOffset = 0.0f;
    mouseFirstMoved = true;
}

int Window::initialize()
{
    // Initialize GLFW
    if (!glfwInit()) 
    {
        printf("GLFW initialization failed\n");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = No Backwards Compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow Forward Compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    mainWindow = glfwCreateWindow(width, height, "Elden Ring", NULL, NULL);
    if (!mainWindow)
    {
        printf("GLFW window creation failed\n");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set the context for GLFW to use
    glfwMakeContextCurrent(mainWindow);

    // Handle key and mouse input
    createCallbacks();
    
    // Get the mouse off the screen
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    if(glewInit() != GLEW_OK)
    {
        printf("GLEW initialization failed!\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Gives us a z-buffer so that we don' surfaces that are blocked by other surfaces
    glEnable(GL_DEPTH_TEST);

    // Create viewport
    glViewport(0, 0, bufferWidth, bufferHeight);

    // This makes it so that we can use the static handleKeys method on an instance of a window
    // So the window will be able to point to it's owner basically
    // Let's us use glfwGetWindowPointer and we'll get this instance of our Window back
    glfwSetWindowUserPointer(mainWindow, this);

    return 1;
}

void Window::createCallbacks()
{
    glfwSetKeyCallback(mainWindow, handleKeys);
    glfwSetCursorPosCallback(mainWindow, handleMouse);
    glfwSetScrollCallback(mainWindow, handleScroll);
}

GLfloat Window::getXChange()
{
    GLfloat theChange = xChange;
    xChange = 0.0f;
    return theChange;
}

GLfloat Window::getYChange()
{
    GLfloat theChange = yChange;
    yChange = 0.0f;
    return theChange;
}

GLfloat Window::getYScrollOffset()
{
    GLfloat theOffset = yScrollOffset;
    yScrollOffset = 0.0f;
    return theOffset;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (key < 0 || key >= 1024)
        return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
        theWindow->keys[key] = true;
    else if (action == GLFW_RELEASE)
        theWindow->keys[key] = false;
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    // This if will only pass a single time
    if (theWindow->mouseFirstMoved)
    {
        theWindow->mouseFirstMoved = false;
        theWindow->lastX = xPos;
        theWindow->lastY = yPos;
    }

    // Subtraction is in the opposite order for y to avoid inverted mouse movement
    theWindow->xChange = xPos - theWindow->lastX;
    theWindow->yChange = theWindow->lastY - yPos;

    theWindow->lastX = xPos;
    theWindow->lastY = yPos;
}

void Window::handleScroll(GLFWwindow* window, double xOffset, double yOffset)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    theWindow->yScrollOffset = yOffset;
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}