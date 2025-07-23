#include "Sun.h"

Sun::Sun(GLfloat mass, std::shared_ptr<Mesh> sphereMesh, std::shared_ptr<Texture> texture) 
    : Sphere{ mass, sphereMesh, texture, nullptr } 
{}

void Sun::setPointLight(GLuint shadowWidth, GLuint shadowHeight,
    GLfloat near, GLfloat far,
    GLfloat red, GLfloat green, GLfloat blue, 
    GLfloat ambientIntensity, GLfloat diffuseIntensity,
    GLfloat exponential, GLfloat linear, GLfloat constant)
{
    m_light = new PointLight{shadowWidth, shadowHeight,
            near, far,
            red, green, blue, 
            ambientIntensity, diffuseIntensity,
            getPosition(),
            exponential, linear, constant};
}
