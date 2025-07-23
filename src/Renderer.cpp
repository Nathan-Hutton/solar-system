#include "Renderer.h"

#include <stdexcept>
#include <array>
#include <memory>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Skybox.h"
#include "Shader.h"
#include "MainShader.h"
#include "Scene.h"
#include "Camera.h"
#include "Window.h"

namespace
{
    struct PostProcessingResources {
        GLuint halfFBO {};
        GLuint halfTexture {};
        GLuint postProcessingFBO {};
        GLuint postProcessingTexture {};
        GLuint textureToBlur {};
        std::array<unsigned int, 2> pingPongFBO;;
        std::array<unsigned int, 2> pingPongBuffer;
        std::unique_ptr<Mesh> framebufferQuad {};
    };
    PostProcessingResources postProcessingResources {};

    struct UniformVariables {
        GLuint uniformModelPlanets {};
        GLuint uniformModelToClipSpacePlanets {};
        GLuint uniformModelToClipSpaceSuns {};
        GLuint uniformEyePositionPlanets {};
        GLuint uniformSpecularIntensityPlanets {};
        GLuint uniformShininessPlanets {};
        GLuint uniformOmniLightPos {};
        GLuint uniformFarPlane {};
        GLuint uniformModelSuns {};
        GLuint uniformModelOmniShadowMap {};
        GLuint uniformHorizontal {};
        GLuint uniformLightMatrices[6] {};
    };
    UniformVariables uniformVariables {};

    struct Shaders {
        std::unique_ptr<MainShader> mainShader {}; // Initially, this is the shader that doesn't use shadows
        std::unique_ptr<MainShader> shaderNotInUse {}; // Initially this is the shader that uses shadows
        std::unique_ptr<Shader> sunShader {};
        std::unique_ptr<Shader> omniShadowShader {};
        std::unique_ptr<Shader> hdrShader {};
        std::unique_ptr<Shader> bloomShader {};
        std::unique_ptr<Shader> halfShader {};
    };
    Shaders shaders {};

    glm::mat4 g_projection {};
}

// Shader nonsense
namespace 
{
    void createShaders()
    {
        // Shader for the suns (no lighting or shadows)
        shaders.sunShader = std::make_unique<Shader>();
        shaders.sunShader->createFromFiles("../assets/shaders/sunShader.vert", "../assets/shaders/sunShader.frag");
        shaders.sunShader->useShader();
        shaders.sunShader->setTexture(2);
        shaders.sunShader->validate();
        // For the sun shaders we don't do any light or shadow calculations
        uniformVariables.uniformModelSuns    = glGetUniformLocation(shaders.sunShader->getShaderID(), "model");
        uniformVariables.uniformModelToClipSpaceSuns = glGetUniformLocation(shaders.sunShader->getShaderID(), "modelToClipSpace");

        shaders.omniShadowShader = std::make_unique<Shader>();
        shaders.omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
            "../assets/shaders/omni_shadow_map.geom",
            "../assets/shaders/omni_shadow_map.frag");

        // The shadow map shader will record the depth values of all objects except suns
        uniformVariables.uniformModelOmniShadowMap   = glGetUniformLocation(shaders.omniShadowShader->getShaderID(), "model");
        uniformVariables.uniformOmniLightPos         = glGetUniformLocation(shaders.omniShadowShader->getShaderID(), "lightPos");
        uniformVariables.uniformFarPlane             = glGetUniformLocation(shaders.omniShadowShader->getShaderID(), "farPlane");
        
        // Light matrices
        std::stringstream ss {};
        for (size_t i {0}; i < 6; ++i)
        {
            ss << "lightMatrices[" << i << "]";
            uniformVariables.uniformLightMatrices[i] = glGetUniformLocation(shaders.omniShadowShader->getShaderID(), ss.str().c_str());
            ss.str(""); // Clear the buffer
            ss.clear(); // Clear error flags
        }

