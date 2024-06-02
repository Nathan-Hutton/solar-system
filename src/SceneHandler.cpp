#include <array>

#include "SceneHandler.h"
#include "OrbitalPhysics.h"
#include "Sun.h"
#include "Planet.h"
#include "Model.h"
#include "Material.h"
#include "Scene.h"
#include "Camera.h"
#include "Skybox.h"

namespace 
{
    // This method will set old positions for all satellites in case we're using Verlet numerical integration
    void setOldPositions()
    {
        glm::vec3 acceleration {};
        glm::vec3 velocity {};
        glm::vec3 position {};
        
        // Apply forces to all planets and moons
        for (SpaceObject* satellite1 : scene::satellites)
        {
            glm::vec3 force {0};
            
            // Add up forces from stars
            for (const SpaceObject *star : scene::stars)
                force += orbitalPhysics::getForce(satellite1, star);
                
            // Add up forces for other satellites
            for (const SpaceObject* satellite2 : scene::satellites)
            {
                if (satellite1 == satellite2) continue;
                force += orbitalPhysics::getForce(satellite1, satellite2);
            }

            acceleration    = force / satellite1->getMass();
            velocity        = satellite1->getVelocity() + acceleration * 0.005f;
            satellite1->setOldPosition(satellite1->getPosition() - velocity * 0.005f);
        }
    }
}

void sceneHandler::setupSkybox(const glm::mat4& projection)
{
	std::array<std::string, 6> skyboxFaces {};
	skyboxFaces[0] = "../assets/textures/skybox/rightImage.png";
	skyboxFaces[1] = "../assets/textures/skybox/leftImage.png";
	skyboxFaces[2] = "../assets/textures/skybox/upImage.png";
	skyboxFaces[3] = "../assets/textures/skybox/downImage.png";
	skyboxFaces[4] = "../assets/textures/skybox/frontImage.png";
	skyboxFaces[5] = "../assets/textures/skybox/frontImage.png";
    skybox::setup(skyboxFaces);
    skybox::setProjectionMatrix(projection);
}

void sceneHandler::createObjectsDefault()
{
    scene::satellites.resize(4);
    scene::stars.resize(1);

    camera::position  = glm::vec3{0.0f, 0.0f, 110.0f};
    camera::moveSpeed = 25.0f;
    camera::turnSpeed = 0.3f;
    camera::setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera::position.x, camera::position.y, camera::position.z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture {new Texture{"../assets/textures/sun.jpg"}};
    sunTexture->loadTexture();
    Texture *earthTexture {new Texture{"../assets/textures/earth.jpg"}};
    earthTexture->loadTexture();
    Texture *marsTexture {new Texture{"../assets/textures/mars.jpg"}};
    marsTexture->loadTexture();
    Texture *moonTexture {new Texture{"../assets/textures/moon.jpg"}};
    moonTexture->loadTexture();

    Material *material {new Material{0.0f, 0}};

    Sun *sun {new Sun{429.3f, 7.0f, 25, 25}};
    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
    sun->setTexturePointer(sunTexture);
    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    scene::stars[0] = sun;
    scene::pointLights[scene::pointLightCount++] = sun->getPointLight();

    Planet *planet {new Planet{4.0f, material, 1.0f, 20, 20}};
    planet->setTexturePointer(earthTexture);
    planet->setPosition(glm::vec3{0.0f, -15.0f, -2.5f});
    planet->setVelocity(glm::vec3{-55.0f, 0.0f, 0.0f});
    planet->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    planet->setRotationSpeed(100.0f);
    scene::satellites[0] = planet;

    Planet *planet1 {new Planet{4.0f, material, 1.0f, 20, 20}};
    planet1->setTexturePointer(marsTexture);
    planet1->setPosition(glm::vec3{45.0f, 0.0f, -2.5f});
    planet1->setVelocity(glm::vec3{0.0f, -32.0f, 0.0f});
    planet1->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
    planet1->setRotationSpeed(-100.0f);
    scene::satellites[1] = planet1;

    Planet *moon {new Planet{1.25f, material, 0.5f, 20, 20}};
    moon->setTexturePointer(moonTexture);
    moon->setPosition(glm::vec3{42.0f, 0.0f, -2.5f});
    moon->setVelocity(glm::vec3{-3.0f, -45.0f, 0.0f});
    moon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    moon->setRotationSpeed(200.0f);
    scene::satellites[2] = moon;

    Model *asteroid {new Model{0.25f, material}};
    asteroid->loadModel("../assets/models/asteroid.obj");
    asteroid->setPosition(glm::vec3{-44.0f, 0.0f, -2.5f});
    asteroid->setVelocity(glm::vec3{-3.0f, 27.0f, 0.0f});
    asteroid->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    asteroid->setRotationSpeed(200.0f);
    asteroid->setScaleFactor(0.5f);
    scene::satellites[3] = asteroid;

    if (orbitalPhysics::verlet)
        setOldPositions();
}

