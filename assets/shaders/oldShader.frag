#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

struct DirectionalLight {
    vec3 color;
    float ambientIntensity;
    vec3 direction;
    float diffuseIntensity;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform sampler2D theTexture;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePosition;

void main()
{
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

    float diffuseFactor = max(dot(normalize(directionalLight.direction), normalize(normal)), 0.0f);
    vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    if (diffuseFactor > 0)
    {
        vec3 fragToEye = normalize(eyePosition - fragPos);
        vec3 reflectedVector = normalize(reflect(directionalLight.direction, normalize(normal)));
        float specularFactor = abs(dot(reflectedVector, fragToEye));
        if (specularFactor > 0)
        {
            specularFactor = pow(specularFactor, material.shininess);
            specularColor = vec4(directionalLight.color * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    color = texture(theTexture, texCoord) * (ambientColor + diffuseColor + specularColor);
}
