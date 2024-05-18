#pragma once

#include <GL/glew.h>
#include "Texture.h"
#include "Material.h"

class Mesh
{
    public:
        Mesh();

        void createMesh(GLfloat *vertices, GLuint *indices, 
                GLuint numOfVertices, GLuint numIndices,
                bool hasNormals=true, bool threeVertices=true);
        void setMaterialPointer(Material *material);
        void render();

        ~Mesh();

    private:
        GLuint VAO {}, VBO {}, IBO {};
        GLsizei indexCount {};
        Texture *texture {};
        Material *material {};

        void clearMesh();
};
