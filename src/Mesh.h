#pragma once

#include <GL/glew.h>
#include "Texture.h"
#include "Material.h"

class Mesh
{
    public:
        Mesh();

        void createMesh(const GLfloat* const vertices, const GLuint* const indices, 
                GLuint numOfVertices, GLuint numIndices,
                bool hasNormals=true, bool threeVertices=true);
        void render() const;

        ~Mesh();

    private:
        GLuint VAO {}, VBO {}, IBO {};
        GLsizei indexCount {};
        Texture *texture {};

        void clearMesh();
};
