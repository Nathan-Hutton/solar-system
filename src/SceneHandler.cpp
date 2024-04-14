#include "SceneHandler.h"

// This method will set old positions for all satellites in case we're using Verlet numerical integration
void SceneFunctions::setOldPositions(std::vector<SpaceObject*>& satellites, std::vector<SpaceObject*>& stars)
{
    glm::vec3 acceleration;
    glm::vec3 velocity;
    glm::vec3 position;
    
    // Apply forces to all planets and moons
    for (int i = 0; i < satellites.size(); i++) 
    {
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // Add up forces from stars
        for (SpaceObject *star : stars)
            force += OrbitalPhysicsFunctions::getForce(satellites[i], star);
            
        // Add up forces for other satellites
        for (int j = 0; j < satellites.size(); j++) 
        {
            if (i == j) continue;
            force += OrbitalPhysicsFunctions::getForce(satellites[i], satellites[j]);
        }

        acceleration = force / satellites[i]->getMass();
        velocity = satellites[i]->getVelocity() + acceleration * 0.005f;
        satellites[i]->setOldPosition(satellites[i]->getPosition() - velocity * 0.005f);
    }
}

void SceneFunctions::setupSkybox(Skybox* skybox, glm::mat4 projection)
{
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("../assets/textures/skybox/rightImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/leftImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/upImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/downImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");
	skyboxFaces.push_back("../assets/textures/skybox/frontImage.png");
	*skybox = Skybox(skyboxFaces);
    skybox->setProjectionMatrix(projection);
}

void SceneFunctions::createObjectsDefault(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 110.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 20.0f, 0.3f);
    camera->setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture = new Texture((char*)("../assets/textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("../assets/textures/earth.jpg"));
    earthTexture->loadTexture();
    Texture *marsTexture = new Texture((char*)("../assets/textures/mars.jpg"));
    marsTexture->loadTexture();
    Texture *moonTexture = new Texture((char*)("../assets/textures/moon.jpg"));
    moonTexture->loadTexture();

    Material *material = new Material(0.0f, 0);

    Sun *sun = new Sun(7.0f, 429.3f);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
    sun->setTexturePointer(sunTexture);
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);
    pLights[(*pLightCount)++] = sun->getPointLight();

    Planet *planet = new Planet(1.0f, 4.0f, 15, 15);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(0.0f, -15.0f, -2.5f));
    planet->setVelocity(glm::vec3(-55.0f, 0.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);

    Planet *planet1 = new Planet(1.0f, 4.0f, 15, 15);
    planet1->setTexturePointer(marsTexture);
    planet1->setMaterialPointer(material);
    planet1->setPosition(glm::vec3(45.0f, 0.0f, -2.5f));
    planet1->setVelocity(glm::vec3(0.0f, -32.0f, 0.0f));
    planet1->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet1->setRotationSpeed(-100.0f);
    satellites.push_back(planet1);

    Planet *moon = new Planet(0.5f, 1.25f, 15, 15);
    moon->setTexturePointer(moonTexture);
    moon->setMaterialPointer(material);
    moon->setPosition(glm::vec3(42.0f, 0.0f, -2.5f));
    moon->setVelocity(glm::vec3(-3.0f, -45.0f, 0.0f));
    moon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    moon->setRotationSpeed(200.0f);
    satellites.push_back(moon);

    Model *asteroid = new Model(0.25f);
    asteroid->loadModel("../assets/models/asteroid.obj");
    asteroid->setMaterialPointer(material);
    asteroid->setPosition(glm::vec3(-44.0f, 0.0f, -2.5f));
    asteroid->setVelocity(glm::vec3(-3.0f, 27.0f, 0.0f));
    asteroid->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    asteroid->setRotationSpeed(200.0f);
    asteroid->setScaleFactor(0.5f);
    satellites.push_back(asteroid);

    if (verlet)
        setOldPositions(satellites, stars);
}

void SceneFunctions::createObjectsFigureEight(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.3f);
    camera->setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture = new Texture((char*)("../assets/textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("../assets/textures/earth.jpg"));
    earthTexture->loadTexture();

    Material *material = new Material(0.0f, 0);

    Sun *sun1 = new Sun(2.0f, 67.0f);
    sun1->setPosition(glm::vec3(-15.0f, 0.0f, -2.5f));
    sun1->setTexturePointer(sunTexture);
    sun1->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
    sun1->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun1->setAngle(90.0f);
    sun1->setRotationSpeed(-25.2f);
    stars.push_back(sun1);
    pLights[(*pLightCount)++] = sun1->getPointLight();

    Sun *sun2 = new Sun(2.0f, 67.0f);
    sun2->setPosition(glm::vec3(15.0f, 0.0f, -2.5f));
    sun2->setTexturePointer(sunTexture);
    sun2->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 0.1f, 20.0f, 0.005f, 0.001f, 0.01f);
    sun2->setRotation(glm::vec3(0.0f, 1.0f, 1.0f));
    sun2->setAngle(90.0f);
    sun2->setRotationSpeed(25.0f);
    stars.push_back(sun2);
    pLights[(*pLightCount)++] = sun2->getPointLight();

    Planet *planet = new Planet(1.0f, 4.0f, 15, 15);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(17.0f, 24.675f, 0.0f));
    planet->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);

    if (verlet)
        setOldPositions(satellites, stars);
}

