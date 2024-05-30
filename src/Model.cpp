#include "Model.h"

#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>

Model::Model(GLfloat mass, Material* material) 
    : SpaceObject{ mass }
    , m_material { material }
    , m_scaleFactor { 1.0f }
    , m_scaleFactorVector { 1.0f }
{}

void Model::setScaleFactor(GLfloat sFactor)
{
    m_scaleFactor       = sFactor;
    m_scaleFactorVector = glm::vec3{sFactor, sFactor, sFactor};
}

void Model::loadModel(const std::string& fileName) 
{
    Assimp::Importer importer {};
    const aiScene *scene {importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)};

    if (!scene)
        throw std::runtime_error("Model " + fileName + " failed to load " + importer.GetErrorString());

    // Load the first node in the scene, and call that recursively on all nodes
    loadNode(scene->mRootNode, scene);
    loadMaterials(scene);
}

void Model::render() const
{
    for (size_t i {0}; i < m_meshList.size(); ++i)
    {
        const unsigned int materialIndex {m_meshToTex[i]};

        // Check if it's possible for the material to be inside the textureList
        // Then check if that index is null
        if (materialIndex < m_textureList.size() && m_textureList[materialIndex])
            m_textureList[materialIndex]->useTexture();

        m_meshList[i]->render();
    }
}

void Model::setWorldProperties(glm::mat4& model)
{
    model = glm::translate(model, m_position);
    model = glm::rotate(model, glm::radians(m_angle), m_rotation);
    model = glm::scale(model, m_scaleFactorVector);
}

//void Model::clearModel()
//{
//    for (size_t i {0}; i < m_meshList.size(); ++i)
//    {
//        if (m_meshList[i])
//        {
//            delete m_meshList[i];
//            m_meshList[i] = nullptr;
//        }
//    }
//
//    for (size_t i {0}; i < m_textureList.size(); ++i)
//    {
//        if (m_textureList[i])
//        {
//            delete m_textureList[i];
//            m_textureList[i] = nullptr;
//        }
//    }
//}

void Model::loadNode(aiNode* node, const aiScene* scene)
{
    // Load meshes for the node
    for(size_t i {0}; i < node->mNumMeshes; ++i)
    {
        // node only stores the ID of the mesh, the scene stores the data
        loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    // Recursively call loadNode on all children
    for (size_t i {0}; i < node->mNumChildren; ++i)
    {
        loadNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(aiMesh* mesh, const aiScene* scene)
{
    // Each vertex will contain 3 float values
    std::vector<GLfloat> vertices {};
    std::vector<unsigned int> indices {};

    for(size_t i {0}; i < mesh->mNumVertices; ++i)
    {
        // With 'insert' we can append all the data from an array to our vector
        vertices.insert(vertices.end(), {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z});

        // If we are using a texture with this mesh
        if(mesh->mTextureCoords[0]) 
            vertices.insert(vertices.end(), {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y});
        else
            vertices.insert(vertices.end(), {0.0f, 0.0f});
        
        // Insert normals. We negate the normals because of the way the Udemy course is doing things
        vertices.insert(vertices.end(), {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z});
    }

    // Each face contains the 3 indices, so we set up our indices with the faces
    for (size_t i {0}; i < mesh->mNumFaces; ++i)
    {
        const aiFace face {mesh->mFaces[i]};
        for (size_t j {0}; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // Get rid of this if we ever make collision detection
    // We're calculating the greatest distance between vectors in the model
    for (int i {0}; i < vertices.size(); i+=8)
    {
        const glm::vec3 vertex1 {vertices[i], vertices[i+1], vertices[i+2]};
        for (int j {0}; i < vertices.size(); ++i)
        {
            const glm::vec3 vertex2 {vertices[j], vertices[j+1], vertices[j+2]};
            const GLfloat displacementVectorLength {glm::length(vertex1 - vertex2)};
            m_greatestDistanceBetweenVertices = std::max(m_greatestDistanceBetweenVertices, displacementVectorLength);
        }
    }

    Mesh *newMesh {new Mesh()};
    newMesh->createMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
    m_meshList.push_back(newMesh);
    m_meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene* scene)
{
    m_textureList.resize(scene->mNumMaterials);

    for (size_t i {0}; i < scene->mNumMaterials; ++i)
    {
        aiMaterial* material {scene->mMaterials[i]};

        m_textureList[i] = nullptr;

        // Diffuse is the standard texture
        if(material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            // Handle the case where the person who made the texture used an absolute path relative to their system (dumb)
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
                continue;

            // Chop off the parts of the path that don't matter
            // This is complicated because they may be on linux or windows
            int idx             = std::string{path.data}.rfind("\\");
            const int idx1      = std::string{path.data}.rfind("/");
            idx                 = std::max(idx, idx1);

            const std::string filename {std::string{path.data}.substr(idx + 1)};

            const std::string textPath {"../assets/textures/" + filename};
            m_textureList[i] = new Texture{textPath};

            if (!m_textureList[i]->loadTexture())
            {
                delete m_textureList[i];
                m_textureList[i] = nullptr;
                throw std::runtime_error("Failed to load texture at " + textPath);
            }
        }

        if (!m_textureList[i])
        {
            m_textureList[i] = new Texture{"../assets/textures/plain.png"};
            m_textureList[i]->loadTexture();
        }
    }
}

void Model::setUniformVariables(GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
    m_specularIntensityLocation   = uniformSpecularIntensity;
    m_shininessLocation           = uniformShininess;
}
