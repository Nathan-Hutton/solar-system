#version 460

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;
layout (location = 2) out float shouldGammaCorrect;

in vec4 vCol;
in vec2 texCoord;
in vec3 fragPos;

uniform sampler2D theTexture;

void main()
{
	fragColor = (texture(theTexture, texCoord) * 2);
    bloomColor = vec4(fragColor.rgb, 1.0f);
    shouldGammaCorrect = 1.0;
}
