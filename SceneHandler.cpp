#include "SceneHandler.h"

static const float toRadians = M_PI / 180.0f;

void SceneFunctions::createObjectsDefault(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites)
{
    Texture *brickTexture = new Texture((char*)("Textures/brick.png"));
    brickTexture->loadTexture();
    Texture *dirtTexture = new Texture((char*)("Textures/dirt.png"));
    dirtTexture->loadTexture();

    Sun *sun = new Sun(2.0f, 2.0f);
    sun->setTexturePointer(brickTexture);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);

    Sphere *planet = new Sphere(1.0f, 3.0f);
    planet->setTexturePointer(brickTexture);
    planet->setPosition(glm::vec3(-5.0f, -10.0f, -2.5f));
    planet->setVelocity(glm::vec3(-30.0f, 0.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);

    Sphere *planet1 = new Sphere(1.0f, 2.0f);
    planet1->setTexturePointer(brickTexture);
    planet1->setPosition(glm::vec3(7.5f, 10.0f, -2.5f));
    planet1->setVelocity(glm::vec3(20.0f, -12.0f, 10.0f));
    planet1->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet1->setRotationSpeed(-100.0f);
    satellites.push_back(planet1);

    Sphere *moon = new Sphere(0.5f, 0.5f);
    moon->setTexturePointer(brickTexture);
    moon->setPosition(glm::vec3(-7.0f, -14.0f, -2.5f));
    moon->setVelocity(glm::vec3(-40.0f, 2.0f, 1.0f));
    moon->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    moon->setRotationSpeed(200.0f);
    satellites.push_back(moon);
}

void SceneFunctions::createObjectsFigureEight(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites)
{
    Texture *brickTexture = new Texture((char*)("Textures/brick.png"));
    brickTexture->loadTexture();
    Texture *dirtTexture = new Texture((char*)("Textures/dirt.png"));
    dirtTexture->loadTexture();

    Sun *sun1 = new Sun(2.0f, 2.0f);
    sun1->setTexturePointer(brickTexture);
    sun1->setPosition(glm::vec3(-15.0f, 0.0f, -2.5f));
    sun1->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun1->setAngle(90.0f);
    sun1->setRotationSpeed(-25.2f);
    stars.push_back(sun1);

    Sun *sun2 = new Sun(2.0f, 2.0f);
    sun2->setTexturePointer(brickTexture);
    sun2->setPosition(glm::vec3(15.0f, 0.0f, -2.5f));
    sun2->setRotation(glm::vec3(0.0f, 1.0f, 1.0f));
    sun2->setAngle(90.0f);
    sun2->setRotationSpeed(25.0f);
    stars.push_back(sun2);

    Sphere *planet = new Sphere(1.0f, 2.0f);
    planet->setTexturePointer(brickTexture);
    planet->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(17.0f, 24.675f, 0.0f));
    planet->setRotation(glm::vec3(-1.0f, 0.0f, -2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);
}

void SceneFunctions::create1Sun1Planet(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites)
{
    Texture *brickTexture = new Texture((char*)("Textures/brick.png"));
    brickTexture->loadTexture();
    Texture *dirtTexture = new Texture((char*)("Textures/dirt.png"));
    dirtTexture->loadTexture();
    Texture *earthTexture = new Texture((char*)("Textures/water.png"));
    earthTexture->loadTexture();
    Texture *sunTexture = new Texture((char*)("Textures/venus.jpg"));
    sunTexture->loadTexture();

    Sun *sun = new Sun(2.0f, 2.0f);
    sun->setTexturePointer(sunTexture);
    sun->setPosition(glm::vec3(0.0f, 0.0f, -2.5f));
    sun->setRotation(glm::vec3(1.0f, 1.0f, 0.0f));
    sun->setAngle(90.0f);
    sun->setRotationSpeed(-25.0f);
    stars.push_back(sun);

    Sphere *planet = new Sphere(1.0f, 3.0f);
    planet->setTexturePointer(earthTexture);
    planet->setPosition(glm::vec3(17.5f, 0.0f, -2.5f));
    planet->setVelocity(glm::vec3(0.0f, 15.0f, 0.0f));
    planet->setRotation(glm::vec3(1.0f, 0.0f, 2.0f));
    planet->setRotationSpeed(100.0f);
    satellites.push_back(planet);
}

void SceneFunctions::renderObjects(std::vector<Sun*>& stars, std::vector<Sphere*>& satellites, std::vector<Shader*>& shaderList, Camera *camera, glm::mat4 *projection)
{
    // Apply shaders
    shaderList[0]->useShader();
    GLuint uniformModel = shaderList[0]->getModelLocation();
    GLuint uniformProjection = shaderList[0]->getProjectionLocation();
    GLuint uniformView = shaderList[0]->getViewLocation();
    glm::mat4 model;

    for (Sun *star : stars)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, star->getPosition());
        model = glm::rotate(model, star->getAngle() * toRadians, star->getRotation());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        star->renderMesh();
    }
    // Apply projection and view
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(*projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));

    shaderList[1]->useShader();
    uniformModel = shaderList[1]->getModelLocation();
    uniformProjection = shaderList[1]->getProjectionLocation();
    uniformView = shaderList[1]->getViewLocation();

    for (Sphere *satellite : satellites)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, satellite->getPosition());
        model = glm::rotate(model, satellite->getAngle() * toRadians, satellite->getRotation());
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        satellite->renderMesh();
    }
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(*projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
}

