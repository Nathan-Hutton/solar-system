#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 fragPos;

out vec4 color;

uniform sampler2D theTexture;

void main()
{
	color = texture(theTexture, texCoord) * 1.5;
}