void sceneHandler::createObjects1Sun1Planet()
{
    scene::satellites.resize(1);
    scene::stars.resize(1);

    camera::position  = glm::vec3{0.0f, 0.0f, 50.0f};
    camera::moveSpeed = 10.0f;
    camera::turnSpeed = 0.3f;
    camera::setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera::position.x, camera::position.y, camera::position.z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture {new Texture{"../assets/textures/sun.jpg"}};
    sunTexture->loadTexture();
    Texture *earthTexture {new Texture{"../assets/textures/earth.jpg"}};
    earthTexture->loadTexture();

    Material *material {new Material{0.0f, 0}};

    Sun *sun = new Sun{225.0f, 5.0f, 25, 25};
    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
    sun->setTexturePointer(sunTexture);
    sun->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 5.0f, 300.0f, 0.1f, 0.1f, 0.01f);
    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    scene::stars[0] = sun;
    scene::pointLights[scene::pointLightCount++] = sun->getPointLight();

    Planet *planet {new Planet{4.0f, material, 1.0f, 20, 20}};
    planet->setTexturePointer(earthTexture);
    planet->setPosition(glm::vec3{25.5f, 0.0f, -2.5f});
    planet->setVelocity(glm::vec3{0.0f, 22.0f, 0.0f});
    planet->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    planet->setRotationSpeed(100.0f);
    scene::satellites[0] = planet;

    if (orbitalPhysics::verlet)
        setOldPositions();
}

void sceneHandler::createObjectsFigureEight()
{
    scene::satellites.resize(1);
    scene::stars.resize(2);

    camera::position  = glm::vec3{0.0f, 0.0f, 50.0f};
    camera::moveSpeed = 10.0f;
    camera::turnSpeed = 0.3f;
    camera::setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera::position.x, camera::position.y, camera::position.z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture {new Texture{"../assets/textures/sun.jpg"}};
    sunTexture->loadTexture();
    Texture *earthTexture {new Texture{"../assets/textures/earth.jpg"}};
    earthTexture->loadTexture();

    Material *material {new Material{0.0f, 0}};

    Sun *sun1 {new Sun{67.0f, 2.0f, 25, 25}};
    sun1->setPosition(glm::vec3{-15.0f, 0.0f, -2.5f});
    sun1->setTexturePointer(sunTexture);
    sun1->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
    sun1->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
    sun1->setAngle(90.0f);
    sun1->setRotationSpeed(-25.2f);
    scene::stars[0] = sun1;
    scene::pointLights[scene::pointLightCount++] = sun1->getPointLight();

    Sun *sun2 {new Sun{67.0f, 2.0f, 25, 25}};
    sun2->setPosition(glm::vec3{15.0f, 0.0f, -2.5f});
    sun2->setTexturePointer(sunTexture);
    sun2->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
    sun2->setRotation(glm::vec3{0.0f, 1.0f, 1.0f});
    sun2->setAngle(90.0f);
    sun2->setRotationSpeed(25.0f);
    scene::stars[1] = sun2;
    scene::pointLights[scene::pointLightCount++] = sun2->getPointLight();

    Planet *planet {new Planet{4.0f, material, 1.0f, 20, 20}};
    planet->setTexturePointer(earthTexture);
    planet->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
    planet->setVelocity(glm::vec3{17.0f, 24.675f, 0.0f});
    planet->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
    planet->setRotationSpeed(100.0f);
    scene::satellites[0] = planet;

    if (orbitalPhysics::verlet)
        setOldPositions();
}

