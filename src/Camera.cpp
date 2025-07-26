#include "Camera.h"

#include <iostream>
#include <glm/gtc/quaternion.hpp>

namespace
{
    glm::vec3 worldUp {glm::vec3{0.0f, 1.0f, 0.0f}};
    GLfloat yaw {-90.0f};
    GLfloat pitch {0.0f};
    GLfloat roll {0.0f};
    
    glm::vec3 front {glm::normalize(glm::vec3{0.0f, 0.0f, -1.0f})};
    glm::vec3 right {glm::normalize(glm::cross(front, worldUp))};
    glm::vec3 up {glm::normalize(glm::cross(right, front))};

    GLfloat oldYaw {-90.0f};
    GLfloat oldRoll {0.0f};
    GLfloat oldPitch {0.0f};

    void handleFlashlightKey(bool* keys)
    {
        // Check for flashlight toggle
        if (!keys[GLFW_KEY_F])
            return;

        // Setting this to false means we won't trigger it multiple times when we press it once
        keys[GLFW_KEY_F] = false;

        // If flashlight is disabled, don't put it in the shader (it's the last spotLight in our array)
        camera::spotLight->toggle();
    }

    void update()
    {
        // TODO: Uncomment this code and make quaternions work. This will resolve the issue where shaking the mouse makes the camera orientation stack up with floating-point errors
        //// Step 1: Create a quaternion from the current orientation
        //glm::quat orientation = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));
        //
        //// Step 2: Create quaternions for the yaw, pitch, and roll changes
        //glm::quat yawQuat = glm::angleAxis(glm::radians(yaw - oldYaw), up);
        //glm::quat pitchQuat = glm::angleAxis(glm::radians(pitch - oldPitch), right);
        //glm::quat rollQuat = glm::angleAxis(glm::radians(roll - oldRoll), front);
        //
        //// Combine the quaternions
        //orientation = yawQuat * orientation;
        //orientation = pitchQuat * orientation;
        //orientation = rollQuat * orientation;
        //
        //// Normalize the quaternion to avoid drift
        //orientation = glm::normalize(orientation);

        //// Update the old angles
        //oldYaw = yaw;
        //oldPitch = pitch;
        //oldRoll = roll;
        //
        //// Step 3: Convert the quaternion back to direction vectors
        //front = -glm::normalize(orientation * glm::vec3(0.0f, 0.0f, -1.0f));
        //up = -glm::normalize(orientation * glm::vec3(0.0f, 1.0f, 0.0f));
        //right = glm::normalize(glm::cross(front, up));

        // Interpolate yaw, pitch, and roll with damping
        float dampingFactor = 0.07f; // Adjust this value for more or less damping
        float dampedYaw = oldYaw + dampingFactor * (yaw - oldYaw);
        float dampedPitch = oldPitch + dampingFactor * (pitch - oldPitch);
        float dampedRoll = oldRoll + dampingFactor * (roll - oldRoll);

        const glm::mat4 yawMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(oldYaw - dampedYaw), up)};
        front       = glm::normalize(glm::vec3{yawMatrix * glm::vec4{front, 0.0f}});
        oldYaw      = dampedYaw;
        right       = glm::normalize(glm::cross(front, up));

        glm::mat4 pitchMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(dampedPitch - oldPitch), right)};
        front       = glm::normalize(glm::vec3{pitchMatrix * glm::vec4{front, 0.0f}});
        oldPitch    = dampedPitch;
        up          = glm::normalize(glm::cross(right, front));

        glm::mat4 rollMatrix {glm::rotate(glm::mat4{1.0f}, glm::radians(dampedRoll - oldRoll), front)};
        up          = glm::normalize(glm::vec3{rollMatrix * glm::vec4{up, 0.0f}});
        oldRoll     = dampedRoll;
        right       = glm::normalize(glm::cross(front, up));

        // Update the flashlight position and direction
        camera::spotLight->setFlash(camera::position, front);
    }
}

