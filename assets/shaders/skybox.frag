#version 330

in vec3 texCoords;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

uniform samplerCube skybox;

void main()
{
    fragColor = texture(skybox, texCoords);
    bloomColor = vec4(0.0, 0.0, 0.0, 1.0);
    gl_FragDepth = 1.0;
}
