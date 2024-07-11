#version 460

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;
                                                                
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

// Responsible for scaling/rotating/moving our objects (put them in world space instead of local space)
uniform mat4 model;

// A view matrix is a type of lookat matrix that transforms coordinates (vertices) from world space to
// view (camera) space. It effectively moves and rotates the entire world so the camera is at the origin 
// of the view space looking down the negative z axis. Can also imagine the coordinate space is being
// manipulated instead of the vertices themselves to achieve the same effect.
// The view space axes are the up, front, and right vectors. The view matrix transforms the scene to
// align with these axes, enabling the rendering of the scene from the camera's perspective.
// The projection defines how this view matrix is projected onto the 2D screen.
uniform mat4 view;

// Responsible for taking care of our perspective projection (or possibly orthographic). 
// Determines how 3D scene is projected onto our 2D screen. That's why it`s called a projection matrix. 
// Persective projections have an FOV. Also responsible for clipping (defined by far and near plane).
// This is different from the clipping performed by the fragment shader.
// Puts things into clip space. In clip space, the hardware can easily perform clipping for us between stages.
uniform mat4 projection;

void main()
{
    // Built in GLSL variable that holds the final position of the vertex.
    // This is the key output (not passed to fragment shader) that the program
    // Uses to make the primitives, which rasterization uses to get fragments out of
    // We multiply all this in reverse order (CBAv instead of vABC) since OpenGL uses column vectors
    gl_Position = projection * view * model * vec4(pos, 1.0);
    
    // Pass the texture coordinates to the fragment shader
    texCoord = tex;
    normal = mat3(transpose(inverse(model))) * norm;

    // Pass the world-space position of the vertex to the fragment shader.
    // It's important to recall that this will be interpolated when it reaches the frag shader
    // .xyz makes it a vec3
    fragPos = (model * vec4(pos, 1.0f)).xyz;
}
