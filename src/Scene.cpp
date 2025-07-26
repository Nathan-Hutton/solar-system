#include "Scene.h"

#include <utility>
#include <fstream>
#include <iostream>

#include "ResourceManager.h"
#include "OrbitalPhysics.h"
#include "Material.h"
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include "SpaceObject.h"
#include <nlohmann/json.hpp>

namespace 
{
    // This method will set old positions for all movables in case we're using Verlet numerical integration
    void setOldPositions()
    {
        for (std::shared_ptr<SpaceObject>& satellite1 : scene::movables)
        {
            glm::vec3 force {0};
            
            // Add up forces for other movables
            for (const std::shared_ptr<SpaceObject>&  satellite2 : scene::movables)
            {
                if (satellite1 == satellite2) continue;
                force += orbitalPhysics::getForce(satellite1.get(), satellite2.get());
            }

			const glm::vec3 acceleration{ force / satellite1->getMass() };
            const glm::vec3 velocity{ satellite1->getVelocity() + acceleration * 0.005f };
            satellite1->setOldPosition(satellite1->getPosition() - velocity * 0.005f);
        }
    }
}

std::vector<std::shared_ptr<SpaceObject>> scene::movables {};
std::vector<std::shared_ptr<SpaceObject>> scene::lightEmitters {};
std::vector<std::shared_ptr<SpaceObject>> scene::litObjects {};
std::array<std::shared_ptr<PointLight>, scene::MAX_POINT_LIGHTS> scene::pointLights;
GLint scene::pointLightCount{};

