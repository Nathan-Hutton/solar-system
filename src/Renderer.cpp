#include "Renderer.h"

#include <stdexcept>
#include <array>
#include <memory>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Skybox.h"
#include "Scene.h"
#include "Camera.h"
#include "Window.h"
#include "ShaderCreator.h"

namespace
{
    struct {
        GLuint halfFBO {};
        GLuint halfTexture {};
        GLuint postProcessingFBO {};
        GLuint postProcessingTexture {};
        GLuint textureToBlur {};
        std::array<unsigned int, 2> pingPongFBO;;
        std::array<unsigned int, 2> pingPongBuffer;
        std::unique_ptr<Mesh> framebufferQuad {};
    } postProcessingResources;

    struct {
        GLuint uniformModelPlanets{};
        GLuint uniformModelToClipSpacePlanets{};
        GLuint uniformModelToClipSpaceSuns{};
        GLuint uniformEyePositionPlanets{};
        GLuint uniformSpecularIntensityPlanets{};
        GLuint uniformShininessPlanets{};
        GLuint uniformModelSuns{};
        GLuint uniformHorizontal{};
		GLuint uniformTextureSuns{};
		GLuint uniformBloomTexture{};
		GLuint uniformHalfTexture{};
    } uniformVariables;

	struct {
		GLuint shaderID{};
		GLuint uniformTexture{};
		struct {
			struct {
				GLuint uniformColor {};
				GLuint uniformAmbientIntensity {};
				GLuint uniformDiffuseIntensity {};
				
				GLuint uniformPosition {};
				GLuint uniformExponential {};
				GLuint uniformLinear {};
				GLuint uniformConstant {};
			} uniformPointLights[scene::MAX_POINT_LIGHTS];

			struct {
				GLuint uniformFlashLightOn{};
				GLuint uniformColor {};
				GLuint uniformAmbientIntensity {};
				GLuint uniformDiffuseIntensity {};
				
				GLuint uniformPosition {};
				GLuint uniformExponential {};
				GLuint uniformLinear {};
				GLuint uniformConstant {};

				GLuint uniformDirection {};
				GLuint uniformEdge {};
			} uniformSpotLight;
		} uniformLightingVariables;
	} mainShader;

    struct {
		GLuint sunShaderID{};
		GLuint hdrShaderID{};
		GLuint bloomShaderID{};
		GLuint halfShaderID{};
    } shaders;

    glm::mat4 g_projection {};
}

