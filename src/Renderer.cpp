#include "Renderer.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Skybox.h"
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"

namespace
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
    PostProcessingResources postProcessingResources {};

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
    UniformVariables uniformVariables {};

    struct Shaders {
        Shader* mainShader {};
        Shader* mainShaders[2] {};
        Shader* sunShader {};
        Shader* omniShadowShader {};
        Shader* hdrShader {};
        Shader* bloomShader {};
        Shader* halfShader {};
    };
    Shaders shaders {};
}

namespace 
{
    void createShaders(const glm::mat4 projection)
    {
        // Shader for the suns (no lighting or shadows)
        shaders.sunShader = new Shader{};
        shaders.sunShader->createFromFiles("../assets/shaders/sunShader.vert", "../assets/shaders/sunShader.frag");
        shaders.sunShader->useShader();
        glUniformMatrix4fv(shaders.sunShader->getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
        shaders.sunShader->setTexture(2);
        shaders.sunShader->validate();
        // For the sun shaders we don't do any light or shadow calculations
        uniformVariables.uniformModelSuns    = shaders.sunShader->getModelLocation();
        uniformVariables.uniformViewSuns     = shaders.sunShader->getViewLocation();

        shaders.omniShadowShader = new Shader{};
        shaders.omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
            "../assets/shaders/omni_shadow_map.geom",
            "../assets/shaders/omni_shadow_map.frag");

        // The shadow map shader will record the depth values of all objects except suns
        uniformVariables.uniformModelOmniShadowMap   = shaders.omniShadowShader->getModelLocation();
        uniformVariables.uniformOmniLightPos         = shaders.omniShadowShader->getOmniLightPosLocation();
        uniformVariables.uniformFarPlane             = shaders.omniShadowShader->getFarPlaneLocation();

        shaders.hdrShader = new Shader{};
        shaders.hdrShader->createFromFiles("../assets/shaders/hdrShader.vert", "../assets/shaders/hdrShader.frag");
        shaders.hdrShader->useShader();
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "theTexture"), 0);
        glUniform1i(glGetUniformLocation(shaders.hdrShader->getShaderID(), "blurTexture"), 1);

        shaders.bloomShader = new Shader{};
        shaders.bloomShader->createFromFiles("../assets/shaders/bloomShader.vert",  "../assets/shaders/bloomShader.frag");
        shaders.bloomShader->useShader();
        shaders.bloomShader->setTexture(0);
        uniformVariables.uniformHorizontal = glGetUniformLocation(shaders.bloomShader->getShaderID(), "horizontal");
        
        // Shader for the satellites, moons, and models. Includes shadows
        shaders.mainShaders[1] = new Shader{};
        shaders.mainShaders[1]->createFromFiles("../assets/shaders/planetShaderShadows.vert", "../assets/shaders/planetShaderShadows.frag");
        shaders.mainShaders[1]->useShader();
        shaders.mainShaders[1]->setTexture(2);
        shaders.mainShaders[1]->validate();
        shaders.mainShaders[1]->setSpotLight(camera::spotLight, true, 4+scene::pointLightCount, scene::pointLightCount);
        glUniformMatrix4fv(glGetUniformLocation(shaders.mainShaders[1]->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
        // texture, and the third is the texture(s) of the objects we're rendering
        shaders.mainShaders[1]->setPointLights(scene::pointLights, scene::pointLightCount, 4, 0);
        glUniform1i(glGetUniformLocation(shaders.mainShaders[1]->getShaderID(), "pointLightCount"), scene::pointLightCount);

        shaders.halfShader = new Shader{};
        shaders.halfShader->createFromFiles("../assets/shaders/half.vert", "../assets/shaders/half.frag");
        shaders.halfShader->useShader();
        shaders.halfShader->setTexture(0);
        
        // Shader for the satellites, moons, and models. Doesn't use shadows
        shaders.mainShaders[0] = new Shader{};
        shaders.mainShaders[0]->createFromFiles("../assets/shaders/planetShaderNoShadows.vert", "../assets/shaders/planetShaderNoShadows.frag");
        shaders.mainShaders[0]->useShader();
        shaders.mainShaders[0]->setTexture(2);
        shaders.mainShaders[0]->validate();
        shaders.mainShaders[0]->setSpotLight(camera::spotLight, false, 4+scene::pointLightCount, scene::pointLightCount);
        glUniformMatrix4fv(glGetUniformLocation(shaders.mainShaders[0]->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        shaders.mainShaders[0]->setPointLightsWithoutShadows(scene::pointLights, scene::pointLightCount);
        glUniform1i(glGetUniformLocation(shaders.mainShaders[0]->getShaderID(), "pointLightCount"), scene::pointLightCount);

        // This is so we can disable shadows
        // By default, shadows will be turned off
        shaders.mainShader                                  = shaders.mainShaders[0];
        uniformVariables.uniformModelPlanets                = shaders.mainShader->getModelLocation();
        uniformVariables.uniformViewPlanets                 = shaders.mainShader->getViewLocation();
        uniformVariables.uniformEyePositionPlanets          = shaders.mainShader->getEyePositionLocation();
        uniformVariables.uniformSpecularIntensityPlanets    = shaders.mainShader->getSpecularIntensityLocation();
        uniformVariables.uniformShininessPlanets            = shaders.mainShader->getShininessLocation();
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
        postProcessingResources.framebufferQuad = new Mesh{};
        postProcessingResources.framebufferQuad->createMesh(quadVertices, quadIndices, 16, 6, false, false);
        // HDR
        glGenFramebuffers(1, &postProcessingResources.postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);
        glViewport(0, 0, 1920, 1200);

        // Make the main framebuffer texture
        glGenTextures(1, &postProcessingResources.postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingResources.postProcessingTexture, 0);

        // Make texture for bright spots so we cacn have bloom
        glGenTextures(1, &postProcessingResources.textureToBlur);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, postProcessingResources.textureToBlur, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        unsigned int RBO {};
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1200);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer error: " << status << '\n';
            std::exit(EXIT_FAILURE);
        }

        const unsigned int attachments[2] {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, attachments);

        // Setup the ping pong framebuffers to take in half-sized textures and output half-sized textures
        glGenFramebuffers(2, postProcessingResources.pingPongFBO);
        glGenTextures(2, postProcessingResources.pingPongBuffer);
        for (unsigned int i {0}; i < 2; ++i)
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
            {
                std::cerr << "Framebuffer Error: " << status << '\n';
                std::exit(EXIT_FAILURE);
            }
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
        {
            std::cerr << "Framebuffer Error: " << status << '\n';
            std::exit(EXIT_FAILURE);
        }
    }


    void setLightUniformVariables()
    {
        for (SpaceObject *satellite : scene::satellites)
            satellite->setUniformVariables(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);
    }
}

