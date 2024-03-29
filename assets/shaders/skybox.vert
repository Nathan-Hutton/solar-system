#version 460

layout (location = 0) in vec3 pos;

// This will really be the same as the position
out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = pos;
    // The xyww sets the z value to w which is the value of the far plane
    gl_Position = (projection * view * vec4(pos, 1.0)).xyww;
}
