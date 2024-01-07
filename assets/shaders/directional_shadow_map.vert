#version 330

layout (location = 0) in vec3 pos;

// Need to know where the vertex is in world space
uniform mat4 model;

// Need pov of world from the light (orthogonal)
uniform mat4 directionalLightTransform; // Will be projection * view

void main()
{
    // directionalLightTransform is projection * view
    // This applies projection and view matrices as if light source is the camera
    // Then we just apply the shader to each vertex of each model like we normally would
    gl_Position = directionalLightTransform * model * vec4(pos, 1.0); 
}