glm::vec3 camera::position {glm::vec3{0.0f}};
GLfloat camera::moveSpeed {5.0f};
GLfloat camera::turnSpeed {1.0f};

std::shared_ptr<SpotLight> camera::spotLight {};

void camera::jsonSetup(nlohmann::json sceneData)
{
	const nlohmann::json& cam = sceneData["camera"];

	if (cam.contains("position") && cam["position"].is_array() && cam["position"].size() == 3)
	{
		const float cameraX{ cam["position"][0] };
		const float cameraY{ cam["position"][1] };
		const float cameraZ{ cam["position"][2] };
		camera::position = glm::vec3(cameraX, cameraY, cameraZ);
	}
	else
	{
		std::cerr << "\nJson file camera is missing position. Using default value.\n\n";
		camera::position = glm::vec3(0.0f, 0.0f, 50.0f);
	}

	if (cam.contains("moveSpeed"))
		camera::moveSpeed = cam["moveSpeed"];
	else
	{
		std::cerr << "\nJson file camera is missing move speed. Using default value.\n\n";
		camera::moveSpeed = 10.0f;
	}

	if (cam.contains("turnSpeed"))
		camera::turnSpeed = cam["turnSpeed"];
	else
	{
		std::cerr << "\nJson file camera is missing turn speed. Using default values.\n\n";
		camera::turnSpeed = 0.3f;
	}

	if (cam.contains("spotLight"))
	{
		const nlohmann::json& spotLight = cam["spotLight"];

		int shadowWidth{ 1024 };
		if (spotLight.contains("shadowWidth"))
			shadowWidth = spotLight["shadowWidth"];
		else
			std::cerr << "Json file camera is missing spotlight shadowWidth. Using default value\n";

		int shadowHeight{ 1024 };
		if (spotLight.contains("shadowHeight"))
			shadowHeight = spotLight["shadowHeight"];
		else
			std::cerr << "Json file camera is missing spotlight shadowHeight. Using default value\n";

		float near{ 0.01f };
		if (spotLight.contains("near"))
			near = spotLight["near"];
		else
			std::cerr << "Json file camera is missing spotlight near. Using default value\n";

		float far{ 0.01f };
		if (spotLight.contains("far"))
			far = spotLight["far"];
		else
			std::cerr << "Json file camera is missing spotlight far. Using default value\n";

		float ambientIntensity{ 0.0f };
		if (spotLight.contains("ambientIntensity"))
			ambientIntensity = spotLight["ambientIntensity"];
		else
			std::cerr << "Json file camera is missing spotlight ambientIntensity. Using default value\n";

		float diffuseIntensity{ 10.0f };
		if (spotLight.contains("diffuseIntensity"))
			diffuseIntensity = spotLight["diffuseIntensity"];
		else
			std::cerr << "Json file camera is missing spotlight diffuseIntensity. Using default value\n";

		float edge{ 20.0f };
		if (spotLight.contains("edge"))
			edge = spotLight["edge"];
		else
			std::cerr << "Json file camera is missing spotlight edge. Using default value\n";

		glm::vec3 color{ 1.0f };
		if (spotLight.contains("color") && spotLight["color"].is_array() && spotLight["color"].size() == 3)
		{
			color.x = spotLight["color"][0];
			color.y = spotLight["color"][1];
			color.z = spotLight["color"][2];
		}
		else
			std::cerr << "Json file camera is missing spotlight color. Using default value\n";

		glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
		if (spotLight.contains("direction") && spotLight["direction"].is_array() && spotLight["direction"].size() == 3)
		{
			direction.x = spotLight["direction"][0];
			direction.y = spotLight["direction"][1];
			direction.z = spotLight["direction"][2];
		}
		else
			std::cerr << "Json file camera is missing spotlight direction. Using default value\n";

		glm::vec3 attenuation{ 0.1f, 0.1f, 0.5f };
		if (spotLight.contains("attenuation") && spotLight["attenuation"].is_array() && spotLight["attenuation"].size() == 3)
		{
			attenuation[0] = spotLight["attenuation"][0];
			attenuation[1] = spotLight["attenuation"][1];
			attenuation[2] = spotLight["attenuation"][2];
		}
		else
			std::cerr << "Json file camera is missing spotlight attenuation. Using default value\n";

		setSpotLight(shadowWidth, shadowHeight, 
			 near, far,
			 color,
			 ambientIntensity, diffuseIntensity,
			 camera::position,
			 direction,
			 attenuation[0], attenuation[1], attenuation[2],
			 edge
		);
	}
	else
	{
		std::cerr << "\nJson file camera is missing spotlight. Using default values.\n\n";

		constexpr int shadowWidth{ 1024 };
		constexpr int shadowHeight{ 1024 };
		constexpr float near{ 0.01f };
		constexpr float far{ 100.0f };
		constexpr glm::vec3 color{ 1.0f, 1.0f, 1.0f };
		constexpr float ambientIntensity{ 0.0f };
		constexpr float diffuseIntensity{ 0.0f };
		constexpr glm::vec3 direction{ 0.0f, -1.0f, 0.0f };
		constexpr glm::vec3 attenuation{ 0.1f, 0.1f, 0.5f };
		constexpr float edge{ 20.0f };

		camera::setSpotLight(shadowWidth, shadowHeight, 
			 near, far,
			 color,
			 ambientIntensity, diffuseIntensity,
			 camera::position,
			 direction,
			 attenuation[0], attenuation[1], attenuation[2],
			 edge
		);
	}
}