void sceneHandler::createObjectsFancy()
{
    scene::satellites.resize(6);
    scene::stars.resize(1);

    camera::position  = glm::vec3{0.0f, 0.0f, 150.0f};
    camera::moveSpeed = 30.0f;
    camera::turnSpeed = 0.3f;
    camera::setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera::position.x, camera::position.y, camera::position.z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture {new Texture{"../assets/textures/sun2.jpg"}};
    sunTexture->loadTexture();
    Texture *earthTexture {new Texture{"../assets/textures/earth.jpg"}};
    earthTexture->loadTexture();
    Texture *marsTexture {new Texture{"../assets/textures/mars.jpg"}};
    marsTexture->loadTexture();
    Texture *moonTexture {new Texture{"../assets/textures/moon.jpg"}};
    moonTexture->loadTexture();
    Texture *jupiterTexture {new Texture{"../assets/textures/jupiter.jpg"}};
    jupiterTexture->loadTexture();
    Texture *neptuneTexture {new Texture{"../assets/textures/neptune.jpg"}};
    neptuneTexture->loadTexture();
    Texture *venusTexture {new Texture{"../assets/textures/venus.jpg"}};
    venusTexture->loadTexture();
    Texture *cloudsTexture {new Texture{"../assets/textures/clouds.jpg"}};
    cloudsTexture->loadTexture();

    Material *material {new Material{0.0f, 0}};

    Sun *sun {new Sun{425.0f, 15.0f, 30, 30}};
    sun->setPosition(glm::vec3{0.0f, 0.0f, -2.5f});
    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
    sun->setTexturePointer(sunTexture);
    sun->setRotation(glm::vec3{1.0f, 1.0f, 0.0f});
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    scene::stars[0] = sun;
    scene::pointLights[scene::pointLightCount++] = sun->getPointLight();

    Planet *jupiter {new Planet{4.0f, material, 4.0f, 20, 20}};
    jupiter->setTexturePointer(jupiterTexture);
    jupiter->setPosition(glm::vec3{0.5f, -20.0f, 65.0f});
    jupiter->setVelocity(glm::vec3{0.0f, 22.0f, 0.0f});
    jupiter->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
    jupiter->setRotationSpeed(30.0f);
    scene::satellites[0] = jupiter;

    Planet *neptune {new Planet{4.0f, material, 1.0f, 20, 20}};
    neptune->setTexturePointer(neptuneTexture);
    neptune->setPosition(glm::vec3{55.5f, 0.0f, -2.5f});
    neptune->setVelocity(glm::vec3{0.0f, 27.0f, 0.0f});
    neptune->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
    neptune->setRotationSpeed(30.0f);
    scene::satellites[1] = neptune;

    Planet *neptuneMoon {new Planet{1.25f, material, 0.5f, 20, 20}};
    neptuneMoon->setTexturePointer(moonTexture);
    neptuneMoon->setPosition(glm::vec3{59.0f, 0.0f, -2.5f});
    neptuneMoon->setVelocity(glm::vec3{0.0f, 29.0f, 10.0f});
    neptuneMoon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    neptuneMoon->setRotationSpeed(200.0f);
    scene::satellites[2] = neptuneMoon;

    Planet *venus {new Planet{4.0f, material, 1.0f, 20, 20}};
    venus->setTexturePointer(venusTexture);
    venus->setPosition(glm::vec3{-65.5f, 0.0f, -22.5f});
    venus->setVelocity(glm::vec3{0.0f, 25.0f, 8.0f});
    venus->setRotation(glm::vec3{0.0f, 1.0f, 0.0f});
    venus->setRotationSpeed(30.0f);
    scene::satellites[3] = venus;

    Planet *venusMoon {new Planet{1.25f, material, 0.5f, 20, 20}};
    venusMoon->setTexturePointer(cloudsTexture);
    venusMoon->setPosition(glm::vec3{-65.5f, -3.0f, -22.5f});
    venusMoon->setVelocity(glm::vec3{-4.0f, 25.0f, 8.0f});
    venusMoon->setRotation(glm::vec3{1.0f, 0.0f, 2.0f});
    venusMoon->setRotationSpeed(200.0f);
    scene::satellites[4] = venusMoon;

    Planet *mars {new Planet{4.0f, material, 1.0f, 20, 20}};
    mars->setTexturePointer(marsTexture);
    mars->setPosition(glm::vec3{35.0f, -35.0f, 22.5f});
    mars->setVelocity(glm::vec3{-15.0f, 15.0f, 15.0f});
    mars->setRotation(glm::vec3{-1.0f, 0.0f, -2.0f});
    mars->setRotationSpeed(-100.0f);
    scene::satellites[5] = mars;

    if (orbitalPhysics::verlet)
        setOldPositions();
}
