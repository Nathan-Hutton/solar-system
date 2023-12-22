#include "Model.h"

Model::Model()
{

}

void Model::loadModel(const std::string& fileName)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    if (!scene)
    {
        printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
        return;
    }

    // Load the first node in the scene, and call that recursively on all nodes
    loadNode(scene->mRootNode, scene);
    loadMaterials(scene);
}

void Model::renderModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        unsigned int materialIndex = meshToTex[i];

        // Check if it's possible for the material to be inside the textureList
        // Then check if that index is null
        if (materialIndex < textureList.size() && textureList[materialIndex])
        {
            textureList[materialIndex]->useTexture();
        }

        meshList[i]->RenderMesh();
    }
}

void Model::clearModel()
{
    for (size_t i = 0; i < meshList.size(); i++)
    {
        if (meshList[i])
        {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }

    for (size_t i = 0; i < textureList.size(); i++)
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
    for(size_t i = 0; i < node->mNumMeshes; i++)
    {
        // node only stores the ID of the mesh, the scene stores the data
        loadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    // Recursively call loadNode on all children
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        loadNode(node->mChildren[i], scene);
    }
}

void Model::loadMesh(aiMesh *mesh, const aiScene *scene)
{
    // Each vertex will contain 3 float values
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    for(size_t i = 0; i < mesh->mNumVertices; i++)
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
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh *newMesh = new Mesh();
    // In memory, the vectors are like an array, so we can just give it vertices[0] and whatnot
    newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
    meshList.push_back(newMesh);
    meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene *scene)
{
    // Lets us not have to use push_back, it sets the size of the array
    textureList.resize(scene->mNumMaterials);

    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* material = scene->mMaterials[i];

        textureList[i] = nullptr;

        // Diffuse is the standard texture
        if(material->GetTextureCount(aiTextureType_DIFFUSE))
        {
            // Handle the case where the person who made the texture used an absolute path relative to their system (dumb)
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) != AI_SUCCESS)
                continue;

            int idx = std::string(path.data).rfind("\\");
            std::string filename = std::string(path.data).substr(idx + 1);

            std::string textPath = std::string("../assets/textures/"), filename;

            textureList[i] = new Texture(textPath.c_str());

            if (!textureList[i]->loadTexture())
            {
                printf("Failed to load texture at %s\n", textPath);
                delete textureList[i];
                textureList[i] = nullptr;
            }
        }

        if (!textureList[i])
        {
            textureList[i] = new Texture("../assets/textures/plain.png");
            textureList[i]->loadTexture();
        }
    }
}

Model::~Model()
{
}
