#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Material.h"

class Mesh
{
    public:
        Mesh();

        void createMesh(GLfloat *vertices, unsigned int *indices, 
                unsigned int numOfVertices, unsigned int numIndices,
                bool hasNormals=true, bool threeVertices=true);
        Texture *getTexturePointer();
        void setTexturePointer(Texture *texture);
        void setMaterialPointer(Material *material);
        void render();
        void clearMesh();

        ~Mesh();

    private:
        GLuint VAO, VBO, IBO;
        GLsizei indexCount;
        Texture *texture;
        Material *material;
};
