#version 330

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec4 directionalLightSpacePos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
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
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

// sampler2D is what GLSL uses for textures
uniform sampler2D theTexture;
uniform sampler2D directionalShadowMap; // Use this to see if the point we're looking at is in shadow or not
uniform OmniShadowMap omniShadowMaps[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

uniform Material material;

uniform vec3 eyePosition;

// This will be used to make soft shadows for omni directional shadow maps
vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float calcDirectionalShadowFactor(DirectionalLight light)
{
	// Normalize from -1 to 1
	vec3 projCoords = directionalLightSpacePos.xyz / directionalLightSpacePos.w;
	
	// Go from a -1, 1 scale to a 0, 1 scale
	projCoords = (projCoords * 0.5) + 0.5;

	// Depth of the fragment we're looking at relative to the light
	float currentDepth = projCoords.z;

	// Could be in shadow, but the far plane doesn't extend that far,
	// so it would be greater than 1 and always return 1.0 (in shadow)
	if (currentDepth > 1.0)
		return 0.0;

	// Implement a bias whose magnitude is dependent on the angle between the light and fragment
	vec3 normal = normalize(normal);
	vec3 lightDir = normalize(light.direction);
	float bias = max(0.0005 * (1 - dot(normal, lightDir)), 0.005);

	float shadow = 0.0;

	// Add PCF for soft shadows
	vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            // 1.0 is full shadow. 0.0 is no shadow
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;
    float closestDepth = texture(directionalShadowMap, projCoords.xy).r;

	return shadow;
}

float CalcOmniShadowFactor(PointLight light, int shadowIndex)
{
	// Use all the commented out code instead to remove soft shadows and increase performance

	//// Swap these in the solar system
	////vec3 fragToLight = light.position - fragPos;
	
	//vec3 fragToLight = fragPos - light.position;
	//float closest = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight).r;

	//// Undo normalization done in omni_shadow_map.frag
	//closest *= omniShadowMaps[shadowIndex].farPlane;

	//float current = length(fragToLight);

	//float bias = 0.05;
	//return current - bias > closest ? 1.0 : 0.0;
	
	vec3 fragToLight = fragPos - light.position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float bias = 0.05;
	int samples = 20;
	
	// Distance in each direction based on how far the camera is from the fragment
	float viewDistance = length(fragPos - eyePosition);
	float diskRadius = (1.0 + (viewDistance / omniShadowMaps[shadowIndex].farPlane)) / 25.0;
	
	// This adds a sort of fade to the edges of the shadows (soft shadows)
	for (int i = 0; i < samples; i++)
	{

		float closestDepth = texture(omniShadowMaps[shadowIndex].shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= omniShadowMaps[shadowIndex].farPlane; // Undo normalization done in omni_shadow_map.frag
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}

	shadow /= float(samples);
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor)
{
	vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(normal), -normalize(direction)), 0.0f);
	vec4 diffuseColor = vec4(light.color * light.diffuseIntensity * diffuseFactor, 1.0f);
	
	vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if(diffuseFactor > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(normal)));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, material.shininess);
			specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
		}
	}

	return ambientColor + (1.0f - shadowFactor) * (diffuseColor + specularColor);
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = calcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight, int shadowIndex)
{
    vec3 direction = fragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);

	float shadowFactor = CalcOmniShadowFactor(pLight, shadowIndex);
    
    vec4 color = CalcLightByDirection(pLight.base, direction, shadowFactor);
    float attenuation = pLight.exponential * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;
    
    return color / attenuation;
}

vec4 CalcSpotLight(SpotLight sLight, int shadowIndex)
{
    vec3 rayDirection = normalize(fragPos - sLight.base.position);
    float slFactor = dot(rayDirection, sLight.direction);

	if (slFactor <= sLight.edge)
		return vec4(0, 0, 0, 0);

	vec4 color = CalcPointLight(sLight.base, shadowIndex);
    return color * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < pointLightCount; i++)
	{
		totalColor += CalcPointLight(pointLights[i], i);
	}
	
	return totalColor;
}

vec4 CalcSpotLights()
{
	vec4 totalColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for(int i = 0; i < spotLightCount; i++)
	{
		totalColor += CalcSpotLight(spotLights[i], i + pointLightCount);
	}
	
	return totalColor;
}

void main()
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
    finalColor += CalcSpotLights();
	
	color = texture(theTexture, texCoord) * finalColor;
}
