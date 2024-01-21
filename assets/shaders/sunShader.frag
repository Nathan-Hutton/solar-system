#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 fragPos;

out vec4 color;

uniform sampler2D theTexture;

void main()
{
    vec4 emissiveColor = vec4(0.8, 0.4, 0.0, 0.0);
	color = texture(theTexture, texCoord) + emissiveColor;
}