        shaders.hdrShader = std::make_unique<Shader>();
        shaders.hdrShader->createFromFiles("../assets/shaders/hdrShader.vert", "../assets/shaders/hdrShader.frag");
        shaders.hdrShader->useShader();
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "theTexture"), 0);
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "blurTexture"), 1);

        shaders.bloomShader = std::make_unique<Shader>();
        shaders.bloomShader->createFromFiles("../assets/shaders/bloomShader.vert",  "../assets/shaders/bloomShader.frag");
        shaders.bloomShader->useShader();
        shaders.bloomShader->setTexture(0);
        uniformVariables.uniformHorizontal = glGetUniformLocation(shaders.bloomShader->getShaderID(), "horizontal");
        
        // Shader for the movables, moons, and models. Includes shadows
        shaders.shaderNotInUse = std::make_unique<MainShader>();
        shaders.shaderNotInUse->createFromFiles("../assets/shaders/planetShaderShadows.vert", "../assets/shaders/planetShaderShadows.frag");
        shaders.shaderNotInUse->setLightsUniformVariables();
        shaders.shaderNotInUse->useShader();
        shaders.shaderNotInUse->setTexture(2);
        shaders.shaderNotInUse->validate();
        shaders.shaderNotInUse->setSpotLight(camera::spotLight.get(), true, 4+scene::pointLightCount, scene::pointLightCount);
        // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
        // texture, and the third is the texture(s) of the objects we're rendering
        shaders.shaderNotInUse->setPointLights(scene::pointLights, scene::pointLightCount, 4, 0);
        glUniform1i(glGetUniformLocation(shaders.shaderNotInUse->getShaderID(), "pointLightCount"), scene::pointLightCount);

        shaders.halfShader = std::make_unique<Shader>();
        shaders.halfShader->createFromFiles("../assets/shaders/half.vert", "../assets/shaders/half.frag");
        shaders.halfShader->useShader();
        shaders.halfShader->setTexture(0);
        
        // Shader for the movables, moons, and models. Doesn't use shadows
        shaders.mainShader = std::make_unique<MainShader>();
        shaders.mainShader->createFromFiles("../assets/shaders/planetShaderNoShadows.vert", "../assets/shaders/planetShaderNoShadows.frag");
        shaders.mainShader->setLightsUniformVariables();
        shaders.mainShader->useShader();
        shaders.mainShader->setTexture(2);
        shaders.mainShader->validate();
        shaders.mainShader->setSpotLight(camera::spotLight.get(), false, 4+scene::pointLightCount, scene::pointLightCount);
        shaders.mainShader->setPointLightsWithoutShadows(scene::pointLights, scene::pointLightCount);
        glUniform1i(glGetUniformLocation(shaders.mainShader->getShaderID(), "pointLightCount"), scene::pointLightCount);

        // This is so we can disable shadows
        // By default, shadows will be turned off
        uniformVariables.uniformModelPlanets                = glGetUniformLocation(shaders.mainShader->getShaderID(), "model");
        uniformVariables.uniformModelToClipSpacePlanets     = glGetUniformLocation(shaders.mainShader->getShaderID(), "modelToClipSpace");
        uniformVariables.uniformEyePositionPlanets          = glGetUniformLocation(shaders.mainShader->getShaderID(), "eyePosition");
        uniformVariables.uniformSpecularIntensityPlanets    = glGetUniformLocation(shaders.mainShader->getShaderID(), "material.specularIntensity");
        uniformVariables.uniformShininessPlanets            = glGetUniformLocation(shaders.mainShader->getShaderID(), "material.shininess");
    }

    void setupPostProcessingObjects()
    {
        float quadVertices[] = {
            // Positions     // Texture Coordinates
            1.0f,  1.0f,    1.0f, 1.0f,  // Top Right
            1.0f, -1.0f,    1.0f, 0.0f,  // Bottom Right
           -1.0f, -1.0f,    0.0f, 0.0f,  // Bottom Left
           -1.0f,  1.0f,    0.0f, 1.0f   // Top Left 
        };
        unsigned int quadIndices[] = {
            0, 1, 3,  // First Triangle (Top Right, Bottom Right, Top Left)
            1, 2, 3   // Second Triangle (Bottom Right, Bottom Left, Top Left)
        };
        postProcessingResources.framebufferQuad = std::make_unique<Mesh>(quadVertices, quadIndices, 16, 6, /*hasNormals=*/false, /*threeVertices=*/false);

        // HDR
        glGenFramebuffers(1, &postProcessingResources.postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);
        glViewport(0, 0, window::windowWidth, window::windowHeight);

        // Make the main framebuffer texture
        glGenTextures(1, &postProcessingResources.postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window::windowWidth, window::windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.postProcessingTexture, 0);

        // Make texture for bright spots so we cacn have bloom
        glGenTextures(1, &postProcessingResources.textureToBlur);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window::windowWidth, window::windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, postProcessingResources.textureToBlur, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        unsigned int RBO {};
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window::windowWidth, window::windowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
        if (status != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer error in Renderer namespace when trying to make RenderBuffer");

        const unsigned int attachments[2] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);

        // Setup the ping pong framebuffers to take in half-sized textures and output half-sized textures
        glGenFramebuffers(2, postProcessingResources.pingPongFBO.data());
        glGenTextures(2, postProcessingResources.pingPongBuffer.data());
        for (int i {0}; i < 2; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[i], 0);

            const GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
            if (status != GL_FRAMEBUFFER_COMPLETE)
                throw std::runtime_error("Framebuffer error in Renderer namespace when trying to make PingPong buffers");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shaders.hdrShader->useShader();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[1]);
        glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "theTexture"), 3);
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "blurTexture"), 4);

        glGenFramebuffers(1, &postProcessingResources.halfFBO);
        glGenTextures(1, &postProcessingResources.halfTexture);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.halfFBO);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.halfTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.halfTexture, 0);
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Framebuffer error in Renderer namespace when trying to make postProcessingResources FBO");
    }


    void setSpecularUniformVariables()
    {
        for (SpaceObject* satellite : scene::litObjects)
            satellite->setUniformVariables(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);
    }

    void setLightMatrices(const std::array<glm::mat4, 6>& lightMatrices)
    {
        for (size_t i {0}; i < 6; ++i)
            glUniformMatrix4fv(uniformVariables.uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));
    }

}

