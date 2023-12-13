#include "SceneHandler.h"

static const float toRadians = M_PI / 180.0f;

void SceneFunctions::createObjectsDefault(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, Camera *camera)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 110.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.3f);

    Texture *sunTexture = new Texture((char*)("Textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("Textures/earth.jpg"));
    earthTexture->loadTexture();
    Texture *marsTexture = new Texture((char*)("Textures/mars.jpg"));
    marsTexture->loadTexture();
    Texture *moonTexture = new Texture((char*)("Textures/moon.jpg"));
    moonTexture->loadTexture();

    Material *material = new Material(0.1f, 1);

    Sun *sun = new Sun(7.0f, 429.3f);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setPointLight(1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 0.0f, 0.001f, 1.0f);
    sun->setTexturePointer(sunTexture);
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);

    Planet *planet = new Planet(1.0f, 4.0f);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(0.0f, -15.0f, -2.5f));
    planet->setVelocity(glm::vec3(-55.0f, 0.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);

    Planet *planet1 = new Planet(1.0f, 4.0f);
    planet1->setTexturePointer(marsTexture);
    planet1->setMaterialPointer(material);
    planet1->setPosition(glm::vec3(45.0f, 0.0f, -2.5f));
    planet1->setVelocity(glm::vec3(0.0f, -32.0f, 0.0f));
    planet1->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet1->setRotationSpeed(-100.0f);
    satellites.push_back(planet1);

    Planet *moon = new Planet(0.5f, 1.25f);
    moon->setTexturePointer(moonTexture);
    moon->setMaterialPointer(material);
    moon->setPosition(glm::vec3(42.0f, 0.0f, -2.5f));
    moon->setVelocity(glm::vec3(-3.0f, -47.0f, 0.0f));
    moon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    moon->setRotationSpeed(200.0f);
    satellites.push_back(moon);
}

void SceneFunctions::createObjectsFigureEight(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, Camera *camera)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.3f);

    Texture *sunTexture = new Texture((char*)("Textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("Textures/earth.jpg"));
    earthTexture->loadTexture();

    Material *material = new Material(0.1f, 1);

    Sun *sun1 = new Sun(2.0f, 67.0f);
    sun1->setPosition(glm::vec3(-15.0f, 0.0f, -2.5f));
    sun1->setTexturePointer(sunTexture);
    sun1->setPointLight(1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 0.005f, 0.001f, 0.01f);
    sun1->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun1->setAngle(90.0f);
    sun1->setRotationSpeed(-25.2f);
    stars.push_back(sun1);

    Sun *sun2 = new Sun(2.0f, 67.0f);
    sun2->setPosition(glm::vec3(15.0f, 0.0f, -2.5f));
    sun2->setTexturePointer(sunTexture);
    sun2->setPointLight(1.0f, 1.0f, 1.0f, 0.1f, 1.0f, 0.005f, 0.001f, 0.01f);
    sun2->setRotation(glm::vec3(0.0f, 1.0f, 1.0f));
    sun2->setAngle(90.0f);
    sun2->setRotationSpeed(25.0f);
    stars.push_back(sun2);

    Planet *planet = new Planet(1.0f, 4.0f);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(17.0f, 24.675f, 0.0f));
    planet->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);
}

void SceneFunctions::create1Sun1Planet(std::vector<Sun*>& stars, std::vector<Planet*>& satellites, Camera *camera)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.3f);

    Texture *sunTexture = new Texture((char*)("Textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("Textures/earth.jpg"));
    earthTexture->loadTexture();

    Material *material = new Material(1.0f, 32);

    Sun *sun = new Sun(5.0f, 225.0f);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setTexturePointer(sunTexture);
    sun->setPointLight(1.0f, 1.0f, 1.0f, 8.0f, 100.0f, 0.1f, 0.1f, 0.01f);
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);

    Planet *planet = new Planet(1.0f, 4.0f);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(25.5f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(0.0f, 22.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);
}
