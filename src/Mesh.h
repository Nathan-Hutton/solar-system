#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>

class Mesh
{
    public:
        Mesh(const GLfloat* const vertices, const GLuint* const indices,
				GLsizei numVertices, GLsizei numIndices,
				bool hasNormals=true, bool threeVertices=true);

		static std::shared_ptr<Mesh> getSphereMesh(float radius, int stacks, int slices, bool hasNormals=true);

        void render() const;

        ~Mesh();

    private:
        GLuint m_VAO {}, m_VBO {}, m_IBO {};
        GLsizei m_indexCount {};
};
