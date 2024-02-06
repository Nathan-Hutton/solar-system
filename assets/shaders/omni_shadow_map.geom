#version 460

layout (triangles) in;
// triangle_strip means that adding a fourth point will use points 2 and 3 to make the next triangle
// We also expect only a max of 18 vertices going out
layout (triangle_strip, max_vertices=18) out;

// Because we're rendering to every side of the cube, we setup a perspective projection and view
// pointing at every side of the cube. We'll have 6 light matrices and render to each one
uniform mat4 lightMatrices[6];

out vec4 fragPos;

void main()
{
    // gl_Layer is built into GLSL
    // The layers are like GL_TEXTURE_CUBE_MAP_POSITIVE_X (left, right, etc.)
    // GLSL thinks this is just a bunch of layers, like a stack of textures
    for (int face = 0; face < 6; face++)
    {
        gl_Layer = face;
        
        // Go through each vertex in each triangle
        for (int i = 0; i < 3; i++)
        {
            // gl_in is also built in. It refers to the triangle that came into this shader.
            // This was defined at the top with layout (triangles) in;
            // We're referencing each vertex of the triangle.
            fragPos = gl_in[i].gl_Position;
            
            // This gl_Position will not get passed to the fragment shader.
            // It's like a state that's being set for when we call EmitVertex()
            // We want to get fragPos in relation to the projection * view for the cube face
            gl_Position = lightMatrices[face] * fragPos;
            EmitVertex();
        }
        // Now that we've emiited our vertices, we need to make a primitive out of them
        EndPrimitive();
    }
}
