#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;
                                                                
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

// Responsible for scaling/rotating/moving our objects (put them in world space instead of local space)
uniform mat4 model;
uniform mat4 modelToClipSpace;

void main()
{
    // Built in GLSL variable that holds the final position of the vertex.
    // This is the key output (not passed to fragment shader) that the program
    // Uses to make the primitives, which rasterization uses to get fragments out of
    // We multiply all this in reverse order (CBAv instead of vABC) since OpenGL uses column vectors
    gl_Position = modelToClipSpace * vec4(pos, 1.0);
    
    // Pass the texture coordinates to the fragment shader
    texCoord = tex;
    normal = mat3(transpose(inverse(model))) * norm;

    // Pass the world-space position of the vertex to the fragment shader.
    // It's important to recall that this will be interpolated when it reaches the frag shader
    // .xyz makes it a vec3
    fragPos = (model * vec4(pos, 1.0f)).xyz;
}
