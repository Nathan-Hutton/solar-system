#include "Shader.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>

namespace
{
    std::string getShaderTypeString(GLenum shaderType)
    {
        switch (shaderType)
        {
            case (GL_VERTEX_SHADER):   return "GL_VERTEX_SHADER";
            case (GL_FRAGMENT_SHADER): return "GL_FRAGMENT_SHADER";
            case (GL_GEOMETRY_SHADER): return "GL_GEOMETRY_SHADER";
        }
        return "";
    }

    std::string readFile(std::string_view fileLocation)
    {
        std::ifstream fileStream{std::filesystem::path{fileLocation}};

        if (!fileStream.is_open())
            throw std::runtime_error("Failed to read " + std::string{fileLocation} + " File doesn't exist");

        std::string line {};
        std::string fileContents {};
        while (getline(fileStream, line))
            fileContents += line + '\n';

        return fileContents;
    }

    void addShader(GLuint theProgram, const std::string& shaderCode, GLenum shaderType)
    {
        // Makes a shader object
        GLuint theShader {glCreateShader(shaderType)};

        // Sort the code in a way OpenGL can understand
        const GLchar* theCode[1];
        theCode[0] = shaderCode.c_str();
        GLint codeLength[1];
        codeLength[0] = std::ssize(shaderCode);

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
            throw std::runtime_error("Error compiling the " + getShaderTypeString(shaderType) + " shader: " + std::string{eLog});
        }

        // Attach the now compiled shader to the OpenGL program
        glAttachShader(theProgram, theShader);
    }
}

void Shader::createFromFiles(std::string_view file1, std::string_view file2, std::string_view file3)
{
    compileShader(readFile(file1), readFile(file2), (file3 == "") ? "" : readFile(file3));
}

void Shader::compileShader(const std::string& shader1Code, const std::string& shader2Code, const std::string& shader3Code)
{
    // Create a new OpenGL program object. Is the final linked version of multiple
    // shaders combined. Shaders are written in GLSL (OpenGL shading language) and run on the GPU
    // For rendering graphics. Creates container which we attach shaders to.
    m_shaderID = glCreateProgram();

    if (!m_shaderID)
        throw std::runtime_error("Error creating shader program");

    // We'll always have a vertex shader
    addShader(m_shaderID, shader1Code, GL_VERTEX_SHADER);

    // If we included 3 files, that means we're using a geometry shader
    if (shader3Code != "")
    {
        addShader(m_shaderID, shader2Code, GL_GEOMETRY_SHADER);
        addShader(m_shaderID, shader3Code, GL_FRAGMENT_SHADER);
    }
    else
    {
        addShader(m_shaderID, shader2Code, GL_FRAGMENT_SHADER);
    }

    compileProgram();
}

void Shader::validate()
{
    GLint result {0};
    GLchar eLog[1024] {0};

    // Check if the shader program can execute on the current OpenGL state
    glValidateProgram(m_shaderID);
    glGetProgramiv(m_shaderID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(m_shaderID, sizeof(eLog), NULL, eLog);
        throw std::runtime_error("Error validating program: " + std::string{eLog});
    }
}

