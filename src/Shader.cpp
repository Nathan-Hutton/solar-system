#include "Shader.h"

Shader::Shader()
{
    shaderID = 0;
    uniformProjection = 0;
    uniformModel = 0;

    pointLightCount = 0;

    uniformDirectionalLight.uniformAmbientIntensity = 0;
    uniformDirectionalLight.uniformColor = 0;
    uniformDirectionalLight.uniformDiffuseIntensity = 0;
    uniformDirectionalLight.uniformDirection = 0;

    uniformEyePosition = 0;
    uniformSpecularIntensity = 0;
    uniformShininess = 0;
    
    uniformPointLightCount = 0;
    uniformSpotLightCount = 0;
}

void Shader::createFromString(const char* vertexCode, const char* fragmentCode)
{
    compileShader(vertexCode, fragmentCode);
}

void Shader::createFromFiles(const char* vertexLocation, const char* fragmentLocation)
{
    std::string vertexString = readFile(vertexLocation);
    std::string fragmentString = readFile(fragmentLocation);
    const char* vertexCode = vertexString.c_str();
    const char* fragmentCode = fragmentString.c_str();

    compileShader(vertexCode, fragmentCode);
}

std::string Shader::readFile(const char* fileLocation)
{
    std::string content;
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s File doesn't exist", fileLocation);
        return "";
    }

    std::string line = "";
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n"); // Newline is cosmetic, GLSL is not whitespace sensitive
    }

    fileStream.close();
    return content;
}

void Shader::compileShader(const char* vertexCode, const char* fragmentCode)
{
    // Create a new OpenGL program object. Is the final linked version of multiple
    // shaders combined. Shaders are written in GLSL (OpenGL shading language) and run on the GPU
    // For rendering graphics. Creates container which we attach shaders to.
    shaderID = glCreateProgram();

    if (!shaderID) {
        printf("Error creating shader program\n");
        return;
    }

    // Attach the shaders to the OpenGL program
    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    // Linking all the shaders together into a single program on the GPU.
    // Checks to make sure that the inputs and outputs of the shaders match up.
    // Creates an executable that runs on the programmable parts of the GPU
    glLinkProgram(shaderID);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // Check for errors
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    // Check if the shader program can execute on the current OpenGL state
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

    // Uniform variables let us pass info from the CPU to the GPU.
    // We want to store these IDs now so that we don't have to query
    // for them every frame

    // Camera/Window
    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");

    // Directional light
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
    uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderID, "directionalLight.base.color");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    
    // Specular
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");
    uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");

    // Point lights
    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        char locBuff[100] = {'\0'};

        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].base.color", i);
        uniformPointLights[i].uniformColor = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].base.ambientIntensity", i);
        uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].base.diffuseIntensity", i);
        uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].position", i);
        uniformPointLights[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].exponential", i);
        uniformPointLights[i].uniformExponential = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].linear", i);
        uniformPointLights[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "pointLights[%ld].constant", i);
        uniformPointLights[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);
    }
    
    // Spot lights
    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");
    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++)
    {
        char locBuff[100] = {'\0'};

        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.base.color", i);
        uniformSpotLights[i].uniformColor = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.base.ambientIntensity", i);
        uniformSpotLights[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.base.diffuseIntensity", i);
        uniformSpotLights[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.position", i);
        uniformSpotLights[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.exponential", i);
        uniformSpotLights[i].uniformExponential = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.linear", i);
        uniformSpotLights[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].base.constant", i);
        uniformSpotLights[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].direction", i);
        uniformSpotLights[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);
        snprintf(locBuff, sizeof(locBuff), "spotLights[%ld].edge", i);
        uniformSpotLights[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
    }
}

void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    // Makes a shader object
    GLuint theShader = glCreateShader(shaderType);

    // Sort the code in a way OpenGL can understand
    const GLchar* theCode[1];
    theCode[0] = shaderCode;
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    // Give the source code to the shader 
    glShaderSource(theShader, 1, theCode, codeLength);
    // Compile the shader source code
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    // Check for compilation errors
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    // Attach the now compiled shader to the OpenGL program
    glAttachShader(theProgram, theShader);
}

GLuint Shader::getProjectionLocation()
{
    return uniformProjection;
}
GLuint Shader::getModelLocation()
{
    return uniformModel;
}
GLuint Shader::getViewLocation()
{
    return uniformView;
}
GLuint Shader::getEyePositionLocation()
{
    return uniformEyePosition;
}
GLuint Shader::getSpecularIntensityLocation()
{
    return uniformSpecularIntensity;
}
GLuint Shader::getShininessLocation()
{
    return uniformShininess;
}

void Shader::setDirectionalLight(DirectionalLight *dLight)
{
    dLight->useLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColor, uniformDirectionalLight.uniformDiffuseIntensity,
    uniformDirectionalLight.uniformDirection);
}

void Shader::setPointLights(PointLight* pLights[], unsigned int lightCount)
{
    // Clamp the number of lights allowed
    if (lightCount > MAX_POINT_LIGHTS) lightCount =  MAX_POINT_LIGHTS;

    // Pass the number of lights we're using
    glUniform1i(uniformPointLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++)
    {
        pLights[i]->useLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformDiffuseIntensity,
                            uniformPointLights[i].uniformColor, uniformPointLights[i].uniformPosition, 
                            uniformPointLights[i].uniformExponential, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformConstant);
    }
}

void Shader::setSpotLights(SpotLight* sLights[], unsigned int lightCount)
{
    if (lightCount > MAX_SPOT_LIGHTS) lightCount =  MAX_SPOT_LIGHTS;

    glUniform1i(uniformSpotLightCount, lightCount);

    for (size_t i = 0; i < lightCount; i++)
    {
        sLights[i]->useLight(uniformSpotLights[i].uniformAmbientIntensity, uniformSpotLights[i].uniformDiffuseIntensity,
                            uniformSpotLights[i].uniformColor, uniformSpotLights[i].uniformPosition, uniformSpotLights[i].uniformDirection,
                            uniformSpotLights[i].uniformExponential, uniformSpotLights[i].uniformLinear, uniformSpotLights[i].uniformConstant,
                            uniformSpotLights[i].uniformEdge);
    }
}

void Shader::useShader()
{
    glUseProgram(shaderID);
}

void Shader::clearShader()
{
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel = 0;
    uniformProjection = 0;
}

Shader::~Shader()
{
    clearShader();
}
