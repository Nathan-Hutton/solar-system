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
        GLuint m_VAO {}, m_VBO {}, m_IBO {};
        GLsizei m_indexCount {};
        Texture *m_texture {};
};