void Shader::compileProgram()
{
    // Linking all the shaders together into a single program on the GPU.
    // Checks to make sure that the inputs and outputs of the shaders match up.
    // Creates an executable that runs on the programmable parts of the GPU
    glLinkProgram(m_shaderID);

    GLint result {0};
    GLchar eLog[1024] {0};

    // Check for errors
    glGetProgramiv(m_shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(m_shaderID, sizeof(eLog), NULL, eLog);
        throw std::runtime_error("Error linking program: " + std::string{eLog});
    }

    // Uniform variables let us pass info from the CPU to the GPU.
    // We want to store these IDs now so that we don't have to query
    // for them every frame

    // Camera/Window
    m_uniformVariables.uniformModel                = glGetUniformLocation(m_shaderID, "model");
    m_uniformVariables.uniformProjection           = glGetUniformLocation(m_shaderID, "projection");
    m_uniformVariables.uniformView                 = glGetUniformLocation(m_shaderID, "view");

    // Specular
    m_uniformVariables.uniformEyePosition          = glGetUniformLocation(m_shaderID, "eyePosition");
    m_uniformVariables.uniformSpecularIntensity    = glGetUniformLocation(m_shaderID, "material.specularIntensity");
    m_uniformVariables.uniformShininess            = glGetUniformLocation(m_shaderID, "material.shininess");

    // Point lights
    std::stringstream ss {};
    for (size_t i {0}; i < scene::MAX_POINT_LIGHTS; ++i)
    {
        // Base color
        ss << "pointLights[" << i << "].base.color";
        m_uniformPointLights[i].uniformColor = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Ambient intensity
        ss << "pointLights[" << i << "].base.ambientIntensity";
        m_uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Diffuse intensity
        ss << "pointLights[" << i << "].base.diffuseIntensity";
        m_uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Position
        ss << "pointLights[" << i << "].position";
        m_uniformPointLights[i].uniformPosition = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Exponential
        ss << "pointLights[" << i << "].exponential";
        m_uniformPointLights[i].uniformExponential = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Linear
        ss << "pointLights[" << i << "].linear";
        m_uniformPointLights[i].uniformLinear = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags

        // Constant
        ss << "pointLights[" << i << "].constant";
        m_uniformPointLights[i].uniformConstant = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }
    
    // Spot lights
    m_uniformVariables.uniformFlashLightOn        = glGetUniformLocation(m_shaderID, "flashLightOn");
    m_uniformSpotLight.uniformColor               = glGetUniformLocation(m_shaderID, "spotLight.base.base.color");
    m_uniformSpotLight.uniformAmbientIntensity    = glGetUniformLocation(m_shaderID, "spotLight.base.base.ambientIntensity");
    m_uniformSpotLight.uniformDiffuseIntensity    = glGetUniformLocation(m_shaderID, "spotLight.base.base.diffuseIntensity");

    m_uniformSpotLight.uniformPosition            = glGetUniformLocation(m_shaderID, "spotLight.base.position");
    m_uniformSpotLight.uniformExponential         = glGetUniformLocation(m_shaderID, "spotLight.base.exponential");
    m_uniformSpotLight.uniformLinear              = glGetUniformLocation(m_shaderID, "spotLight.base.linear");
    m_uniformSpotLight.uniformConstant            = glGetUniformLocation(m_shaderID, "spotLight.base.constant");

    m_uniformSpotLight.uniformDirection           = glGetUniformLocation(m_shaderID, "spotLight.direction");
    m_uniformSpotLight.uniformEdge                = glGetUniformLocation(m_shaderID, "spotLight.edge");

    m_uniformVariables.uniformTexture             = glGetUniformLocation(m_shaderID, "theTexture");

    // Point light shadow maps. Values in the omni_shadow_map shaders
    m_uniformVariables.uniformOmniLightPos        = glGetUniformLocation(m_shaderID, "lightPos");
    m_uniformVariables.uniformFarPlane            = glGetUniformLocation(m_shaderID, "farPlane");

    // Light matrices
    for (size_t i {0}; i < 6; ++i)
    {
        ss << "lightMatrices[" << i << "]";
        m_uniformLightMatrices[i] = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }

    for (size_t i {0}; i < scene::MAX_POINT_LIGHTS + 1; ++i)
    {
        // Shadowmap
        ss << "omniShadowMaps[" << i << "].shadowMap";
        m_uniformOmniShadowMaps[i].shadowMap = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
                    
        // Far plane
        ss << "omniShadowMaps[" << i << "].farPlane";
        m_uniformOmniShadowMaps[i].farPlane = glGetUniformLocation(m_shaderID, ss.str().c_str());
        ss.str(""); // Clear the buffer
        ss.clear(); // Clear error flags
    }
}

