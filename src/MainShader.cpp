#include "MainShader.h"

#include <filesystem>

void MainShader::setLightsUniformVariables()
{
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

    m_uniformTexture                              = glGetUniformLocation(m_shaderID, "theTexture");

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

void MainShader::setPointLightsWithoutShadows(const std::array<PointLight*, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount) const
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

void MainShader::setPointLights(const std::array<PointLight*, scene::MAX_POINT_LIGHTS> pLights, unsigned int lightCount, unsigned int textureUnit, unsigned int offset) const
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
void MainShader::setSpotLight(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const
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
void MainShader::setSpotLightDirAndPos(SpotLight* sLight, bool shadowsEnabled, unsigned int textureUnit, unsigned int offset) const
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
