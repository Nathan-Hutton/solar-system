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
        keys[i] = 0;
    
    xChange = 0.0f;
    yChange = 0.0f;
    yScrollOffset = 0.0f;
    mouseFirstMoved = true;
}

int Window::initialize()
{
    // Initialize GLFW, which is a window manager
    // Must be called before any other GLFW functions
    // Sets up things that are OS specific and sets up input handling
    if (!glfwInit()) 
    {
        printf("GLFW initialization failed\n");
        glfwTerminate();
        return 1;
    }

    // Setup window properties. This is typically called multiple times before glfwCreateWindow
    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Core profile = No Backwards Compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Allow Forward Compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create window and OpengGL context in our GLFW app. Allows for OpenGL input as well
    mainWindow = glfwCreateWindow(width, height, "Solar System", NULL, NULL);
    if (!mainWindow)
    {
        printf("GLFW window creation failed\n");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set the OpenGL context to be current. Directs OpenGL commands to this context.
    // An OpenGL context is a complete set of OpenGL state variables. For OpenGL commands to work,
    // a context must be current, so it needs to be attached to a thread because OpenGL is a state machine.
    // We attach the context of the window to the current OpenGL thread. Then all OpenGL calls
    // made by that thread will be directed to this context.
    glfwMakeContextCurrent(mainWindow);

    // Handle key and mouse input
    createCallbacks();
    
    // Get the mouse off the screen
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Query and load all OpenGL extensions allowed by your drivers
    // Allows us to access features/extensions not in the core OpenGL specification
    if(glewInit() != GLEW_OK)
    {
        printf("GLEW initialization failed!\n");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    // Gives us a z-buffer so that we don't render surfaces that are blocked by other surfaces
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // No idea why this is CW and not CCW
    glFrontFace(GL_CW);

    glfwSwapInterval(1);

    // Make sure that skybox passes depth test if depth is less than or equal to 1.0
    glDepthFunc(GL_LEQUAL);

    // Create viewport. This sets up the portion of the window that OpenGL will draw to
    // Sets up the rectangular area of the window that OpenGL will draw to
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

void Window::setWindowTitle(std::string newTitle)
{
    glfwSetWindowTitle(mainWindow, newTitle.c_str());
}

Window::~Window()
{
    glfwDestroyWindow(mainWindow);
    glfwTerminate();
}
