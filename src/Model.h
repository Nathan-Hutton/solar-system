#pragma once

#include <glm/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"

// A mesh is a collection of vertices whereas a model is either a mesh or a collection
// of meshes that represents a more advanced object
// For example, a model of a human will have meshes for the limbs, torso, head, etc.
class Model
{
    public:
        Model();
        Model(GLfloat mass, glm::vec3 position=glm::vec3(0.0f, 0.0f, 0.0f));

        void loadModel(const std::string& fileName);
        void renderModel();
        void clearModel();

        GLfloat getMass();
        glm::vec3 getPosition() const;
        void setPosition(glm::vec3 position);
        glm::vec3 getVelocity();
        void setVelocity(glm::vec3 velocity);
        glm::vec3 getRotation() const;
        void setRotation(glm::vec3 rotation);
        GLfloat getAngle();
        void setAngle(GLfloat angle);
        GLfloat getRotationSpeed();
        void setRotationSpeed(GLfloat speed);


        ~Model();

    private:
        // We tend to call each mesh in the tree a node
        // The node won't store the mesh, just a reference to it
        // Scene holds all the data from a 3D scene or model, node holds data to a specific node
        void loadNode(aiNode *node, const aiScene *scene);
        void loadMesh(aiMesh *mesh, const aiScene *scene);
        // Materials contain data for textures, colors, shaders, etc.
        void loadMaterials(const aiScene *scene);

        std::vector<Mesh*> meshList;
        std::vector<Texture*> textureList;
        // We'll create one of each texture and if a mesh uses the same texture 
        // as another, they'll just say which one it's using instead of remaking the texture
        std::vector<unsigned int> meshToTex; 

        GLfloat mass;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotation;
        GLfloat angle;
        GLfloat rotationSpeed;

};
