#include "Shader.h"

Shader::Shader() {}

void Shader::createFromFiles(const char* file1, const char* file2, const char* file3)
{
    if (file3 != nullptr)
    {
        std::string vertexString{readFile(file1)};
        std::string geometryString{readFile(file2)};
        std::string fragmentString{readFile(file3)};
        const char* vertexCode{vertexString.c_str()};
        const char* geometryCode{geometryString.c_str()};
        const char* fragmentCode{fragmentString.c_str()};
        compileShader(vertexCode, geometryCode, fragmentCode);
        return;
    }
    std::string vertexString{readFile(file1)};
    std::string fragmentString{readFile(file2)};
    const char* vertexCode{vertexString.c_str()};
    const char* fragmentCode{fragmentString.c_str()};
    compileShader(vertexCode, fragmentCode);
}

std::string Shader::readFile(const char* fileLocation)
{
    std::string content {};
    std::ifstream fileStream(fileLocation, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Failed to read" << fileLocation << " File doesn't exist.\n";
        std::exit(EXIT_FAILURE);
    }

    std::string line {""};
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + '\n'); // Newline is cosmetic, GLSL is not whitespace sensitive
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
        std::cerr << "Error creating shader program\n";
        std::exit(EXIT_FAILURE);
    }

    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    compileProgram();
}

void Shader::compileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode)
{
    // Create a new OpenGL program object. Is the final linked version of multiple
    // shaders combined. Shaders are written in GLSL (OpenGL shading language) and run on the GPU
    // For rendering graphics. Creates container which we attach shaders to.
    shaderID = glCreateProgram();

    if (!shaderID) {
        std::cerr << "Error creating shader program\n";
        std::exit(EXIT_FAILURE);
    }

    addShader(shaderID, vertexCode, GL_VERTEX_SHADER);
    addShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
    addShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

    compileProgram();
}

void Shader::validate()
{
    GLint result {0};
    GLchar eLog[1024] {0};

    // Check if the shader program can execute on the current OpenGL state
    glValidateProgram(shaderID);
    glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cerr << "Error validating program: " << eLog << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void Shader::compileProgram()
{
    // Linking all the shaders together into a single program on the GPU.
    // Checks to make sure that the inputs and outputs of the shaders match up.
    // Creates an executable that runs on the programmable parts of the GPU
    glLinkProgram(shaderID);

    GLint result {0};
    GLchar eLog[1024] {0};

    // Check for errors
    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
        std::cerr << "Error linking program: " << eLog << '\n';
        std::exit(EXIT_FAILURE);
    }

    // Uniform variables let us pass info from the CPU to the GPU.
    // We want to store these IDs now so that we don't have to query
    // for them every frame

    // Camera/Window
    uniformModel                = glGetUniformLocation(shaderID, "model");
    uniformProjection           = glGetUniformLocation(shaderID, "projection");
    uniformView                 = glGetUniformLocation(shaderID, "view");

    // Specular
    uniformEyePosition          = glGetUniformLocation(shaderID, "eyePosition");
    uniformSpecularIntensity    = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess            = glGetUniformLocation(shaderID, "material.shininess");

    // Point lights
    std::stringstream ss {};
    for (size_t i {0}; i < commonVals::MAX_POINT_LIGHTS; i++)
    {
        // Base color
        ss << "pointLights[" << i << "].base.color";
        uniformPointLights[i].uniformColor = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Ambient intensity
        ss << "pointLights[" << i << "].base.ambientIntensity";
        uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Diffuse intensity
        ss << "pointLights[" << i << "].base.diffuseIntensity";
        uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Position
        ss << "pointLights[" << i << "].position";
        uniformPointLights[i].uniformPosition = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Exponential
        ss << "pointLights[" << i << "].exponential";
        uniformPointLights[i].uniformExponential = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Linear
        ss << "pointLights[" << i << "].linear";
        uniformPointLights[i].uniformLinear = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Constant
        ss << "pointLights[" << i << "].constant";
        uniformPointLights[i].uniformConstant = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }
    
    // Spot lights
    uniformFlashLightOn                         = glGetUniformLocation(shaderID, "flashLightOn");
    uniformSpotLight.uniformColor               = glGetUniformLocation(shaderID, "spotLight.base.base.color");
    uniformSpotLight.uniformAmbientIntensity    = glGetUniformLocation(shaderID, "spotLight.base.base.ambientIntensity");
    uniformSpotLight.uniformDiffuseIntensity    = glGetUniformLocation(shaderID, "spotLight.base.base.diffuseIntensity");

    uniformSpotLight.uniformPosition            = glGetUniformLocation(shaderID, "spotLight.base.position");
    uniformSpotLight.uniformExponential         = glGetUniformLocation(shaderID, "spotLight.base.exponential");
    uniformSpotLight.uniformLinear              = glGetUniformLocation(shaderID, "spotLight.base.linear");
    uniformSpotLight.uniformConstant            = glGetUniformLocation(shaderID, "spotLight.base.constant");

    uniformSpotLight.uniformDirection           = glGetUniformLocation(shaderID, "spotLight.direction");
    uniformSpotLight.uniformEdge                = glGetUniformLocation(shaderID, "spotLight.edge");

    uniformTexture = glGetUniformLocation(shaderID, "theTexture");

    // Point light shadow maps. Values in the omni_shadow_map shaders
    uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
    uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");

    // Light matrices
    for (size_t i {0}; i < 6; i++)
    {
        ss << "lightMatrices[" << i << "]";
        uniformLightMatrices[i] = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }

    for (size_t i {0}; i < commonVals::MAX_POINT_LIGHTS + 1; i++)
    {
        // Shadowmap
        ss << "omniShadowMaps[" << i << "].shadowMap";
        uniformOmniShadowMaps[i].shadowMap = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
                    
        // Far plane
        ss << "omniShadowMaps[" << i << "].farPlane";
        uniformOmniShadowMaps[i].farPlane = glGetUniformLocation(shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }
}

void Shader::addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
    // Makes a shader object
    GLuint theShader {glCreateShader(shaderType)};

    // Sort the code in a way OpenGL can understand
    const GLchar* theCode[1];
    theCode[0] = shaderCode;
    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    // Give the source code to the shader 
    glShaderSource(theShader, 1, theCode, codeLength);
    // Compile the shader source code
    glCompileShader(theShader);

    GLint result {0};
    GLchar eLog[1024] { 0 };

    // Check for compilation errors
    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        std::cerr << "Error compiling the " << shaderType << " shader: " << eLog << '\n';
        std::exit(EXIT_FAILURE);
    }

    // Attach the now compiled shader to the OpenGL program
    glAttachShader(theProgram, theShader);
}

