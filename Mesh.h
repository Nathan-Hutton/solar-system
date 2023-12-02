#pragma once

#include <GL/glew.h>
#include "Texture.h"

class Mesh
{
    public:
        Mesh();

        void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numIndices);
        void setTexturePointer(Texture *texture);
        void renderMesh();
        void ClearMesh();

        ~Mesh();

    private:
        GLuint VAO, VBO, IBO;
        GLsizei indexCount;
        Texture *texture;
};
