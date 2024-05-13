#pragma once

#include "CommonValues.h"
#include "Shader.h"
#include "Scene.h"

namespace SolarSystemRenderer
{

    struct PostProcessingResources {
        GLuint halfFBO {};
        GLuint halfTexture {};
        GLuint postProcessingFBO {};
        GLuint postProcessingTexture {};
        GLuint textureToBlur {};
        Mesh* framebufferQuad {};
        unsigned int pingPongFBO[2] {};
        unsigned int pingPongBuffer[2] {};
    };
    extern PostProcessingResources postProcessingResources;

    struct UniformVariables {
        GLuint uniformModelPlanets {};
        GLuint uniformViewPlanets {};
        GLuint uniformEyePositionPlanets {};
        GLuint uniformSpecularIntensityPlanets {};
        GLuint uniformShininessPlanets {};
        GLuint uniformOmniLightPos {};
        GLuint uniformFarPlane {};
        GLuint uniformModelSuns {};
        GLuint uniformViewSuns {};
        GLuint uniformModelOmniShadowMap {};
        GLuint uniformHorizontal {};
    };
    extern UniformVariables uniformVariables;

    struct Shaders {
        Shader* mainShader {};
        Shader* mainShaders[2] {};
        //Shader* mainShaderWithoutShadows {};
        //Shader* mainShaderWithShadows {};
        Shader* sunShader {};
        Shader* omniShadowShader {};
        Shader* hdrShader {};
        Shader* bloomShader {};
        Shader* halfShader {};
    };
    extern Shaders shaders;

    extern bool shadowsEnabled;

    void toggleShadows();
    void createShaders(glm::mat4 projection);
    void setLightUniformVariables();
    void setupPostProcessingObjects();
    void setup(glm::mat4 projection);
    void omniShadowMapPass(PointLight* light);
    void renderObjectsVector(const std::vector<SpaceObject*>& objects, GLuint uniformModel);
    void omniShadowMapPasses();
    void handleBloom();
    void renderToScreen();
    void renderPass();
};
