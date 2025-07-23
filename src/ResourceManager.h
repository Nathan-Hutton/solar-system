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

	bool meshExists(const std::string& path);
	std::shared_ptr<Mesh> getMesh(const std::string& path);
	void loadMeshIntoCache(const std::string& path);
};
