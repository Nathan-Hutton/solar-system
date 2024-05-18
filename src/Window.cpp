#include "Window.h"

namespace
{
    // When we start the scene, the mouse will go somewhere and we don't want that to be counted as movement
    bool mouseFirstMoved {true};

    // Every time we move the mouse, we want to see how much it's changed since it's last location
    GLfloat lastX {0};
    GLfloat lastY {0};

    // Callback functions needs to have all 4 of these params
    void handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
    {
        if (key < 0 || key >= 1024)
            return;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        if (action == GLFW_PRESS)
            window::keys[key] = true;
        else if (action == GLFW_RELEASE)
            window::keys[key] = false;
    }

    void handleMouse(GLFWwindow* window, double xPos, double yPos)
    {
        // This if will only pass a single time
        if (mouseFirstMoved)
        {
            mouseFirstMoved = false;
            lastX = xPos;
            lastY = yPos;
        }

        // Subtraction is in the opposite order for y to avoid inverted mouse movement
        window::xChange = xPos - lastX;
        window::yChange = lastY - yPos;

        lastX = xPos;
        lastY = yPos;
    }

    void handleScroll(GLFWwindow* window, double xOffset, double yOffset)
    {
        window::yScrollOffset = yOffset;
    }

    void createCallbacks()
    {
        glfwSetKeyCallback(window::mainWindow, handleKeys);
        glfwSetCursorPosCallback(window::mainWindow, handleMouse);
        glfwSetScrollCallback(window::mainWindow, handleScroll);
    }
}

namespace window 
{
    GLFWwindow *mainWindow;
    GLint width = 800;
    GLint height = 600;
    GLint bufferWidth = 0;
    GLint bufferHeight = 0;
    bool keys[1024] = {0};
    GLfloat xChange = 0;
    GLfloat yChange = 0;
    GLfloat yScrollOffset = 0;

    int initialize()
    {
        // Initialize GLFW, which is a window manager
        // Must be called before any other GLFW functions
        // Sets up things that are OS specific and sets up input handling
        if (!glfwInit()) 
        {
            std::cerr << "GLFW initialization failed\n";
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }

        // Setup window properties. This is typically called multiple times before glfwCreateWindow
        // OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Core profile = No Backwards Compatibility
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // Allow Forward Compatibility
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create window and OpengGL context in our GLFW app. Allows for OpenGL input as well
        mainWindow = glfwCreateWindow(width, height, "Solar System", NULL, NULL);
        if (!mainWindow)
        {
            std::cerr << "GLFW window creation failed\n";
            glfwTerminate();
            std::exit(EXIT_FAILURE);
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
            std::cerr << "Glew initialization failed\n";
            glfwDestroyWindow(mainWindow);
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }

        // Gives us a z-buffer so that we don't render surfaces that are blocked by other surfaces
        glEnable(GL_DEPTH_TEST);

        // Make sure that skybox passes depth test if depth is less than or equal to 1.0
        glDepthFunc(GL_LEQUAL);

        // Create viewport. This sets up the portion of the window that OpenGL will draw to
        // Sets up the rectangular area of the window that OpenGL will draw to
        glViewport(0, 0, bufferWidth, bufferHeight);

        return 1;
    }

    GLfloat getXChange()
    {
        const GLfloat theChange {xChange};
        xChange = 0.0f;
        return theChange;
    }

    GLfloat getYChange()
    {
        const GLfloat theChange {yChange};
        yChange = 0.0f;
        return theChange;
    }

    GLfloat getYScrollOffset()
    {
        const GLfloat theOffset {yScrollOffset};
        yScrollOffset = 0.0f;
        return theOffset;
    }
}
