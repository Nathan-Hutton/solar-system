#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SpotLight.h"

namespace camera {
    extern glm::vec3 position;
    extern GLfloat moveSpeed;
    extern GLfloat turnSpeed;
    extern std::unique_ptr<SpotLight> spotLight;

	void jsonSetup(nlohmann::json sceneData);
    void keyControl(bool* keys, GLfloat deltaTime);
    void mouseControl(GLfloat xChange, GLfloat yChange);
    void setSpotLight(GLuint shadowWidth, GLuint shadowHeight,
                    GLfloat near, GLfloat far,
                    GLfloat red, GLfloat green, GLfloat blue, 
                    GLfloat ambientIntensity, GLfloat diffuseIntensity, 
                    glm::vec3 position,
                    glm::vec3 direction,
                    GLfloat exponential, GLfloat linear, GLfloat constant,
                    GLfloat edge);

    void calculateViewMatrix(glm::mat4& viewMatrix);
};
