#version 460

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D theTexture;

// We have 2 framebuffers. One that blurs horizontally and one that blurs vertically.
// We use this to tell which framebuffer we're on
uniform bool horizontal;

// These values align with values on the gaussian (bell) curve
float weight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);

// TODO: consider if it's better to just have 2 shaders instead of an if-else.
void main()
{
    vec2 tex_offset = 1.0f / textureSize(theTexture, 0); // Get the size of a texel (unit of a texture map)
    vec3 result = texture(theTexture, texCoords).rgb * weight[0]; // Current frag shader's contribution
    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(theTexture, texCoords + vec2(tex_offset.x * i * 2.0, 0.0)).rgb * weight[i];
            result += texture(theTexture, texCoords - vec2(tex_offset.x * i * 2.0, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(theTexture, texCoords + vec2(0.0, tex_offset.y * i * 1.5)).rgb * weight[i];
            result += texture(theTexture, texCoords - vec2(0.0, tex_offset.y * i * 1.5)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0f);
}
