#version 460

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D theTexture;

// We have 2 framebuffers. One that blurs horizontally and one that blurs vertically.
// We use this to tell which framebuffer we're on
uniform bool horizontal;

// These values align with values on the gaussian (bell) curve
// The center value (top of the bell curve) is really the 0.227 value on the left.
// What we do is apply this both to the right and left to save on performance, so we only need
// to define 1 side of the weights (bell curve) since there's symmetry
float weight[5] = float[] (0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f);
void main()
{
    vec2 tex_offset = 1.0f / textureSize(theTexture, 0); // Get the size of a texel (unit of a texture map)

    // The first value is at the top of bell curve (weight[0])
    vec3 result = texture(theTexture, texCoords).rgb * weight[0]; // Current frag shader's contribution
    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            // Multiply by 2.5 so we don't need to do as many passes.
            // Multiply by texture size (tex_offset.x) which offsets us 1 pixel.
            result += texture(theTexture, texCoords + vec2(tex_offset.x * i * 2.5, 0.0)).rgb * weight[i];
            result += texture(theTexture, texCoords - vec2(tex_offset.x * i * 2.5, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(theTexture, texCoords + vec2(0.0, tex_offset.y * i * 2.5)).rgb * weight[i];
            result += texture(theTexture, texCoords - vec2(0.0, tex_offset.y * i * 2.5)).rgb * weight[i];
        }
    }
    fragColor = vec4(result, 1.0f);
}