namespace
{
    bool shadowsEnabled {false};

    void renderObjectsVector(const std::vector<SpaceObject*>& objects, GLuint uniformModel)
    {
        // Apply rotations, transformations, and render objects
        // Objects vertices are first transformed by the model matrix, then the view matrix
        // to bring them into camera space, positioning them relative to the camera,
        // then the projection matrix is applied to the view space coordinates to project them
        // onto our 2D screen. 
        // In the shader: gl_Position = projection * view * model * vec4(pos, 1.0);

        // Model matrix positions and orients objects in the world.
        // Takes coordinates local to the ojbect and transforms them into coordinates relative to world space.
        glm::mat4 model {};

        // They'll all use GL_TEXTURE2
        glActiveTexture(GL_TEXTURE2);
        for (SpaceObject *object : objects)
        {
            model = glm::mat4{1.0f};
            object->setWorldProperties(&model);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            object->render();
        }
    }

    void omniShadowMapPass(PointLight* light)
    {
        shaders.omniShadowShader->useShader();

        // Make the viewport the same dimenstions as the FBO
        glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

        // Bind our framebuffer so that the shader output goes to it
        // Every draw call after this will go to that framebuffer
        light->getShadowMap()->write();
        glClear(GL_DEPTH_BUFFER_BIT);

        // Get the uniformModel and lightTransform for the shader

        glUniform3fv(uniformVariables.uniformOmniLightPos, 1, glm::value_ptr(light->getPosition()));
        glUniform1f(uniformVariables.uniformFarPlane, light->getFarPlane());
        shaders.omniShadowShader->setLightMatrices(light->calculateLightTransform());

        shaders.omniShadowShader->validate();

        // Draw just to the depth buffer
        renderObjectsVector(scene::satellites, uniformVariables.uniformModelOmniShadowMap);

        // Bind the default framebuffer
        // If we called swapbuffers without doing this the image wouldn't change
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void renderAllObjects()
    {
        const glm::mat4 view = camera::calculateViewMatrix();

        // ====================================
        // RENDER SUNS
        // ====================================

        shaders.sunShader->useShader();
        
        // Apply view matrix.
        // View matrix represents the camera's position and orientation in world.
        // The world is actually rotated around the camera with the view matrix. The camera is stationary.
        glUniformMatrix4fv(uniformVariables.uniformViewSuns, 1, GL_FALSE, glm::value_ptr(view));

        renderObjectsVector(scene::stars, uniformVariables.uniformModelSuns);

        // =================================================
        // RENDER PLANETS, MOONS, ASTEROIDS, and the SKYBOX
        // =================================================

        shaders.mainShader->useShader();
        shaders.mainShader->setSpotLightDirAndPos(camera::spotLight, shadowsEnabled, 4+scene::pointLightCount, scene::pointLightCount);

        // Eye position is for specular lighting
        glUniform3fv(uniformVariables.uniformEyePositionPlanets, 1, glm::value_ptr(camera::position));
        glUniformMatrix4fv(uniformVariables.uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));

        renderObjectsVector(scene::satellites, uniformVariables.uniformModelPlanets);

        // ==============
        // RENDER SKYBOX
        // ==============
        skybox::drawSkybox(view);
    }