void scene::readSceneJson(std::string filePath)
{
	std::ifstream file{ filePath };
	if (!file.good())
	{
		std::cerr << "Json file path invalid: " << filePath << '\n';
		std::exit(EXIT_FAILURE);
	}

	nlohmann::json sceneData;
	file >> sceneData;

	// TODO: When I incorporate ImGui, just have some default camera if we don't
	// include one in the JSON and let the user adjust it with the gui
	if (!sceneData.contains("camera"))
	{
		std::cerr << "\nJson file is missing camera\n\n";
		std::exit(EXIT_FAILURE);
	}

	if (!sceneData.contains("objects") || !sceneData["objects"].is_array() || sceneData["objects"].empty())
	{
		std::cerr << "\nJson file is missing objects\n\n";
		std::exit(EXIT_FAILURE);
	}

	camera::jsonSetup(sceneData);

	for (size_t i{ 0 }; i < sceneData["objects"].size(); ++i)
	{
		const nlohmann::json& object = sceneData["objects"][i];

		if (!object.contains("mesh"))
		{
			std::cerr << "\nJson file object " << i << " is missing mesh info\n\n";
			std::exit(EXIT_FAILURE);
		}

		if (!object["mesh"].contains("type"))
		{
			std::cerr << "\nJson file object " << i << " is missing mesh type\n\n";
			std::exit(EXIT_FAILURE);
		}

		std::shared_ptr<SpaceObject> spaceObject;

		glm::vec3 position{ 0.0f };
		if (object.contains("position") && object["position"].is_array() && object["position"].size() == 3)
		{
			position.x = object["position"][0];
			position.y = object["position"][1];
			position.z = object["position"][2];
		}
		else
			std::cerr << "Object " << i << " is missing position info. Using defaults\n";

		glm::vec3 velocity{ 0.0f };
		if (object.contains("velocity") && object["velocity"].is_array() && object["velocity"].size() == 3)
		{
			velocity.x = object["velocity"][0];
			velocity.y = object["velocity"][1];
			velocity.z = object["velocity"][2];
		}
		else
			std::cerr << "Object " << i << " is missing velocity info. Using defaults\n";

		float mass{ 4.0f };
		if (object.contains("mass"))
			mass = object["mass"];
		else
			std::cerr << "Object " << i << " mass info missing. Using defaults\n";


		glm::vec3 rotationVector{ 0.0f };
		float rotationSpeed{ 0.0f };
		float angle{ 0.0f };
		if (object.contains("rotation"))
		{
			nlohmann::json rotation = object["rotation"];

			if (rotation.contains("rotationVector") && rotation["rotationVector"].is_array() && rotation["rotationVector"].size() == 3)
			{
				rotationVector.x = rotation["rotationVector"][0];
				rotationVector.y = rotation["rotationVector"][1];
				rotationVector.z = rotation["rotationVector"][2];
			}
			else
				std::cerr << "Object " << i << " is missing rotation vector info. Using default\n";

			if (rotation.contains("rotationSpeed"))
				rotationSpeed = rotation["rotationSpeed"];
			else
				std::cerr << "Object " << i << " is missing rotation speed info. Using default\n";

			if (rotation.contains("angle"))
				angle = rotation["angle"];
			else
				std::cerr << "Object " << i << " is missing angle info. Using default\n";
		}
		else
			std::cerr << "Object " << i << " rotation info missing. Using defaults\n";

		std::shared_ptr<Texture> texture;
		if (object.contains("texture"))
		{
			const std::string textureFilePath{ object["texture"] };
			if (!resourceManager::textureExists(textureFilePath))
				resourceManager::loadTextureIntoCache(textureFilePath);

			texture = resourceManager::getTexture(textureFilePath);
		}
		else
		{
			std::cerr << "Object " << i << " texture info missing. Using default\n";
			if (!resourceManager::textureExists("../assets/textures/plain.png"))
				resourceManager::loadTextureIntoCache("../assets/textures/plain.png");

			texture = resourceManager::getTexture("../assets/textures/plain.png");
		}

		bool objectGlows{ false };
		if (!object.contains("objectGlows"))
			std::cerr << "Object " << i << " is missing objectGlows boolean. Defaulting to false.\n";
		else
			objectGlows = object["objectGlows"];

		const nlohmann::json& meshInfo = object["mesh"];
		std::shared_ptr<Mesh> mesh;
		float collisionDistance{};
		if (meshInfo["type"] == "sphere")
		{
			float radius{ 1.0f };
			if (meshInfo.contains("radius"))
				radius = meshInfo["radius"];
			else
				std::cerr << "Object " << i << " sphere mesh radius info missing. Using defaults\n";
			collisionDistance = radius;

			int stacks{ 20 };
			int slices{ 20 };
			if (meshInfo.contains("resolution"))
			{
				if (meshInfo["resolution"].contains("stacks"))
					stacks = meshInfo["resolution"]["stacks"];
				else
					std::cerr << "Object " << i << " sphere mesh stacks info missing. Using defaults\n";

				if (meshInfo["resolution"].contains("slices"))
					slices = meshInfo["resolution"]["slices"];
				else
					std::cerr << "Object " << i << " sphere mesh slices info missing. Using defaults\n";
			}
			else
				std::cerr << "Object " << i << " sphere mesh resolution info missing. Using defaults\n";

			const bool hasNormals{ objectGlows ? false : true };
			if (!resourceManager::sphereMeshExists(radius, stacks, slices, hasNormals))
				resourceManager::loadSphereMeshIntoCache(radius, stacks, slices, hasNormals);

			mesh = resourceManager::getSphereMesh(radius, stacks, slices, hasNormals);
		}
		else if (meshInfo["type"] == "file")
		{
			if (!meshInfo.contains("filePath"))
			{
				std::cerr << "\nJson file object " << i << " mesh file path missing\n\n";
				std::exit(EXIT_FAILURE);
			}
			const std::string filePath{ meshInfo["filePath"] };

			float scaleFactor{ 1.0f };
			if (meshInfo.contains("scaleFactor"))
				scaleFactor = meshInfo["scaleFactor"];
			else
				std::cerr << "Object " << i << " mesh is from an obj file and is missing a scaleFactor. Using default (1.0f)\n\n";

			if (!resourceManager::fileMeshExists(filePath))
				resourceManager::loadFileMeshIntoCache(filePath, scaleFactor);

			mesh = resourceManager::getFileMesh(filePath);
		}
		else
		{
			std::cerr << "\nJson file object " << i << " mesh type not valid: " << meshInfo["type"] << "\n\n";
			std::exit(EXIT_FAILURE);
		}

		std::shared_ptr<PointLight> pLight{ nullptr };
		if (object.contains("pointLight"))
		{
			if (!objectGlows)
				std::cerr << "Object " << i << " contains pointLight but has objectGlows bool set to false\n";

			const nlohmann::json& pointLight = object["pointLight"];

			int shadowWidth{ 1024 };
			if (pointLight.contains("shadowWidth"))
				shadowWidth = pointLight["shadowWidth"];
			else
				std::cerr << "Object " << i << " point light shadowWidth missing. Using default value\n";

			int shadowHeight{ 1024 };
			if (pointLight.contains("shadowHeight"))
				shadowHeight = pointLight["shadowHeight"];
			else
				std::cerr << "Object " << i << " point light shadowHeight missing. Using default value\n";

			float near{ 0.01f };
			if (pointLight.contains("near"))
				near = pointLight["near"];
			else
				std::cerr << "Object " << i << " point light near missing. Using default value\n";

			float far{ 100.0f };
			if (pointLight.contains("far"))
				far = pointLight["far"];
			else
				std::cerr << "Object " << i << " point light far missing. Using default value\n";

			float ambientIntensity{ 5.0f };
			if (pointLight.contains("ambientIntensity"))
				ambientIntensity = pointLight["ambientIntensity"];
			else
				std::cerr << "Object " << i << " point light ambientIntensity missing. Using default value\n";

			float diffuseIntensity{ 300.0f };
			if (pointLight.contains("diffuseIntensity"))
				diffuseIntensity = pointLight["diffuseIntensity"];
			else
				std::cerr << "Object " << i << " point light diffuseIntensity missing. Using default value\n";

			glm::vec3 attenuation{ 1.0f };
			if (pointLight.contains("attenuation") && pointLight["attenuation"].is_array() && pointLight["attenuation"].size() == 3)
			{
				attenuation.x = pointLight["attenuation"][0];
				attenuation.y = pointLight["attenuation"][1];
				attenuation.z = pointLight["attenuation"][2];
			}

			else
				std::cerr << "Object " << i << " point light attenuation missing. Using default value\n";

			glm::vec3 color{ 1.0f };
			if (pointLight.contains("color") && pointLight["color"].is_array() && pointLight["color"].size() == 3)
			{
				color.x = pointLight["color"][0];
				color.y = pointLight["color"][1];
				color.z = pointLight["color"][2];
			}
			else
				std::cerr << "Object " << i << " point light color missing. Using default value\n";

			pLight = std::make_shared<PointLight>(
				static_cast<GLuint>(shadowWidth), static_cast<GLuint>(shadowHeight),
				near, far,
				color,
				ambientIntensity, diffuseIntensity,
				position,
				attenuation[0], attenuation[1], attenuation[2]
			);

			pointLights[pointLightCount++] = pLight;
		}

		std::shared_ptr<Material> material{ nullptr };
		if (object.contains("material"))
		{
			if (objectGlows)
				std::cerr << "Object " << i << " objectGlows bool set to true but contains a material\n";

			if (object.contains("material"))
			{
				const std::string materialFilePath{ object["material"] };
				if (!resourceManager::materialExists(materialFilePath))
					resourceManager::loadMaterialIntoCache(materialFilePath);

				material = resourceManager::getMaterial(materialFilePath);
			}
			else
			{
				std::cerr << "Object " << i << " objectGlows flag set to false but material info is missing. Using default\n";
				if (!resourceManager::materialExists("../assets/materials/planetMaterial.json"))
					resourceManager::loadMaterialIntoCache("../assets/materials/planetMaterial.json");

				material = resourceManager::getMaterial("../assets/materials/planetMaterial.json");
			}
		}

		spaceObject = std::make_shared<SpaceObject>(mass, objectGlows, mesh, texture, material, pLight);
		spaceObject->setPosition(position);
		spaceObject->setVelocity(velocity);
		spaceObject->setRotation(rotationVector);
		spaceObject->setAngle(angle);
		spaceObject->setRotationSpeed(rotationSpeed);
		spaceObject->setCollisionDistance(collisionDistance);

		if (objectGlows)
			lightEmitters.push_back(spaceObject);
		else
			litObjects.push_back(spaceObject);

		movables.push_back(spaceObject);
	}

    if (orbitalPhysics::verlet)
        setOldPositions();
}

void scene::setupSkybox()
{
    std::array<std::string, 6> skyboxFaces {};
    skyboxFaces[0] = "../assets/textures/skybox/rightImage.png";
    skyboxFaces[1] = "../assets/textures/skybox/leftImage.png";
    skyboxFaces[2] = "../assets/textures/skybox/upImage.png";
    skyboxFaces[3] = "../assets/textures/skybox/downImage.png";
    skyboxFaces[4] = "../assets/textures/skybox/frontImage.png";
    skyboxFaces[5] = "../assets/textures/skybox/frontImage.png";
    skybox::setup(skyboxFaces);
}

