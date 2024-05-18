#include "Renderer.h"
#include "Skybox.h"

namespace 
{
    void createShaders(const glm::mat4 projection)
    {
        // Shader for the suns (no lighting or shadows)
        renderer::shaders.sunShader = new Shader{};
        renderer::shaders.sunShader->createFromFiles("../assets/shaders/sunShader.vert", "../assets/shaders/sunShader.frag");
        renderer::shaders.sunShader->useShader();
        glUniformMatrix4fv(renderer::shaders.sunShader->getProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));
        renderer::shaders.sunShader->setTexture(2);
        renderer::shaders.sunShader->validate();
        // For the sun shaders we don't do any light or shadow calculations
        renderer::uniformVariables.uniformModelSuns    = renderer::shaders.sunShader->getModelLocation();
        renderer::uniformVariables.uniformViewSuns     = renderer::shaders.sunShader->getViewLocation();

        renderer::shaders.omniShadowShader = new Shader{};
        renderer::shaders.omniShadowShader->createFromFiles("../assets/shaders/omni_shadow_map.vert",
            "../assets/shaders/omni_shadow_map.geom",
            "../assets/shaders/omni_shadow_map.frag");

        // The shadow map shader will record the depth values of all objects except suns
        renderer::uniformVariables.uniformModelOmniShadowMap   = renderer::shaders.omniShadowShader->getModelLocation();
        renderer::uniformVariables.uniformOmniLightPos         = renderer::shaders.omniShadowShader->getOmniLightPosLocation();
        renderer::uniformVariables.uniformFarPlane             = renderer::shaders.omniShadowShader->getFarPlaneLocation();

        renderer::shaders.hdrShader = new Shader{};
        renderer::shaders.hdrShader->createFromFiles("../assets/shaders/hdrShader.vert", "../assets/shaders/hdrShader.frag");
        renderer::shaders.hdrShader->useShader();
        glUniform1i(glGetUniformLocation(renderer::shaders.hdrShader->getShaderID(), "theTexture"), 0);
        glUniform1i(glGetUniformLocation(renderer::shaders.hdrShader->getShaderID(), "blurTexture"), 1);

        renderer::shaders.bloomShader = new Shader{};
        renderer::shaders.bloomShader->createFromFiles("../assets/shaders/bloomShader.vert",  "../assets/shaders/bloomShader.frag");
        renderer::shaders.bloomShader->useShader();
        renderer::shaders.bloomShader->setTexture(0);
        renderer::uniformVariables.uniformHorizontal = glGetUniformLocation(renderer::shaders.bloomShader->getShaderID(), "horizontal");
        