// Called when renderering
namespace
{
    bool shadowsEnabled {false};

    void renderObjectsVector(const std::vector<SpaceObject*>& objects, GLuint uniformModel, GLuint uniformModelToClipSpace, const glm::mat4& worldToClip)
    {
        // They'll all use GL_TEXTURE2
        glActiveTexture(GL_TEXTURE2);
        for (SpaceObject* object : objects)
        {
            glm::mat4 model {1.0f};
            object->setWorldProperties(model);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformModelToClipSpace, 1, GL_FALSE, glm::value_ptr(worldToClip * model));

			std::shared_ptr<Material> material{ object->getMaterial() };
			if (material != nullptr)
			{
				material->useMaterial(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);
			}
            object->render();
        }
    }

    void omniShadowMapPass(const PointLight* const light)
    {
        shaders.omniShadowShader->useShader();

        // Make the viewport the same dimenstions as the FBO
        glViewport(0, 0, light->getShadowMapWidth(), light->getShadowMapHeight());

        // Bind our framebuffer so that the shader output goes to it
        // Every draw call after this will go to that framebuffer
        light->shadowMapWrite();
        glClear(GL_DEPTH_BUFFER_BIT);

        // Get the uniformModel and lightTransform for the shader

        glUniform3fv(uniformVariables.uniformOmniLightPos, 1, glm::value_ptr(light->getPosition()));
        glUniform1f(uniformVariables.uniformFarPlane, light->getFarPlane());
        setLightMatrices(light->calculateLightTransform());

        shaders.omniShadowShader->validate();

        // Draw just to the depth buffer
        //glActiveTexture(GL_TEXTURE2);
        for (const std::unique_ptr<SpaceObject>& satellite : scene::movables)
        {
            glm::mat4 model {1.0f};
            satellite->setWorldProperties(model);
            glUniformMatrix4fv(uniformVariables.uniformModelOmniShadowMap, 1, GL_FALSE, glm::value_ptr(model));
            satellite->render();
        }

        // Bind the default framebuffer
        // If we called swapbuffers without doing this the image wouldn't change
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderAllObjects()
    {
        glm::mat4 view;
        camera::calculateViewMatrix(view);
        glm::mat4 worldToClip = g_projection * view;

        // ====================================
        // RENDER SUNS
        // ====================================

        shaders.sunShader->useShader();
        
        // Apply view matrix.
        // View matrix represents the camera's position and orientation in world.
        // The world is actually moved to and rotated around the camera with the view matrix. The camera is stationary.
        //glUniformMatrix4fv(uniformVariables.uniformViewSuns, 1, GL_FALSE, glm::value_ptr(view));

        renderObjectsVector(scene::lightEmitters, uniformVariables.uniformModelSuns, uniformVariables.uniformModelToClipSpaceSuns, worldToClip);

        // =================================================
        // RENDER PLANETS, MOONS, ASTEROIDS, and the SKYBOX
        // =================================================

        shaders.mainShader->useShader();
        shaders.mainShader->setSpotLightDirAndPos(camera::spotLight.get(), shadowsEnabled, 4+scene::pointLightCount, scene::pointLightCount);

        // Eye position is for specular lighting
        glUniform3fv(uniformVariables.uniformEyePositionPlanets, 1, glm::value_ptr(camera::position));

        renderObjectsVector(scene::litObjects, uniformVariables.uniformModelPlanets, uniformVariables.uniformModelToClipSpacePlanets, worldToClip);

        // ==============
        // RENDER SKYBOX
        // ==============
        skybox::drawSkybox(view, g_projection);
    }

    void handleBloom()
    {
        // Half the bloom texture size before the pingPongFBOs can actually start bluring it
        glViewport(0, 0, window::windowWidth/2, window::windowHeight/2);

        shaders.halfShader->useShader();
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.halfFBO);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
        postProcessingResources.framebufferQuad->render();

        // Ping-pong bloom effect. Performs horiztonal and vertical bluring.
        // First iteration of the bloom effect. This means we don't need if conditions in the for loop
        bool horizontal {false};
        shaders.bloomShader->useShader();
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[!horizontal]);
        glUniform1i(uniformVariables.uniformHorizontal, !horizontal);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.halfTexture);
        postProcessingResources.framebufferQuad->render();

        const int amount {5};
        for (int _ {0}; _ < amount; ++_)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.pingPongFBO[horizontal]);
            glUniform1i(uniformVariables.uniformHorizontal, horizontal);

            // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
            // Move the data between pingPong FBOs
            glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[!horizontal]);
            postProcessingResources.framebufferQuad->render();
            horizontal = !horizontal;
        }

        // Make the viewport regularly sized again
        glViewport(0, 0, window::windowWidth, window::windowHeight);
    }
    
    // Render to the screen after we've done all the post-processing effects
    void renderToScreen()
    {
        // The HDR shader uses the texture of the entire scene + the bloom texture to render to the screen.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaders.hdrShader->useShader();
        postProcessingResources.framebufferQuad->render();
    }
}

