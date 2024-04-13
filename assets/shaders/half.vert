#version 330 core
in vec2 aPos;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
}
