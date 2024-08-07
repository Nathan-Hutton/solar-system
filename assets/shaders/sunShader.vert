#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;
                                                                
out vec4 vCol;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 modelToClipSpace;

void main()
{
    gl_Position = modelToClipSpace * vec4(pos, 1.0);
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
    
    texCoord = tex;
    normal = mat3(transpose(inverse(model))) * norm;

    // .xyz makes it a vec3
    fragPos = (model * vec4(pos, 1.0f)).xyz;
}

