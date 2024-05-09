#pragma once

#include "CommonValues.h"
#include "Shader.h"
#include "Camera.h"
#include "Sun.h"
#include "Planet.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "SceneHandler.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"

class SolarSystemRenderer
{
    public:
        SolarSystemRenderer();
        bool* getShadowsEnabledAddress();
        bool getShadowsEnabled();
        void toggleShadows();
        void createShaders(PointLight* pointLights[], glm::mat4 projection);
        void setupPostProcessingObjects();
        void omniShadowMapPass(PointLight* light);
        void renderObjects(std::vector<SpaceObject*> objects, GLuint uniformModel);
        void renderPass(glm::mat4 view);
        ~SolarSystemRenderer();

    private:
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
        PostProcessingResources postProcessingResources;

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
        UniformVariables uniformVariables;

        struct Shaders {
            Shader* mainShader {};
            Shader* mainShaderWithoutShadows {};
            Shader* mainShaderWithShadows {};
            Shader* sunShader {};
            Shader* omniShadowShader {};
            Shader* hdrShader {};
            Shader* bloomShader {};
            Shader* halfShader {};
        };
        Shaders shaders;

        std::vector<SpaceObject*> stars {};
        std::vector<SpaceObject*> satellites {};

        Skybox skybox {};
        Camera camera {};

        unsigned int pointLightCount {};
        bool shadowsEnabled {false};
};
