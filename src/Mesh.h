#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Material.h"

class Mesh
{
    public:
        Mesh();

        void createMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numIndices);
        void setTexturePointer(Texture *texture);
        void setMaterialPointer(Material *material);
        void renderMesh();
        void clearMesh();

        ~Mesh();

    private:
        GLuint VAO, VBO, IBO;
        GLsizei indexCount;
        Texture *texture;
        Material *material;
};