        // Shader for the satellites, moons, and models. Includes shadows
        renderer::shaders.mainShaders[1] = new Shader{};
        renderer::shaders.mainShaders[1]->createFromFiles("../assets/shaders/planetShaderShadows.vert", "../assets/shaders/planetShaderShadows.frag");
        renderer::shaders.mainShaders[1]->useShader();
        renderer::shaders.mainShaders[1]->setTexture(2);
        renderer::shaders.mainShaders[1]->validate();
        renderer::shaders.mainShaders[1]->setSpotLight(camera::spotLight, true, 4+scene::pointLightCount, scene::pointLightCount);
        glUniformMatrix4fv(glGetUniformLocation(renderer::shaders.mainShaders[1]->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // We need offsets of 4 since the first texture unit is the skybox, the second is the framebuffer
        // texture, and the third is the texture(s) of the objects we're rendering
        renderer::shaders.mainShaders[1]->setPointLights(scene::pointLights, scene::pointLightCount, 4, 0);
        glUniform1i(glGetUniformLocation(renderer::shaders.mainShaders[1]->getShaderID(), "pointLightCount"), scene::pointLightCount);

        renderer::shaders.halfShader = new Shader{};
        renderer::shaders.halfShader->createFromFiles("../assets/shaders/half.vert", "../assets/shaders/half.frag");
        renderer::shaders.halfShader->useShader();
        renderer::shaders.halfShader->setTexture(0);
        
        // Shader for the satellites, moons, and models. Doesn't use shadows
        renderer::shaders.mainShaders[0] = new Shader{};
        renderer::shaders.mainShaders[0]->createFromFiles("../assets/shaders/planetShaderNoShadows.vert", "../assets/shaders/planetShaderNoShadows.frag");
        renderer::shaders.mainShaders[0]->useShader();
        renderer::shaders.mainShaders[0]->setTexture(2);
        renderer::shaders.mainShaders[0]->validate();
        renderer::shaders.mainShaders[0]->setSpotLight(camera::spotLight, false, 4+scene::pointLightCount, scene::pointLightCount);
        glUniformMatrix4fv(glGetUniformLocation(renderer::shaders.mainShaders[0]->getShaderID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        renderer::shaders.mainShaders[0]->setPointLightsWithoutShadows(scene::pointLights, scene::pointLightCount);
        glUniform1i(glGetUniformLocation(renderer::shaders.mainShaders[0]->getShaderID(), "pointLightCount"), scene::pointLightCount);

        // This is so we can disable shadows
        // By default, shadows will be turned off
        renderer::shaders.mainShader             = renderer::shaders.mainShaders[0];
        renderer::uniformVariables.uniformModelPlanets                = renderer::shaders.mainShader->getModelLocation();
        renderer::uniformVariables.uniformViewPlanets                 = renderer::shaders.mainShader->getViewLocation();
        renderer::uniformVariables.uniformEyePositionPlanets          = renderer::shaders.mainShader->getEyePositionLocation();
        renderer::uniformVariables.uniformSpecularIntensityPlanets    = renderer::shaders.mainShader->getSpecularIntensityLocation();
        renderer::uniformVariables.uniformShininessPlanets            = renderer::shaders.mainShader->getShininessLocation();
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
        renderer::postProcessingResources.framebufferQuad = new Mesh{};
        renderer::postProcessingResources.framebufferQuad->createMesh(quadVertices, quadIndices, 16, 6, false, false);
        // HDR
        glGenFramebuffers(1, &renderer::postProcessingResources.postProcessingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.postProcessingFBO);
        glViewport(0, 0, 1920, 1200);

        // Make the main framebuffer texture
        glGenTextures(1, &renderer::postProcessingResources.postProcessingTexture);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.postProcessingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer::postProcessingResources.postProcessingTexture, 0);

        // Make texture for bright spots so we cacn have bloom
        glGenTextures(1, &renderer::postProcessingResources.textureToBlur);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.textureToBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 1920, 1200, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderer::postProcessingResources.textureToBlur, 0);

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
        glGenFramebuffers(2, renderer::postProcessingResources.pingPongFBO);
        glGenTextures(2, renderer::postProcessingResources.pingPongBuffer);
        for (unsigned int i {0}; i < 2; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.pingPongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.pingPongBuffer[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer::postProcessingResources.pingPongBuffer[i], 0);

            const GLenum status {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "Framebuffer Error: " << status << '\n';
                std::exit(EXIT_FAILURE);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        renderer::shaders.hdrShader->useShader();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.postProcessingTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.pingPongBuffer[1]);
        glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(renderer::shaders.hdrShader->getShaderID(), "theTexture"), 3);
        glUniform1i(glGetUniformLocation(renderer::shaders.hdrShader->getShaderID(), "blurTexture"), 4);

        glGenFramebuffers(1, &renderer::postProcessingResources.halfFBO);
        glGenTextures(1, &renderer::postProcessingResources.halfTexture);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.halfFBO);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.halfTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 960, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer::postProcessingResources.halfTexture, 0);
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
            satellite->setUniformVariables(renderer::uniformVariables.uniformSpecularIntensityPlanets, renderer::uniformVariables.uniformShininessPlanets);
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
        renderer::shaders.omniShadowShader->useShader();

