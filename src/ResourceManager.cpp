#include "ResourceManager.h"

#include <iostream>

namespace
{
	std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
	std::unordered_map<std::string, std::shared_ptr<Material>> materialCache;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> meshCache;
}

namespace resourceManager
{
	// Textures
	bool textureExists(const std::string& path)
	{
		return textureCache.contains(path);
	}

	std::shared_ptr<Texture> getTexture(const std::string& path)
	{
		if (!textureExists(path))
			throw std::runtime_error("Requested texture does not exist in cache: " + path);

		return textureCache.at(path);
	}

	void loadTextureIntoCache(const std::string& path)
	{
		if (textureExists(path))
		{
			std::cerr << "Trying to load texture " << path << " more than once\n";
			return;
		}

		std::shared_ptr<Texture> texture{ std::make_shared<Texture>(path) };
		texture->loadTexture();
		textureCache[path] = texture;
	}

	// Materials
	bool materialExists(const std::string& path)
	{
		return materialCache.contains(path);
	}

	std::shared_ptr<Material> getMaterial(const std::string& path)
	{
		if (!materialExists(path))
			throw std::runtime_error("Requested material does not exist in cache: " + path);

		return materialCache.at(path);
	}

	void loadMaterialIntoCache(const std::string& path)
	{
		if (materialExists(path))
		{
			std::cerr << "Trying to load material " << path << " more than once\n";
			return;
		}

		std::shared_ptr<Material> material{ std::make_shared<Material>(path) };
		materialCache[path] = material;
	}
}