    void handleBloom()
    {
        // Half the bloom texture size before the pingPongFBOs can actually start bluring it
        glViewport(0, 0, 960, 600);

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

        const int amount {4};
        for (unsigned int i {0}; i < amount; ++i)
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
        glViewport(0, 0, 1920, 1200);
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

namespace renderer
{
    void toggleShadows()
    {
        shadowsEnabled = !shadowsEnabled;
        shaders.mainShader = shaders.mainShaders[shadowsEnabled];

        shaders.mainShader                                  = shaders.mainShader;
        uniformVariables.uniformModelPlanets                = shaders.mainShader->getModelLocation();
        uniformVariables.uniformViewPlanets                 = shaders.mainShader->getViewLocation();
        uniformVariables.uniformEyePositionPlanets          = shaders.mainShader->getEyePositionLocation();
        uniformVariables.uniformSpecularIntensityPlanets    = shaders.mainShader->getSpecularIntensityLocation();
        uniformVariables.uniformShininessPlanets            = shaders.mainShader->getShininessLocation();

        for (SpaceObject *satellite : scene::satellites)
            satellite->setUniformVariables(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);
    }

    void setup(glm::mat4 projection)
    {
        createShaders(projection);
        setupPostProcessingObjects();
        setLightUniformVariables();
    }

    void omniShadowMapPasses()
    {
        if (!shadowsEnabled)
            return;

        for (size_t i {0}; i < scene::pointLightCount; ++i)
            omniShadowMapPass(scene::pointLights[i]);
        omniShadowMapPass(camera::spotLight);
    }

    void renderPass()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.postProcessingFBO);
        glViewport(0, 0, 1920, 1200);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        renderAllObjects();
        handleBloom();
        renderToScreen();
    }

}