void camera::keyControl(bool* keys, GLfloat deltaTime)
{
    const GLfloat velocity {moveSpeed * deltaTime};

    if (keys[GLFW_KEY_W])
        position += front * velocity;
    if (keys[GLFW_KEY_A])
        position -= right * velocity;
    if (keys[GLFW_KEY_S])
        position -= front * velocity;
    if (keys[GLFW_KEY_D])
        position += right * velocity;

    // Up and down
    if (keys[GLFW_KEY_LEFT_SHIFT])
        position -= up * velocity;
    if (keys[GLFW_KEY_SPACE])
        position += up * velocity;

    // Roll
    if (keys[GLFW_KEY_E])
        roll += turnSpeed * 3;
    if (keys[GLFW_KEY_Q])
        roll -= turnSpeed * 3;

    handleFlashlightKey(keys);
}

void camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    update();
}

void camera::setSpotLight(GLuint shadowWidth, GLuint shadowHeight,
                GLfloat near, GLfloat far,
				glm::vec3 color,
                GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                glm::vec3 position,
                glm::vec3 direction,
                GLfloat exponential, GLfloat linear, GLfloat constant,
                GLfloat edge)
{
    spotLight = std::make_unique<SpotLight>(shadowWidth, shadowHeight,
                        near, far,
						color,
                        ambientIntensity, diffuseIntensity,
                        position,
                        direction,
                        exponential, linear, constant,
                        edge);
}

void camera::calculateViewMatrix(glm::mat4& viewMatrix)
{
    // This creates a lookat matrix, which is a transformation matrix that positions and orients (transforms) 
    // objects to look at a target.
    
    // A view matrix is a type of lookat matrix that transforms coordinates (vertices) from world space to
    // view (camera) space. It effectively moves and rotates the entire world so the camera is at the origin 
    // of the view space looking down the negative z axis. Can also imagine the coordinate space is being
    // manipulated instead of the vertices themselves to achieve the same effect.
    // The view space axes are the up, front, and right vectors. The view matrix transforms the scene to
    // align with these axes, enabling the rendering of the scene from the camera's perspective.
    // The projection defines how this view matrix is projected onto the 2D screen.
    
    // Args are where the camera is, what the camera is looking at, and what its up is
    viewMatrix = glm::lookAt(position, position + front, up);
}
