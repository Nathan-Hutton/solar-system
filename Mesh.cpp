#include "Mesh.h"

Mesh::Mesh()
{
    VAO = 0;
    VBO = 0;
    IBO = 0;
    indexCount = 0;
    this->texture = NULL;
}

void Mesh::createMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    indexCount = numOfIndices;

    // order that we unbind and bind matters
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(normals[0]) * 3, 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::setTexturePointer(Texture *texture)
{
    this->texture = texture;
}

void Mesh::renderMesh()
{
    texture->useTexture();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
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

void Mesh::calcAverageNormals(unsigned int *indices, unsigned int indexCount, GLfloat *vertices, unsigned int vertexCount, unsigned int vLen, unsigned int normalOffset)
{
    // Go through all the triangles
    for (size_t i = 0; i < indexCount; i+=3)
    {
        // Get each index of the triangle. Multiply it by vLen (8) so that we're pointing at the right vertex
        unsigned int in0 = indices[i] * vLen;
        unsigned int in1 = indices[i + 1] * vLen;
        unsigned int in2 = indices[i + 2] * vLen;

        // Create 2 edges of a triangle. Each line is the equivalent to subtracting the 2 points to get a vector.
        // We are using all 3 points to make 2 edges of a triangle so we can get the surface normal
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        
        // Get the cross product of these 2 edges and normalize to get the surface normal. 
        // We want to normalize since we're only concerned with the direction
        glm::vec3 normal = cross(v1, v2);
        normal = glm::normalize(normal);

        // Now we just update the surface normal in the vertices list. 
        // This will accumulate for vertices that are connected to multiple triangles.
        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    // Normalize the vertex normals. 
    for (size_t i = 0; i < vertexCount / vLen; i++)
    {
        unsigned int nOffset = i * vLen + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}


Mesh::~Mesh()
{
    clearMesh();
}