// Shader nonsense
namespace 
{
    void createShaders()
    {
		shaders.sunShaderID = shaderCreator::compileShader({"../assets/shaders/sunShader.vert", "../assets/shaders/sunShader.frag"});
        glUseProgram(shaders.sunShaderID);
		uniformVariables.uniformTextureSuns = glGetUniformLocation(shaders.sunShaderID, "theTexture");
        glUniform1i(uniformVariables.uniformTextureSuns, 2);
        uniformVariables.uniformModelSuns    = glGetUniformLocation(shaders.sunShaderID, "model");
        uniformVariables.uniformModelToClipSpaceSuns = glGetUniformLocation(shaders.sunShaderID, "modelToClipSpace");

		shaders.hdrShaderID = shaderCreator::compileShader({"../assets/shaders/hdrShader.vert", "../assets/shaders/hdrShader.frag"});
		glUseProgram(shaders.hdrShaderID);
        glUniform1i(glGetUniformLocation(shaders.hdrShaderID, "theTexture"), 0);
        glUniform1i(glGetUniformLocation(shaders.hdrShaderID, "blurTexture"), 1);

		shaders.bloomShaderID = shaderCreator::compileShader({"../assets/shaders/bloomShader.vert", "../assets/shaders/bloomShader.frag"});
		glUseProgram(shaders.bloomShaderID);
        glUniform1i(uniformVariables.uniformBloomTexture, 0);
        uniformVariables.uniformHorizontal = glGetUniformLocation(shaders.bloomShaderID, "horizontal");
        
		shaders.halfShaderID = shaderCreator::compileShader({"../assets/shaders/half.vert", "../assets/shaders/half.frag"});
		glUseProgram(shaders.halfShaderID);
		glUniform1i(uniformVariables.uniformHalfTexture, 0);

		// ***********
		// MAIN SHADER
		// ***********
		mainShader.shaderID = shaderCreator::compileShader({"../assets/shaders/planetShaderNoShadows.vert", "../assets/shaders/planetShaderNoShadows.frag"});
		glUseProgram(mainShader.shaderID);
		mainShader.uniformTexture = glGetUniformLocation(mainShader.shaderID, "theTexture");
		glUniform1i(mainShader.uniformTexture, 2);

		// Point lights
        std::stringstream ss {};
		for (size_t i {0}; i < scene::MAX_POINT_LIGHTS; ++i)
		{
			// Base color
			ss << "pointLights[" << i << "].base.color";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformColor = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Ambient intensity
			ss << "pointLights[" << i << "].base.ambientIntensity";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Diffuse intensity
			ss << "pointLights[" << i << "].base.diffuseIntensity";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Position
			ss << "pointLights[" << i << "].position";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformPosition = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Exponential
			ss << "pointLights[" << i << "].exponential";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformExponential = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Linear
			ss << "pointLights[" << i << "].linear";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformLinear = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags

			// Constant
			ss << "pointLights[" << i << "].constant";
			mainShader.uniformLightingVariables.uniformPointLights[i].uniformConstant = glGetUniformLocation(mainShader.shaderID, ss.str().c_str());
			ss.str(""); // Clear the buffer
			ss.clear(); // Clear error flags
		}
		
		// Spot light
		mainShader.uniformLightingVariables.uniformSpotLight.uniformFlashLightOn = glGetUniformLocation(mainShader.shaderID, "flashLightOn");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformColor = glGetUniformLocation(mainShader.shaderID, "spotLight.base.base.color");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformAmbientIntensity = glGetUniformLocation(mainShader.shaderID, "spotLight.base.base.ambientIntensity");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformDiffuseIntensity = glGetUniformLocation(mainShader.shaderID, "spotLight.base.base.diffuseIntensity");

		mainShader.uniformLightingVariables.uniformSpotLight.uniformPosition = glGetUniformLocation(mainShader.shaderID, "spotLight.base.position");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformExponential = glGetUniformLocation(mainShader.shaderID, "spotLight.base.exponential");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformLinear = glGetUniformLocation(mainShader.shaderID, "spotLight.base.linear");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformConstant = glGetUniformLocation(mainShader.shaderID, "spotLight.base.constant");

		mainShader.uniformLightingVariables.uniformSpotLight.uniformDirection = glGetUniformLocation(mainShader.shaderID, "spotLight.direction");
		mainShader.uniformLightingVariables.uniformSpotLight.uniformEdge = glGetUniformLocation(mainShader.shaderID, "spotLight.edge");

		glUniform1i(mainShader.uniformLightingVariables.uniformSpotLight.uniformFlashLightOn, camera::spotLight->isOn());
		camera::spotLight->useLight(
			mainShader.uniformLightingVariables.uniformSpotLight.uniformAmbientIntensity,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformDiffuseIntensity,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformColor, 
			mainShader.uniformLightingVariables.uniformSpotLight.uniformPosition, 
			mainShader.uniformLightingVariables.uniformSpotLight.uniformDirection,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformExponential,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformLinear,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformConstant,
			mainShader.uniformLightingVariables.uniformSpotLight.uniformEdge
		);

		for (int i{ 0 }; i < scene::pointLightCount; ++i)
		{
			scene::pointLights[i]->useLight(
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformAmbientIntensity,
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformDiffuseIntensity,
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformColor,
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformPosition, 
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformExponential, 
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformLinear,
				mainShader.uniformLightingVariables.uniformPointLights[i].uniformConstant
			);
		}

		glUniform1i(glGetUniformLocation(mainShader.shaderID, "pointLightCount"), scene::pointLightCount);

        uniformVariables.uniformModelPlanets                = glGetUniformLocation(mainShader.shaderID, "model");
        uniformVariables.uniformModelToClipSpacePlanets     = glGetUniformLocation(mainShader.shaderID, "modelToClipSpace");
        uniformVariables.uniformEyePositionPlanets          = glGetUniformLocation(mainShader.shaderID, "eyePosition");
        uniformVariables.uniformSpecularIntensityPlanets    = glGetUniformLocation(mainShader.shaderID, "material.specularIntensity");
        uniformVariables.uniformShininessPlanets            = glGetUniformLocation(mainShader.shaderID, "material.shininess");
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

		glUseProgram(shaders.hdrShaderID);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.postProcessingTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.pingPongBuffer[1]);
        glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(shaders.hdrShaderID, "theTexture"), 3);
        glUniform1i(glGetUniformLocation(shaders.hdrShaderID, "blurTexture"), 4);

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
}

