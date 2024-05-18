#pragma once

#include "CommonValues.h"
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"

namespace renderer
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
        Shader* sunShader {};
        Shader* omniShadowShader {};
        Shader* hdrShader {};
        Shader* bloomShader {};
        Shader* halfShader {};
    };
    extern Shaders shaders;

    void toggleShadows();
    void setup(const glm::mat4 projection);
    void omniShadowMapPasses();
    void renderPass();
};
