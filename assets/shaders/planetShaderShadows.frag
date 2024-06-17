#version 460

// This lets us write to our 2 color buffers.
// For the bloom effect, we put the blurred parts over the main color image
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 directionalLightSpacePos;

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct PointLight
{
	Light base;
	vec3 position;
	float exponential;
	float linear;
	float constant;
};

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct OmniShadowMap
{
	samplerCube shadowMap;
	float farPlane;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform bool flashLightOn;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;

// sampler2D is what GLSL uses for textures
uniform sampler2D theTexture;
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + 1];

uniform Material material;

uniform vec3 eyePosition;

// This will be used to make soft shadows for omni directional shadow maps
const vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
	// Use all the commented out code instead to remove soft shadows and increase performance

	//vec3 lightToFrag = fragPos - light.position;
	//float closest = texture(omniShadowMaps[shadowIndex].shadowMap, lightToFrag).r;

	//// Undo normalization done in omni_shadow_map.frag
	//closest *= omniShadowMaps[shadowIndex].farPlane;

	//float current = length(lightToFrag);

	//float bias = 0.05;
	//return current - bias > closest ? 1.0 : 0.0;
	
	const vec3 lightToFrag = fragPos - light.position;
	const float currentDepth = length(lightToFrag);

	float shadow = 0.0;
	const float bias = 0.05;
	const int samples = 20;
	
	// Distance in each direction based on how far the camera is from the fragment
	const float viewDistance = length(fragPos - eyePosition);
	const float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0;
	
	// This adds a sort of fade to the edges of the shadows (soft shadows)
	for (int i = 0; i < samples; i++)
	{

		float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= omniShadowMaps[shadowIndex].farPlane; // Undo normalization done in omni_shadow_map.frag
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	const vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;
	
	const float diffuseFactor = max(dot(normalize(normal), -normalize(direction)), 0.0f);
	const vec4 diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.0f);
	
	vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if(diffuseFactor > 0.0f)
	{
		const vec3 fragToEye = normalize(eyePosition - fragPos);
		const vec3 reflectedVertex = normalize(reflect(direction, normalize(normal)));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return ambientColor + (1.0f - shadowFactor) * (diffuseColor + specularColor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
    vec3 direction = fragPos - pLight.position;
    const float distance = length(direction);
    direction = normalize(direction);

	float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);
    
    const vec4 color = CalcLightByDirection(pLight.base, direction, shadowFactor);
    const float attenuation = pLight.exponential * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;
    
    return color / attenuation;
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
    const vec3 rayDirection = normalize(fragPos - sLight.base.position);
    const float slFactor = dot(rayDirection, sLight.direction);

	if (slFactor <= sLight.edge)
		return vec4(0, 0, 0, 0);

	const vec4 color = CalcPointLight(sLight.base, shadowIndex);
    return color * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < pointLightCount; i++)
		totalColor += CalcPointLight(pointLights[i], i);
	
	return totalColor;
}

void main()
{
	vec4 finalColor = CalcPointLights();
    if (flashLightOn)
        finalColor += CalcSpotLight(spotLight, pointLightCount);

	fragColor = texture(theTexture, texCoord) * finalColor;
    bloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
