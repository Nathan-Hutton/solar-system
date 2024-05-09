#include "Model.h"
#include <iostream>

Model::Model() : SpaceObject()
{
    this->material          = nullptr;
    this->scaleFactor       = 1.0f;
    this->scaleFactorVector = glm::vec3{1.0f};
}

Model::Model(GLfloat mass) : SpaceObject(mass)
{
    this->material          = nullptr;
    this->scaleFactor       = 1.0f;
    this->scaleFactorVector = glm::vec3{1.0f};
}

void Model::setScaleFactor(GLfloat sFactor)
{
    this->scaleFactor       = sFactor;
    this->scaleFactorVector = glm::vec3{sFactor, sFactor, sFactor};
}

GLfloat Model::getScaleFactor()
{
    return scaleFactor;
}

glm::vec3 Model::getScaleFactorVector()
{
    return scaleFactorVector;
}

void Model::loadModel(const std::string& fileName) {
    Assimp::Importer importer {};
    const aiScene *scene {importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices)};

    if (!scene)
    {
        std::cerr << "Model " << fileName.c_str() << " failed to load " << importer.GetErrorString() << '\n';
        return;
    }

    // Load the first node in the scene, and call that recursively on all nodes
    loadNode(scene->mRootNode, scene);
    loadMaterials(scene);
}

void Model::render()
{
    for (size_t i {0}; i < meshList.size(); i++)
    {
        unsigned int materialIndex {meshToTex[i]};

        // Check if it's possible for the material to be inside the textureList
        // Then check if that index is null
        if (materialIndex < textureList.size() && textureList[materialIndex])
            textureList[materialIndex]->useTexture();

        meshList[i]->render();
    }
}

void Model::setWorldProperties(glm::mat4* model)
{
    *model = glm::translate(*model, position);
    *model = glm::rotate(*model, glm::radians(angle), rotation);
    *model = glm::scale(*model, scaleFactorVector);
}

void Model::clearModel()
{
    for (size_t i {0}; i < meshList.size(); i++)
    {
        if (meshList[i])
        {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }

    for (size_t i {0}; i < textureList.size(); i++)
    {
        if (textureList[i])
        {
            delete textureList[i];
            textureList[i] = nullptr;
        }
    }
}

void Model::loadNode(aiNode *node, const aiScene *scene)
{
    // Load meshes for the node
    for(size_t i {0}; i < node->mNumMeshes; i++)
    {
        // node only stores the ID of the mesh, the scene stores the data
        loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    // Recursively call loadNode on all children
    for (size_t i {0}; i < node->mNumChildren; i++)
    {
        loadNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(aiMesh *mesh, const aiScene *scene)
{
    // Each vertex will contain 3 float values
    std::vector<GLfloat> vertices {};
    std::vector<unsigned int> indices {};

    for(size_t i {0}; i < mesh->mNumVertices; i++)
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
    for (size_t i {0}; i < mesh->mNumFaces; i++)
    {
        aiFace face {mesh->mFaces[i]};
        for (size_t j {0}; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Get rid of this if we ever make collision detection
    // We're calculating the greatest distance between vectors in the model
    for (int i {0}; i < vertices.size(); i+=8)
    {
        glm::vec3 vertex1 {vertices[i], vertices[i+1], vertices[i+2]};
        for (int j {0}; i < vertices.size(); i++)
        {
            glm::vec3 vertex2 {vertices[j], vertices[j+1], vertices[j+2]};
            GLfloat displacementVectorLength {glm::length(vertex1 - vertex2)};
            greatestDistanceBetweenVertices = std::max(greatestDistanceBetweenVertices, displacementVectorLength);
        }
    }

    Mesh *newMesh {new Mesh()};
    newMesh->createMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
    meshList.push_back(newMesh);
    meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene *scene)
{
    textureList.resize(scene->mNumMaterials);

    for (size_t i {0}; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material {scene->mMaterials[i]};

        textureList[i] = nullptr;

        // Diffuse is the standard texture
        if(material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            // Handle the case where the person who made the texture used an absolute path relative to their system (dumb)
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
                continue;

            // Chop off the parts of the path that don't matter
            // This is complicated because they may be on linux or windows
            int idx     = std::string(path.data).rfind("\\");
            int idx1    = std::string(path.data).rfind("/");
            idx         = std::max(idx, idx1);

            std::string filename {std::string(path.data).substr(idx + 1)};

            std::string textPath {std::string("../assets/textures/") + filename};
            textureList[i] = new Texture{textPath.c_str()};

            if (!textureList[i]->loadTexture())
            {
                std::cerr << "Failed to laod teture at " << textPath.c_str() << '\n';
                delete textureList[i];
                textureList[i] = nullptr;
            }
        }

        if (!textureList[i])
        {
            textureList[i] = new Texture{"../assets/textures/plain.png"};
            textureList[i]->loadTexture();
        }
    }
}

void Model::setUniformVariables(GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
    specularIntensityLocation   = uniformSpecularIntensity;
    shininessLocation           = uniformShininess;
}

Model::~Model()
{}