        // Make the viewport the same dimenstions as the FBO
        glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

        // Bind our framebuffer so that the shader output goes to it
        // Every draw call after this will go to that framebuffer
        light->getShadowMap()->write();
        glClear(GL_DEPTH_BUFFER_BIT);

        // Get the uniformModel and lightTransform for the shader

        glUniform3fv(renderer::uniformVariables.uniformOmniLightPos, 1, glm::value_ptr(light->getPosition()));
        glUniform1f(renderer::uniformVariables.uniformFarPlane, light->getFarPlane());
        renderer::shaders.omniShadowShader->setLightMatrices(light->calculateLightTransform());

        renderer::shaders.omniShadowShader->validate();

        // Draw just to the depth buffer
        renderObjectsVector(scene::satellites, renderer::uniformVariables.uniformModelOmniShadowMap);

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

        renderer::shaders.sunShader->useShader();
        
        // Apply view matrix.
        // View matrix represents the camera's position and orientation in world.
        // The world is actually rotated around the camera with the view matrix. The camera is stationary.
        glUniformMatrix4fv(renderer::uniformVariables.uniformViewSuns, 1, GL_FALSE, glm::value_ptr(view));

        renderObjectsVector(scene::stars, renderer::uniformVariables.uniformModelSuns);

        // =================================================
        // RENDER PLANETS, MOONS, ASTEROIDS, and the SKYBOX
        // =================================================

        renderer::shaders.mainShader->useShader();
        renderer::shaders.mainShader->setSpotLightDirAndPos(camera::spotLight, shadowsEnabled, 4+scene::pointLightCount, scene::pointLightCount);

        // Eye position is for specular lighting
        glUniform3fv(renderer::uniformVariables.uniformEyePositionPlanets, 1, glm::value_ptr(camera::position));
        glUniformMatrix4fv(renderer::uniformVariables.uniformViewPlanets, 1, GL_FALSE, glm::value_ptr(view));

        renderObjectsVector(scene::satellites, renderer::uniformVariables.uniformModelPlanets);

        // ==============
        // RENDER SKYBOX
        // ==============
        skybox::drawSkybox(view);
    }

    void handleBloom()
    {
        // Half the bloom texture size before the pingPongFBOs can actually start bluring it
        glViewport(0, 0, 960, 600);

        renderer::shaders.halfShader->useShader();
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.halfFBO);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.textureToBlur);
        renderer::postProcessingResources.framebufferQuad->render();

        // Ping-pong bloom effect. Performs horiztonal and vertical bluring.
        // First iteration of the bloom effect. This means we don't need if conditions in the for loop
        bool horizontal {false};
        renderer::shaders.bloomShader->useShader();
        glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.pingPongFBO[!horizontal]);
        glUniform1i(renderer::uniformVariables.uniformHorizontal, !horizontal);
        glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.halfTexture);
        renderer::postProcessingResources.framebufferQuad->render();

        const int amount {4};
        for (unsigned int i {0}; i < amount; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, renderer::postProcessingResources.pingPongFBO[horizontal]);
            glUniform1i(renderer::uniformVariables.uniformHorizontal, horizontal);

            // If it's the first iteration, we want data from the bloom texture (the texture with the bright points)
            // Move the data between pingPong FBOs
            glBindTexture(GL_TEXTURE_2D, renderer::postProcessingResources.pingPongBuffer[!horizontal]);
            renderer::postProcessingResources.framebufferQuad->render();
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
        renderer::shaders.hdrShader->useShader();
        renderer::postProcessingResources.framebufferQuad->render();
    }
}

namespace renderer
{
    PostProcessingResources postProcessingResources {};
    UniformVariables uniformVariables {};
    Shaders shaders {};
    bool shadowsEnabled {false};

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
