#include "Light.h"

Light::Light()
    : m_color { 1.0f },
    m_ambientIntensity { 1.0f },
    m_diffuseIntensity { 0.0f }
{}

Light::Light(GLuint shadowWidth, GLuint shadowHeight,
    GLfloat red, GLfloat green, GLfloat blue,
    GLfloat ambientIntensity, GLfloat diffuseIntensity)
    : m_shadowMap { new ShadowMap{} },
    m_color { red, green, blue },
    m_ambientIntensity { ambientIntensity },
    m_diffuseIntensity { diffuseIntensity }
{
    m_shadowMap->init(shadowWidth, shadowHeight);
}

Light::~Light()
{
}
