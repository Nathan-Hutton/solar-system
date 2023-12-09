#pragma once

#include <GL/glew.h>
#include "Texture.h"
#include <glm/glm.hpp>

class Mesh
{
    public:
        Mesh();

        void createMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numIndices);
        void setTexturePointer(Texture *texture);
        void renderMesh();
        void clearMesh();
        void calcAverageNormals(unsigned int *indices, unsigned int indexCount, GLfloat *vertices, unsigned int vertexCount, unsigned int vLen, unsigned int normalOffset);

        ~Mesh();

    private:
        GLuint VAO, VBO, IBO;
        GLsizei indexCount;
        Texture *texture;
};
