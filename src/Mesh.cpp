#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
    this->texture = NULL;
    this->material = NULL;
}

void Mesh::createMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    indexCount = numOfIndices;

    // VBOs store the vertex data itself whereas VAOs are like the instructions for how to interpret that data

    // Create ID for VAO. VAOs cache all the state needed for vertex input, reducing number of state
    // changes and function calls to render objects, improving performance.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate buffer object ID, which stores an array of unformatted memory allocated by OpenGL context.
    // Stores vertex data, pixel data, etc. that the GPU needs.
    // Gives an efficient way to transfer data between CPU and GPU

    // Store the index data in an index buffer object so we can do an indexed draw (reuse vertices)
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    // Store the vertex data in a vertex buffer object.
    // VBO stores vertex data, texture coordinates, normals, and other per-vertex data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    // For glVertexAttribPointer:
    // Specify the location and data format of vertex attribute data in the VBO.
    // Basically, we're telling OpenGL how to interpret the data in the VBO.
    // We specify where the attribute starts, how many components are in each attribute,
    // the type of data, whether it's normalized, the byte offset between consecutive 
    // vertex attributes, and a pointer to the first one.
    // We've bound the shader programs which have the attributes specified with the 'layout' keyword.
    // The shader gets sent these attributes when we call glDrawElements after binding the VAO and IBO again.

    // Specify how to find the vertex location attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    glEnableVertexAttribArray(0);
    // Specify how to find the vertex texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);
    // Specify how to find the vertex normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    // Unbind everything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Texture *Mesh::getTexturePointer()
{
    return texture;
}

void Mesh::setTexturePointer(Texture *texture)
{
    this->texture = texture;
}

void Mesh::setMaterialPointer(Material *material)
{
    this->material = material;
}

void Mesh::renderMesh()
{
    // Bind our texture so that OpenGL knows which one to use
    texture->useTexture();

    // Bind the VAO and IBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Draw. OpenGL will automatically give the shader the vertex attributes 
    // for each vertex defined by the VAO
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    // Unbind VOA and IBO
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::clearMesh()
{
    if (IBO =! 0) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO =! 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO =! 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}

Mesh::~Mesh()
{
    clearMesh();
}
