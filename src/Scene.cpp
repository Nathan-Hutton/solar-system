#include "Scene.h"

#include <utility>
#include <fstream>
#include <iostream>

#include "ResourceManager.h"
#include "OrbitalPhysics.h"
#include "Sun.h"
#include "Planet.h"
#include "Model.h"
#include "Material.h"
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"
#include <nlohmann/json.hpp>

namespace 
{
    // This method will set old positions for all movables in case we're using Verlet numerical integration
    void setOldPositions()
    {
        for (std::unique_ptr<SpaceObject>& satellite1 : scene::movables)
        {
            glm::vec3 force {0};
            
            // Add up forces for other movables
            for (const std::unique_ptr<SpaceObject>&  satellite2 : scene::movables)
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

std::vector<std::unique_ptr<SpaceObject>> scene::movables {};
std::vector<SpaceObject*> scene::stars {};
std::vector<SpaceObject*> scene::nonStars {};
std::array<PointLight*, scene::MAX_POINT_LIGHTS> scene::pointLights;
GLint scene::pointLightCount {};

void scene::readSceneJson(std::string filePath)
{
	std::ifstream file{ filePath };
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

	for (const nlohmann::json& object : sceneData["objects"])
	{
		if (!object.contains("type"))
		{
			std::cerr << "\nJson file is object is missing a type\n\n";
			std::exit(EXIT_FAILURE);
		}

		if (object["type"] == "sun")
		{
			continue;
		}
	}
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

void scene::createObjects1Sun1Planet()
{
	readSceneJson("../jsonScenes/1planet1sun.json");

	resourceManager::loadTextureIntoCache("../assets/textures/sun.jpg");
	resourceManager::loadTextureIntoCache("../assets/textures/earth.jpg");
	resourceManager::loadMaterialIntoCache("../assets/materials/planetMaterial.json");

    std::unique_ptr<Sun> sun { std::make_unique<Sun>(525.0f, 5.0f, 25, 25) };
    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
    sun->setTexturePointer(resourceManager::getTexture("../assets/textures/sun.jpg"));
    sun->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 5.0f, 300.0f, 0.1f, 0.1f, 0.01f);
    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    pointLights[pointLightCount++] = sun->getPointLight();
	Sun* sunPtr{ sun.get() };
    movables.push_back(std::move(sun));
	stars.push_back(sunPtr);

    std::unique_ptr<Planet> planet { std::make_unique<Planet>(4.0f, resourceManager::getMaterial("../assets/materials/planetMaterial.json"), 1.0f, 20, 20) };
    planet->setTexturePointer(resourceManager::getTexture("../assets/textures/earth.jpg"));
    planet->setPosition(glm::vec3{25.5f, 0.0f, -2.5f});
    planet->setVelocity(glm::vec3{0.0f, 35.0f, 0.0f});
    planet->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    planet->setRotationSpeed(100.0f);
	Planet* planetPtr{ planet.get() };
    movables.push_back(std::move(planet));
	nonStars.push_back(planetPtr);

    if (orbitalPhysics::verlet)
        setOldPositions();
}

void scene::createObjectsDefault(){}
void scene::createObjectsFigureEight(){}
void scene::createObjectsFancy(){}

//void scene::createObjectsDefault()
//{
//    camera::position  = glm::vec3{0.0f, 0.0f, 110.0f};
//    camera::moveSpeed = 25.0f;
//    camera::turnSpeed = 0.3f;
//    camera::setSpotLight(1024, 1024, 
//                         0.01f, 100.0f,
//                         1.0f, 1.0f, 1.0f,
//                         0.0f, 10.0f,
//                         camera::position,
//                         glm::vec3(0.0f, -1.0f, 0.0f),
//                         0.1f, 0.1f, 0.5f,
//                         20.0f);
//
//    std::shared_ptr<Texture> sunTexture { std::make_shared<Texture>("../assets/textures/sun.jpg") };
//    sunTexture->loadTexture();
//    std::shared_ptr<Texture> earthTexture { std::make_shared<Texture>("../assets/textures/earth.jpg") };
//    earthTexture->loadTexture();
//    std::shared_ptr<Texture> marsTexture { std::make_shared<Texture>("../assets/textures/mars.jpg") };
//    marsTexture->loadTexture();
//    std::shared_ptr<Texture> moonTexture { std::make_shared<Texture>("../assets/textures/moon.jpg") };
//    moonTexture->loadTexture();
//
//    std::shared_ptr<Material> material { std::make_shared<Material>(0.0f, 0) };
//
//    std::unique_ptr<Sun> sun { std::make_unique<Sun>(429.3f, 7.0f, 25, 25) };
//    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
//    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
//    sun->setTexturePointer(sunTexture);
//    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
//    sun->setAngle(90.0f);
//    sun->setRotationSpeed(-25.0f);
//    pointLights[pointLightCount++] = sun->getPointLight();
//	Sun* sunPtr{ sun.get() };
//	movables.push_back(std::move(sun));
//	stars.push_back(sunPtr);
//
//    std::unique_ptr<Planet> planet { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    planet->setTexturePointer(earthTexture);
//    planet->setPosition(glm::vec3{0.0f, -15.0f, -2.5f});
//    planet->setVelocity(glm::vec3{-55.0f, 0.0f, 0.0f});
//    planet->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
//    planet->setRotationSpeed(100.0f);
//	Planet* planetPtr{ planet.get() };
//    movables.push_back(std::move(planet));
//	nonStars.push_back(planetPtr);
//
//    std::unique_ptr<Planet> planet1 { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    planet1->setTexturePointer(marsTexture);
//    planet1->setPosition(glm::vec3{45.0f, 0.0f, -2.5f});
//    planet1->setVelocity(glm::vec3{0.0f, -32.0f, 0.0f});
//    planet1->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
//    planet1->setRotationSpeed(-100.0f);
//	Planet* planet1Ptr{ planet1.get() };
//    movables.push_back(std::move(planet1));
//	nonStars.push_back(planet1Ptr);
//
//    std::unique_ptr<Moon> moon { std::make_unique<Planet>(1.25f, material, 0.5f, 20, 20) };
//    moon->setTexturePointer(moonTexture);
//    moon->setPosition(glm::vec3{42.0f, 0.0f, -2.5f});
//    moon->setVelocity(glm::vec3{-3.0f, -45.0f, 0.0f});
//    moon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
//    moon->setRotationSpeed(200.0f);
//	Moon* moonPtr{ moon.get() };
//    movables.push_back(std::move(moon));
//	nonStars.push_back(moonPtr);
//
//    std::unique_ptr<Model> asteroid { std::make_unique<Model>(0.25f, material) };
//    asteroid->loadModel("../assets/models/asteroid.obj");
//    asteroid->setPosition(glm::vec3{-44.0f, 0.0f, -2.5f});
//    asteroid->setVelocity(glm::vec3{-3.0f, 27.0f, 0.0f});
//    asteroid->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
//    asteroid->setRotationSpeed(200.0f);
//    asteroid->setScaleFactor(0.5f);
//	Model* asteroidPtr{ asteroid.get() };
//    movables.push_back(std::move(asteroid));
//	nonStars.push_back(asteroidPtr);
//
//    if (orbitalPhysics::verlet)
//        setOldPositions();
//}
//
//void scene::createObjectsFigureEight()
//{
//    camera::position  = glm::vec3{0.0f, 0.0f, 50.0f};
//    camera::moveSpeed = 10.0f;
//    camera::turnSpeed = 0.3f;
//    camera::setSpotLight(1024, 1024, 
//                         0.01f, 100.0f,
//                         1.0f, 1.0f, 1.0f,
//                         0.0f, 10.0f,
//                         camera::position,
//                         glm::vec3(0.0f, -1.0f, 0.0f),
//                         0.1f, 0.1f, 0.5f,
//                         20.0f);
//
//    std::shared_ptr<Texture> sunTexture { std::make_shared<Texture>("../assets/textures/sun.jpg") };
//    sunTexture->loadTexture();
//    std::shared_ptr<Texture> earthTexture { std::make_shared<Texture>("../assets/textures/earth.jpg") };
//    earthTexture->loadTexture();
//
//    std::shared_ptr<Material> material { std::make_shared<Material>(0.0f, 0) };
//
//    std::unique_ptr<Sun> sun1 { std::make_unique<Sun>(67.0f, 2.0f, 25, 25) };
//    sun1->setPosition(glm::vec3{-15.0f, 0.0f, -2.5f});
//    sun1->setTexturePointer(sunTexture);
//    sun1->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
//    sun1->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
//    sun1->setAngle(90.0f);
//    sun1->setRotationSpeed(-25.2f);
//    pointLights[pointLightCount++] = sun1->getPointLight();
//	Sun* sunPtr1{ sun1.get() };
//	movables.push_back(std::move(sun1));
//    stars.push_back(sunPtr1);
//
//    std::unique_ptr<Sun> sun2 { std::make_unique<Sun>(67.0f, 2.0f, 25, 25) };
//    sun2->setPosition(glm::vec3{15.0f, 0.0f, -2.5f});
//    sun2->setTexturePointer(sunTexture);
//    sun2->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
//    sun2->setRotation(glm::vec3{0.0f, 1.0f, 1.0f});
//    sun2->setAngle(90.0f);
//    sun2->setRotationSpeed(25.0f);
//    pointLights[pointLightCount++] = sun2->getPointLight();
//	Sun* sunPtr2{ sun2.get() };
//	movables.push_back(std::move(sun2));
//    stars.push_back(sunPtr2);
//
//    std::unique_ptr planet { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    planet->setTexturePointer(earthTexture);
//    planet->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
//    planet->setVelocity(glm::vec3{17.0f, 24.675f, 0.0f});
//    planet->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
//    planet->setRotationSpeed(100.0f);
//	Planet* planetPtr{ planet.get() };
//    movables.push_back(std::move(planet));
//	nonStars.push_back(planetPtr);
//
//    if (orbitalPhysics::verlet)
//        setOldPositions();
//}
//
//void scene::createObjectsFancy()
//{
//    camera::position  = glm::vec3{0.0f, 0.0f, 150.0f};
//    camera::moveSpeed = 30.0f;
//    camera::turnSpeed = 0.3f;
//    camera::setSpotLight(1024, 1024, 
//                         0.01f, 100.0f,
//                         1.0f, 1.0f, 1.0f,
//                         0.0f, 10.0f,
//                         camera::position,
//                         glm::vec3(0.0f, -1.0f, 0.0f),
//                         0.1f, 0.1f, 0.5f,
//                         20.0f);
//
//    std::shared_ptr<Texture> sunTexture { std::make_shared<Texture>("../assets/textures/sun2.jpg") };
//    sunTexture->loadTexture();
//    std::shared_ptr<Texture> earthTexture { std::make_shared<Texture>("../assets/textures/earth.jpg") };
//    earthTexture->loadTexture();
//    std::shared_ptr<Texture> marsTexture { std::make_shared<Texture>("../assets/textures/mars.jpg") };
//    marsTexture->loadTexture();
//    std::shared_ptr<Texture> moonTexture { std::make_shared<Texture>("../assets/textures/moon.jpg") };
//    moonTexture->loadTexture();
//    std::shared_ptr<Texture> jupiterTexture { std::make_shared<Texture>("../assets/textures/jupiter.jpg") };
//    jupiterTexture->loadTexture();
//    std::shared_ptr<Texture> neptuneTexture { std::make_shared<Texture>("../assets/textures/neptune.jpg") };
//    neptuneTexture->loadTexture();
//    std::shared_ptr<Texture> venusTexture { std::make_shared<Texture>("../assets/textures/venus.jpg") };
//    venusTexture->loadTexture();
//    std::shared_ptr<Texture> cloudsTexture { std::make_shared<Texture>("../assets/textures/clouds.jpg") };
//    cloudsTexture->loadTexture();
//
//    std::shared_ptr<Material> material { std::make_shared<Material>(0.0f, 0) };
//
//    std::unique_ptr<Sun> sun { std::make_unique<Sun>(425.0f, 15.0f, 30, 30) };
//    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
//    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
//    sun->setTexturePointer(sunTexture);
//    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
//    sun->setAngle(90.0f);
//    sun->setRotationSpeed(-25.0f);
//    pointLights[pointLightCount++] = sun->getPointLight();
//	Sun* sunPtr{ sun.get() };
//	movables.push_back(std::move(sun));
//    stars.push_back(sunPtr);
//
//    std::unique_ptr<Planet> jupiter { std::make_unique<Planet>(4.0f, material, 4.0f, 20, 20) };
//    jupiter->setTexturePointer(jupiterTexture);
//    jupiter->setPosition(glm::vec3{0.5f, -20.0f, 65.0f});
//    jupiter->setVelocity(glm::vec3{0.0f, 22.0f, 0.0f});
//    jupiter->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
//    jupiter->setRotationSpeed(30.0f);
//	Planet* jupiterPtr{ jupiter.get() };
//    movables.push_back(std::move(jupiter));
//	nonStars.push_back(jupiterPtr);
//
//    std::unique_ptr<Planet> neptune { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    neptune->setTexturePointer(neptuneTexture);
//    neptune->setPosition(glm::vec3{55.5f, 0.0f, -2.5f});
//    neptune->setVelocity(glm::vec3{0.0f, 27.0f, 0.0f});
//    neptune->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
//    neptune->setRotationSpeed(30.0f);
//	Planet* neptunePtr{ neptune.get() };
//    movables.push_back(std::move(neptune));
//	nonStars.push_back(neptunePtr);
//
//    std::unique_ptr<Moon> neptuneMoon { std::make_unique<Planet>(1.25f, material, 0.5f, 20, 20) };
//    neptuneMoon->setTexturePointer(moonTexture);
//    neptuneMoon->setPosition(glm::vec3{59.0f, 0.0f, -2.5f});
//    neptuneMoon->setVelocity(glm::vec3{0.0f, 29.0f, 10.0f});
//    neptuneMoon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
//    neptuneMoon->setRotationSpeed(200.0f);
//	Moon* neptuneMoonPtr{ neptuneMoon.get() };
//    movables.push_back(std::move(neptuneMoon));
//	nonStars.push_back(neptuneMoonPtr);
//
//    std::unique_ptr<Planet> venus { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    venus->setTexturePointer(venusTexture);
//    venus->setPosition(glm::vec3{-65.5f, 0.0f, -22.5f});
//    venus->setVelocity(glm::vec3{0.0f, 25.0f, 8.0f});
//    venus->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
//    venus->setRotationSpeed(30.0f);
//	Planet* venusPtr{ venus.get() };
//    movables.push_back(std::move(venus));
//	nonStars.push_back(venusPtr);
//
//    std::unique_ptr<Moon> venusMoon { std::make_unique<Planet>(1.25f, material, 0.5f, 20, 20) };
//    venusMoon->setTexturePointer(cloudsTexture);
//    venusMoon->setPosition(glm::vec3{-65.5f, -3.0f, -22.5f});
//    venusMoon->setVelocity(glm::vec3{-4.0f, 25.0f, 8.0f});
//    venusMoon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
//    venusMoon->setRotationSpeed(200.0f);
//	Planet* venusMoonPtr{ venusMoon.get() };
//    movables.push_back(std::move(venusMoon));
//	nonStars.push_back(venusMoonPtr);
//
//    std::unique_ptr<Planet> mars { std::make_unique<Planet>(4.0f, material, 1.0f, 20, 20) };
//    mars->setTexturePointer(marsTexture);
//    mars->setPosition(glm::vec3{35.0f, -35.0f, 22.5f});
//    mars->setVelocity(glm::vec3{-15.0f, 15.0f, 15.0f});
//    mars->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
//    mars->setRotationSpeed(-100.0f);
//	Planet* marsPtr{ mars.get() };
//    movables.push_back(std::move(mars));
//	nonStars.push_back(marsPtr);
//
//    if (orbitalPhysics::verlet)
//        setOldPositions();
//}