void SceneFunctions::createObjects1Sun1Planet(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet)
{
    *camera = Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 10.0f, 0.3f);
    camera->setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Texture *sunTexture = new Texture((char*)("../assets/textures/sun.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("../assets/textures/earth.jpg"));
    earthTexture->loadTexture();

    Material *material = new Material(0.0f, 0);

    Sun *sun = new Sun(5.0f, 225.0f);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setTexturePointer(sunTexture);
    sun->setPointLight(1024, 1024, 0.01f, 100.0f, 1.0f, 1.0f, 1.0f, 5.0f, 300.0f, 0.1f, 0.1f, 0.01f);
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);
    pLights[(*pLightCount)++] = sun->getPointLight();

    Planet *planet = new Planet(1.0f, 4.0f, 15, 15);
    planet->setTexturePointer(earthTexture);
    planet->setMaterialPointer(material);
    planet->setPosition(glm::vec3(25.5f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(0.0f, 22.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);

    if (verlet)
        setOldPositions(satellites, stars);
}

void SceneFunctions::createObjectsFancy(std::vector<SpaceObject*>& stars, std::vector<SpaceObject*>& satellites,
            PointLight* pLights[], unsigned int *pLightCount,
            Camera *camera, bool verlet)
{
    Texture *sunTexture = new Texture((char*)("../assets/textures/sun2.jpg"));
    sunTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("../assets/textures/earth.jpg"));
    earthTexture->loadTexture();
    Texture *marsTexture = new Texture((char*)("../assets/textures/mars.jpg"));
    marsTexture->loadTexture();
    Texture *moonTexture = new Texture((char*)("../assets/textures/moon.jpg"));
    moonTexture->loadTexture();
    Texture *jupiterTexture = new Texture((char*)("../assets/textures/jupiter.jpg"));
    jupiterTexture->loadTexture();
    Texture *neptuneTexture = new Texture((char*)("../assets/textures/neptune.jpg"));
    neptuneTexture->loadTexture();
    Texture *venusTexture = new Texture((char*)("../assets/textures/venus.jpg"));
    venusTexture->loadTexture();
    Texture *cloudsTexture = new Texture((char*)("../assets/textures/clouds.jpg"));
    cloudsTexture->loadTexture();

    Material *material = new Material(0.0f, 0);

    *camera = Camera(glm::vec3(0.0f, 0.0f, 150.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 30.0f, 0.3f);
    camera->setSpotLight(1024, 1024, 
                         0.01f, 100.0f,
                         1.0f, 1.0f, 1.0f,
                         0.0f, 10.0f,
                         camera->getPosition().x, camera->getPosition().y, camera->getPosition().z,
                         0.0f, -1.0f, 0.0f,
                         0.1f, 0.1f, 0.5f,
                         20.0f);

    Sun *sun = new Sun(15.0f, 425.0f, 30, 30);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setPointLight(1024, 1024, 1.0f, 100.0f, 1.0f, 1.0f, 1.0f, 0.2f, 15.0f, 0.0f, 0.001f, 1.0f);
    sun->setTexturePointer(sunTexture);
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);
    pLights[(*pLightCount)++] = sun->getPointLight();

    Planet *jupiter = new Planet(4.0f, 4.0f, 15, 15);
    jupiter->setTexturePointer(jupiterTexture);
    jupiter->setMaterialPointer(material);
    jupiter->setPosition(glm::vec3(0.5f, -20.0f, 65.0f));
    jupiter->setVelocity(glm::vec3(0.0f, 22.0f, 0.0f));
    jupiter->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    jupiter->setRotationSpeed(30.0f);
    satellites.push_back(jupiter);

    Planet *neptune = new Planet(1.0f, 4.0f, 15, 15);
    neptune->setTexturePointer(neptuneTexture);
    neptune->setMaterialPointer(material);
    neptune->setPosition(glm::vec3(55.5f, 0.0f, -2.5f));
    neptune->setVelocity(glm::vec3(0.0f, 27.0f, 0.0f));
    neptune->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    neptune->setRotationSpeed(30.0f);
    satellites.push_back(neptune);

    Planet *neptuneMoon = new Planet(0.5f, 1.25f, 15, 15);
    neptuneMoon->setTexturePointer(moonTexture);
    neptuneMoon->setMaterialPointer(material);
    neptuneMoon->setPosition(glm::vec3(59.0f, 0.0f, -2.5f));
    neptuneMoon->setVelocity(glm::vec3(0.0f, 29.0f, 10.0f));
    neptuneMoon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    neptuneMoon->setRotationSpeed(200.0f);
    satellites.push_back(neptuneMoon);

    Planet *venus = new Planet(1.0f, 4.0f, 15, 15);
    venus->setTexturePointer(venusTexture);
    venus->setMaterialPointer(material);
    venus->setPosition(glm::vec3(-65.5f, 0.0f, -22.5f));
    venus->setVelocity(glm::vec3(0.0f, 25.0f, 8.0f));
    venus->setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    venus->setRotationSpeed(30.0f);
    satellites.push_back(venus);

    Planet *venusMoon = new Planet(0.5f, 1.25f, 15, 15);
    venusMoon->setTexturePointer(cloudsTexture);
    venusMoon->setMaterialPointer(material);
    venusMoon->setPosition(glm::vec3(-65.5f, -3.0f, -22.5f));
    venusMoon->setVelocity(glm::vec3(-4.0f, 25.0f, 8.0f));
    venusMoon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    venusMoon->setRotationSpeed(200.0f);
    satellites.push_back(venusMoon);

    Planet *mars = new Planet(1.0f, 4.0f, 15, 15);
    mars->setTexturePointer(marsTexture);
    mars->setMaterialPointer(material);
    mars->setPosition(glm::vec3(35.0f, -35.0f, 22.5f));
    mars->setVelocity(glm::vec3(-15.0f, 15.0f, 15.0f));
    mars->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    mars->setRotationSpeed(-100.0f);
    satellites.push_back(mars);

    if (verlet)
        setOldPositions(satellites, stars);
}
