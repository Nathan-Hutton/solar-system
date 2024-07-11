#version 460

layout (location = 0) in vec3 pos;

// This will really be the same as the position
out vec3 texCoords;

uniform mat4 worldToClip;

void main()
{
    texCoords = pos;
    // The xyww sets the z value to w which is the value of the far plane
    gl_Position = (worldToClip * vec4(pos, 1.0)).xyww;
}