void renderer::toggleShadows()
{
    shadowsEnabled = !shadowsEnabled;
    std::swap(shaders.mainShader, shaders.shaderNotInUse);

    uniformVariables.uniformModelToClipSpacePlanets     = glGetUniformLocation(shaders.mainShader->getShaderID(), "modelToClipSpace");
    uniformVariables.uniformEyePositionPlanets          = glGetUniformLocation(shaders.mainShader->getShaderID(), "eyePosition");
    uniformVariables.uniformSpecularIntensityPlanets    = glGetUniformLocation(shaders.mainShader->getShaderID(), "material.specularIntensity");
    uniformVariables.uniformShininessPlanets            = glGetUniformLocation(shaders.mainShader->getShaderID(), "material.shininess");

    setSpecularUniformVariables();
}

void renderer::setup(const glm::mat4& projection)
{
    g_projection = projection;
    createShaders();
    setupPostProcessingObjects();
    setSpecularUniformVariables();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void renderer::omniShadowMapPasses()
{
    if (!shadowsEnabled)
        return;

    for (int i {0}; i < scene::pointLightCount; ++i)
        omniShadowMapPass(scene::pointLights[i]);
    omniShadowMapPass(camera::spotLight.get());
}

void renderer::renderPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);
    glViewport(0, 0, window::windowWidth, window::windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderAllObjects();
    handleBloom();
    renderToScreen();
}
