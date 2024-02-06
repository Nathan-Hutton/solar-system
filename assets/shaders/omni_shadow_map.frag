#version 460

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    // Distance between the fragment and light, relative to the far plane (between 0 and 1)
    gl_FragDepth = length(fragPos.xyz - lightPos) / farPlane;
}
