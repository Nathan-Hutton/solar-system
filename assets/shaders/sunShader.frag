#version 330

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

in vec4 vCol;
in vec2 texCoord;
in vec3 fragPos;

uniform sampler2D theTexture;

void main()
{
    vec4 emissiveColor = vec4(0.8, 0.4, 0.0, 0.0);
	fragColor = (texture(theTexture, texCoord) * 200) + emissiveColor;

    // Consider multiplying yellow colors to make them pop more

    float brightness = dot(fragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    bloomColor = vec4(fragColor.rgb, 1.0f);
}
