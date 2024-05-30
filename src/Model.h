#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SpaceObject.h"
#include "Mesh.h"

// A mesh is a collection of vertices whereas a model is either a mesh or a collection
// of meshes that represents a more advanced object
// For example, a model of a human will have meshes for the limbs, torso, head, etc.
class Model : public SpaceObject
{
    public:
        explicit Model(GLfloat mass=1.0f, Material* material = new Material{});

        void setScaleFactor(GLfloat sFactor);
        void loadModel(const std::string& fileName);
        void render() const override;
        void setWorldProperties(glm::mat4& model) override;
        void setUniformVariables(GLuint uniformSpecularIntesnity, GLuint uniformShininess) override;
        GLfloat getCollisionDistance() const override { return m_greatestDistanceBetweenVertices; }
        //void clearModel();

    private:
        Material* m_material {};
        GLuint m_specularIntensityLocation {}, m_shininessLocation {};
        // Since this isn't a sphere where we just set the radius, we may want to use a glm::scale
        GLfloat m_scaleFactor {};
        glm::vec3 m_scaleFactorVector {};
        // We tend to call each mesh in the tree a node
        // The node won't store the mesh, just a reference to it
        // Scene holds all the data from a 3D scene or model, node holds data to a specific node
        void loadNode(aiNode* node, const aiScene* scene);
        void loadMesh(aiMesh* mesh, const aiScene* scene);
        // Materials contain data for textures, colors, shaders, etc.
        void loadMaterials(const aiScene* scene);

        std::vector<Mesh*> m_meshList {};
        std::vector<Texture*> m_textureList {};
        // We'll create one of each texture and if a mesh uses the same texture 
        // as another, they'll just say which one it's using instead of remaking the texture
        std::vector<unsigned int> m_meshToTex {}; 
        GLfloat m_greatestDistanceBetweenVertices {};
};