GLuint Shader::getShaderID()
{
    return shaderID;
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
GLuint Shader::getOmniLightPosLocation()
{
    return uniformOmniLightPos;
}
GLuint Shader::getFarPlaneLocation()
{
    return uniformFarPlane;
}

void Shader::setPointLightsWithoutShadows(PointLight* pLights[], unsigned int lightCount)
{
    // Clamp the number of lights allowed
    if (lightCount > commonVals::MAX_POINT_LIGHTS) lightCount =  commonVals::MAX_POINT_LIGHTS;

    for (size_t i {0}; i < lightCount; i++)
    {
        pLights[i]->useLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformDiffuseIntensity,
                            uniformPointLights[i].uniformColor, uniformPointLights[i].uniformPosition, 
                            uniformPointLights[i].uniformExponential, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformConstant);
    }
}

void Shader::setPointLights(PointLight* pLights[], unsigned int lightCount, unsigned int textureUnit, unsigned int offset)
{
    // Clamp the number of lights allowed
    if (lightCount > commonVals::MAX_POINT_LIGHTS) lightCount =  commonVals::MAX_POINT_LIGHTS;

    for (size_t i {0}; i < lightCount; i++)
    {
        pLights[i]->useLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformDiffuseIntensity,
                            uniformPointLights[i].uniformColor, uniformPointLights[i].uniformPosition, 
                            uniformPointLights[i].uniformExponential, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformConstant);

        // We need the GL_TEXTURE0 since it needs to be an enum type
        pLights[i]->getShadowMap()->read(GL_TEXTURE0 + textureUnit + i);
        // Offset for point lights will usually just be 0.
        // The offset is to take into account that the shadowmaps are all 1 array in the shader
        glUniform1i(uniformOmniShadowMaps[i + offset].shadowMap, textureUnit + i);
        glUniform1f(uniformOmniShadowMaps[i + offset].farPlane, pLights[i]->getFarPlane());
    }
}

// We'll only call this once since nothing but the position and shadow map will change
void Shader::setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset)
{
    glUniform1i(uniformFlashLightOn, sLight->isOn());
    sLight->useLight(uniformSpotLight.uniformAmbientIntensity, uniformSpotLight.uniformDiffuseIntensity,
                        uniformSpotLight.uniformColor, uniformSpotLight.uniformPosition, uniformSpotLight.uniformDirection,
                        uniformSpotLight.uniformExponential, uniformSpotLight.uniformLinear, uniformSpotLight.uniformConstant,
                        uniformSpotLight.uniformEdge);

    if (!shadowsEnabled)
        return;

    // We need the GL_TEXTURE0 since it needs to be an enum type
    sLight->getShadowMap()->read(GL_TEXTURE0 + textureUnit);
    // The offset is to take into account that the shadowmaps are all 1 array in the shader
    glUniform1i(uniformOmniShadowMaps[offset].shadowMap, textureUnit);
    glUniform1f(uniformOmniShadowMaps[offset].farPlane, sLight->getFarPlane());
}

// Called every frame
void Shader::setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset)
{
    glUniform1i(uniformFlashLightOn, sLight->isOn());

    // No need to set all of these values if the spotlight isn't even on
    if (!sLight->isOn())
        return;

    sLight->setPosAndDir(uniformSpotLight.uniformPosition, uniformSpotLight.uniformDirection);

    if (!shadowsEnabled)
        return;

    // The offset is to take into account that the shadowmaps are all 1 array in the shader
    glUniform1i(uniformOmniShadowMaps[offset].shadowMap, textureUnit);
}

void Shader::setTexture(GLuint textureUnit)
{
    glUniform1i(uniformTexture, textureUnit);
}

void Shader::setLightMatrices(std::vector<glm::mat4> lightMatrices)
{
    for (size_t i {0}; i < 6; i++)
        glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
}

void Shader::useShader()
{
    // Set the active shader program for subsequent rendering operations
    glUseProgram(shaderID);
}

void Shader::clearShader()
{
    if (shaderID != 0)
    {
        glDeleteProgram(shaderID);
        shaderID = 0;
    }

    uniformModel        = 0;
    uniformProjection   = 0;
}

Shader::~Shader()
{
    clearShader();
}