void Shader::setPointLightsWithoutShadows(const std::array<PointLight*, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount) const
{
    // Clamp the number of lights allowed
    if (lightCount > scene::MAX_POINT_LIGHTS) lightCount =  scene::MAX_POINT_LIGHTS;

    for (size_t i {0}; i < lightCount; ++i)
    {
        pLights[i]->useLight(m_uniformPointLights[i].uniformAmbientIntensity, m_uniformPointLights[i].uniformDiffuseIntensity,
                            m_uniformPointLights[i].uniformColor, m_uniformPointLights[i].uniformPosition, 
                            m_uniformPointLights[i].uniformExponential, m_uniformPointLights[i].uniformLinear, m_uniformPointLights[i].uniformConstant);
    }
}

void Shader::setPointLights(const std::array<PointLight*, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const
{
    // Clamp the number of lights allowed
    if (lightCount > scene::MAX_POINT_LIGHTS) lightCount =  scene::MAX_POINT_LIGHTS;

    for (size_t i {0}; i < lightCount; ++i)
    {
        pLights[i]->useLight(m_uniformPointLights[i].uniformAmbientIntensity, m_uniformPointLights[i].uniformDiffuseIntensity,
                            m_uniformPointLights[i].uniformColor, m_uniformPointLights[i].uniformPosition, 
                            m_uniformPointLights[i].uniformExponential, m_uniformPointLights[i].uniformLinear, m_uniformPointLights[i].uniformConstant);

        // We need the GL_TEXTURE0 since it needs to be an enum type
        pLights[i]->shadowMapRead(GL_TEXTURE0 + textureUnit + i);
        // Offset for point lights will usually just be 0.
        // The offset is to take into account that the shadowmaps are all 1 array in the shader
        glUniform1i(m_uniformOmniShadowMaps[i + offset].shadowMap, textureUnit + i);
        glUniform1f(m_uniformOmniShadowMaps[i + offset].farPlane, pLights[i]->getFarPlane());
    }
}

// We'll only call this once since nothing but the position and shadow map will change
void Shader::setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const
{
    glUniform1i(m_uniformVariables.uniformFlashLightOn, sLight->isOn());
    sLight->useLight(m_uniformSpotLight.uniformAmbientIntensity, m_uniformSpotLight.uniformDiffuseIntensity,
                        m_uniformSpotLight.uniformColor, m_uniformSpotLight.uniformPosition, m_uniformSpotLight.uniformDirection,
                        m_uniformSpotLight.uniformExponential, m_uniformSpotLight.uniformLinear, m_uniformSpotLight.uniformConstant,
                        m_uniformSpotLight.uniformEdge);

    if (!shadowsEnabled)
        return;

    // We need the GL_TEXTURE0 since it needs to be an enum type
    sLight->shadowMapRead(GL_TEXTURE0 + textureUnit);
    // The offset is to take into account that the shadowmaps are all 1 array in the shader
    glUniform1i(m_uniformOmniShadowMaps[offset].shadowMap, textureUnit);
    glUniform1f(m_uniformOmniShadowMaps[offset].farPlane, sLight->getFarPlane());
}

// Called every frame
void Shader::setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const
{
    glUniform1i(m_uniformVariables.uniformFlashLightOn, sLight->isOn());

    // No need to set all of these values if the spotlight isn't even on
    if (!sLight->isOn())
        return;

    sLight->setPosAndDir(m_uniformSpotLight.uniformPosition, m_uniformSpotLight.uniformDirection);

    if (!shadowsEnabled)
        return;

    // The offset is to take into account that the shadowmaps are all 1 array in the shader
    glUniform1i(m_uniformOmniShadowMaps[offset].shadowMap, textureUnit);
}

void Shader::setLightMatrices(const std::array<glm::mat4, 6>& lightMatrices) const
{
    for (size_t i {0}; i < 6; ++i)
        glUniformMatrix4fv(m_uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
}

Shader::~Shader()
{
    if (m_shaderID != 0)
    {
        glDeleteProgram(m_shaderID);
        m_shaderID = 0;
    }

    m_uniformVariables.uniformModel        = 0;
    m_uniformVariables.uniformProjection   = 0;
}
