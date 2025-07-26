#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

// TODO: Add shaders here
namespace resourceManager
{
	bool textureExists(const std::string& path);
	std::shared_ptr<Texture> getTexture(const std::string& path);
	void loadTextureIntoCache(const std::string& path);

	bool materialExists(const std::string& path);
	std::shared_ptr<Material> getMaterial(const std::string& path);
	void loadMaterialIntoCache(const std::string& path);

	bool fileMeshExists(const std::string& path);
	bool sphereMeshExists(float radius, int stacks, int slices, bool hasNormals);
	std::shared_ptr<Mesh> getFileMesh(const std::string& path);
	std::shared_ptr<Mesh> getSphereMesh(float radius, int stacks, int slices, bool hasNormals);
	void loadFileMeshIntoCache(const std::string& path);
	void loadSphereMeshIntoCache(float radius, int stacks, int slices, bool hasNormals);
};
