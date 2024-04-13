#version 330
in vec2 aPos;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    texCoord = vec2(aPos.x * 0.5 + 0.5, aPos.y * 0.5 + 0.5);
}
