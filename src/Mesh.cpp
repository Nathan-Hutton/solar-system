#include "Mesh.h"

Mesh::Mesh()
    : m_VAO { 0 }
    , m_VBO { 0 }
    , m_IBO { 0 }
    , m_indexCount { 0 }
{}

void Mesh::createMesh(const GLfloat* const vertices, const GLuint* const indices, 
        GLuint numOfVertices, GLuint numOfIndices,
        bool hasNormals, bool threeVertices)
{
    m_indexCount = numOfIndices;

    // VBOs store the vertex data itself whereas VAOs are like the instructions for how to interpret that data.
    // A single VAO can reference multiple VBOs.

    // Create ID for VAO. VAOs cache all the state needed for vertex input, reducing number of state
    // changes and function calls to render objects, improving performance.
    // Once the VAO is binded, you can bind VBOs and IBOs and they'll all work together
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Generate buffer object ID, which stores an array of unformatted memory allocated by OpenGL context.
    // Stores vertex data, pixel data, etc. that the GPU needs.
    // Gives an efficient way to transfer data between CPU and GPU

    // Store the index data in an index buffer object so we can do an indexed draw (reuse vertices)
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    // Store the vertex data in a vertex buffer object.
    // VBO stores vertex data, texture coordinates, normals, and other per-vertex data
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    // For glVertexAttribPointer:
    // Specify the location and data format of vertex attribute data in the VBO.
    // Basically, we're telling OpenGL how to interpret the data in the VBO.
    // We specify where the attribute starts, how many components are in each attribute,
    // the type of data, whether it's normalized, the byte offset between consecutive 
    // vertex attributes, and a pointer to the first one.
    // We've bound the shader programs which have the attributes specified with the 'layout' keyword.
    // The shader gets sent these attributes when we call glDrawElements after binding the VAO and IBO again.

    // ****************************
    // Create the vertex attributes
    // ****************************
    // Specify how to find the vertex location attribute

    // In the case where we're just rendering the quad for post-processing effects,
    // we just use 2 values for each vertex since it's a 2D shape
    const int vertexSize {threeVertices ? 3 : 2};
    const int stride { hasNormals ? static_cast<int>(sizeof(GLfloat)) * (vertexSize + 5) : static_cast<int>(sizeof(GLfloat)) * (vertexSize + 2) }; // + 2 is for the UV coords

    // Not all objects need normals. Suns and quads used for post-processing don't need them
    glVertexAttribPointer(0, vertexSize, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    // Specify how to find the vertex texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(vertices[0]) * vertexSize));
    glEnableVertexAttribArray(1);

    // Specify how to find the vertex normal attribute
    if (hasNormals)
    {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(vertices[0]) * 5));
        glEnableVertexAttribArray(2);
    }

    // Unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::render() const
{
    // Bind the VAO and IBO
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    // ==========================================================
    // Summary of each major component of the rendering pipeline:
    // ==========================================================

    /*
    1. Vertex shader: handles UV coords, normals, etc. Uutputs transformed vertices.
    OpenGL will automatically give the vertex shader the vertex attributes for each
    vertex defined by the VAO. This is what the 'layout' keyword means. Vertex shader
    can choose to pass these (usually processed) values along to the fragment shader 
    with the 'out' keyword.
    When data is passed from the vertex shader to the fragment shader, it's interpolated.
    In our case, we do pass things like the normals, texCoord, and vec positions
     
    2. Primitive assembly: After this the vertices are assembled into primitives (triangles 
    based on drawing mode.
    VAO determines which vertices and in what order to make the primitives. VAO doesn't store the
    vertices but just how to work with them from the VBO(s).
    The value passed out of the vertex shader that makes up the primitives is gl_Position
    
    2. Rasterization: Turns the primitives into fragments, which correspond to pixels on the screen.
    Interpolates vertex attributes like color, texture coordinates, and normals for each fragment
    
    3. Fragment shader: Receives each fragment and calculates the final color and property
    of each pixel. involves textures, lighting calculations, and per-pixel effects.
    The fragment shader knows nothing about vertices, it just has the interpolated fragments.
    
    Per-fragment operations: After the fragment shader, we do things like the depth test and 
    alpha blending. Final result is the processed fragment which are written to a framebuffer
    creating the image we see on the screen
    
    OpenGL gives us a default framebuffer that the output is written to which is what we see
    on our screen. We can create custom framebuffers which can handle off-screen things. For
    instance, we can render to a texture, which is what we do for shadow mapping and post-
    processing effects.
    */

    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

    // Unbind VOA and IBO
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
    if (m_IBO != 0) {
        glDeleteBuffers(1, &m_IBO);
        m_IBO = 0;
    }

    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    m_indexCount = 0;
}
