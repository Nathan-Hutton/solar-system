#pragma once

#include "CommonValues.h"
#include "Shader.h"
#include "Sun.h"
#include "Planet.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"

class SolarSystemRenderer
{
    public:
        SolarSystemRenderer();
        bool* getShadowsEnabledAddress();
        bool getShadowsEnabled();
        void toggleShadows(std::vector<SpaceObject*>& satellites, std::vector<SpaceObject*>& stars);
        void createShaders(PointLight* pointLights[], GLuint pointLightCount, glm::mat4 projection, SpotLight* spotLight);
        void setLightUniformVariables(std::vector<SpaceObject*>& satellites);
        void setupPostProcessingObjects();
        void omniShadowMapPass(PointLight* light, const std::vector<SpaceObject*>& satellites);
        void renderObjects(const std::vector<SpaceObject*>& objects, GLuint uniformModel);
        void renderPass(const glm::mat4& view, SpotLight* spotLight, GLuint pointLightCount, const glm::vec3& eyePos, const std::vector<SpaceObject*>& satellites, const std::vector<SpaceObject*>& stars, Skybox* skybox);
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

        bool shadowsEnabled {false};
};
