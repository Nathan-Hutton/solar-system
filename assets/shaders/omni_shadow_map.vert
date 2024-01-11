#version 330

layout (location = 0) in vec3 pos;

// Need to know where the vertex is in world space
uniform mat4 model;

void main ()
{
    // We only need to know the position of the vertex in the world
    gl_Position = model * vec4(pos, 1.0);
}