// Called when renderering
namespace
{
    void renderObjectsVector(const std::vector<std::shared_ptr<SpaceObject>>& objects, GLuint uniformModel, GLuint uniformModelToClipSpace, const glm::mat4& worldToClip)
    {
        // They'll all use GL_TEXTURE2
        glActiveTexture(GL_TEXTURE2);
        for (const std::shared_ptr<SpaceObject>& object : objects)
        {
            glm::mat4 model {1.0f};
            object->setWorldProperties(model);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformModelToClipSpace, 1, GL_FALSE, glm::value_ptr(worldToClip * model));

			std::shared_ptr<Material> material{ object->getMaterial() };
			if (material != nullptr)
				material->useMaterial(uniformVariables.uniformSpecularIntensityPlanets, uniformVariables.uniformShininessPlanets);

            object->render();
        }
    }

    void renderAllObjects()
    {
        glm::mat4 view;
        camera::calculateViewMatrix(view);
        glm::mat4 worldToClip = g_projection * view;

		glUseProgram(shaders.sunShaderID);
        renderObjectsVector(scene::glowingObjects, uniformVariables.uniformModelSuns, uniformVariables.uniformModelToClipSpaceSuns, worldToClip);

		glUseProgram(mainShader.shaderID);
		glUniform1i(mainShader.uniformLightingVariables.uniformSpotLight.uniformFlashLightOn, camera::spotLight->isOn());

		if (!camera::spotLight->isOn())
		{
			camera::spotLight->setPosAndDir(mainShader.uniformLightingVariables.uniformSpotLight.uniformPosition, mainShader.uniformLightingVariables.uniformSpotLight.uniformDirection);

			glUniform3fv(uniformVariables.uniformEyePositionPlanets, 1, glm::value_ptr(camera::position));
		}

        renderObjectsVector(scene::shadedObjects, uniformVariables.uniformModelPlanets, uniformVariables.uniformModelToClipSpacePlanets, worldToClip);

        skybox::drawSkybox(view, g_projection);
    }

    void handleBloom()
    {
        // Half the bloom texture size before the pingPongFBOs can actually start bluring it
        glViewport(0, 0, window::windowWidth/2, window::windowHeight/2);

		glUseProgram(shaders.halfShaderID);
        glActiveTexture(GL_TEXTURE0);
        glBindFramebuffer(GL_FRAMEBUFFER, postProcessingResources.halfFBO);
        glBindTexture(GL_TEXTURE_2D, postProcessingResources.textureToBlur);
        postProcessingResources.framebufferQuad->render();

        // Ping-pong bloom effect. Performs horiztonal and vertical bluring.
        // First iteration of the bloom effect. This means we don't need if conditions in the for loop
        bool horizontal {false};
		glUseProgram(shaders.bloomShaderID);
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
		glUseProgram(shaders.hdrShaderID);
        postProcessingResources.framebufferQuad->render();
    }
}

void renderer::setup(const glm::mat4& projection)
{
    g_projection = projection;
    createShaders();
    setupPostProcessingObjects();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
