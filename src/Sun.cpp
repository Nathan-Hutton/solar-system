#include "Sun.h"

Sun::Sun(GLfloat mass, GLfloat radius, int stacks, int slices) 
    : Sphere(mass,radius, stacks, slices, false) 
{}

// This method is different from Sphere's version because we don't need normals
void Sun::render() const
{
    m_texture->useTexture();
    m_sphereMesh->render();
}

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
            m_position.x, m_position.y, m_position.z, 
            exponential, linear, constant};
}

// This is here so we can throw a vector of suns into the same methods that planets and models use
void Sun::setUniformVariables(GLuint, GLuint){}
