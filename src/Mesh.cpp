#include "Mesh.h"

#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(const GLfloat* const vertices, const GLuint* const indices,
		GLsizei numVertices, GLsizei numIndices,
		bool hasNormals, bool threeVertices)
	: m_indexCount{ numIndices }
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numIndices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numVertices, vertices, GL_STATIC_DRAW);

    const int vertexSize {threeVertices ? 3 : 2};
    const int stride{ static_cast<int>(sizeof(GLfloat)) * (vertexSize + (hasNormals ? 5 : 2)) };

    glVertexAttribPointer(0, vertexSize, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(vertices[0]) * vertexSize));
    glEnableVertexAttribArray(1);

    if (hasNormals)
    {
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(vertices[0]) * 5));
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

std::shared_ptr<Mesh> Mesh::getSphereMesh(float radius, int stacks, int slices, bool hasNormals)
{
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	for (int i {0}; i <= stacks; ++i) {
		const float V {i / static_cast<float>(stacks)};
		const float phi {V * glm::pi<float>()};

		for (int j {0}; j <= slices; ++j) {
			const float U {j / static_cast<float>(slices)};
			const float theta {U * glm::pi<float>() * 2};

			const float x {cosf(theta) * sinf(phi) * radius};
			const float y {cosf(phi) * radius};
			const float z {sinf(theta) * sinf(phi) * radius};

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// Calculate texture coordinates
			vertices.push_back(U);
			vertices.push_back(V);

			if (hasNormals)
			{
				glm::vec3 normal{x,y,z};
				normal = glm::normalize(-normal);
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);
			}
		}
	}

	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int first = (i * (slices + 1)) + j;
			int second = first + slices + 1;

			indices.push_back(first);
			indices.push_back(first + 1);
			indices.push_back(second);

			indices.push_back(second);
			indices.push_back(first + 1);
			indices.push_back(second + 1);
		}
	}

	return std::make_shared<Mesh>( vertices.data(), indices.data(), static_cast<GLsizei>(vertices.size()), static_cast<GLsizei>(indices.size()), hasNormals );
}

void Mesh::render() const
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
    if (m_IBO != 0) {
        glDeleteBuffers(1, &m_IBO);
        m_IBO = 0;
    }

    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    m_indexCount = 0;
}
