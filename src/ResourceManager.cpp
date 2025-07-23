#include "ResourceManager.h"

#include <iostream>

namespace
{
	std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
	std::unordered_map<std::string, std::shared_ptr<Material>> materialCache;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> fileMeshCache;
	std::unordered_map<std::string, std::shared_ptr<Mesh>> sphereMeshCache;
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

	// Meshes
	bool fileMeshExists(const std::string& path)
	{
		return fileMeshCache.contains(path);
	}

	bool sphereMeshExists(float radius, int stacks, int slices, bool hasNormals)
	{
		return sphereMeshCache.contains(std::to_string(static_cast<int>(radius)) + '-' + std::to_string(stacks) + '-' + std::to_string(slices) + '-' + std::to_string(hasNormals));
	}

	std::shared_ptr<Mesh> getFileMesh(const std::string& path)
	{
		if (!fileMeshExists(path))
			throw std::runtime_error("Requested file mesh does not exist in cache: " + path);

		return fileMeshCache.at(path);
	}

	//std::shared_ptr<Mesh> getFileMesh(const std::string& path)
	//{

	//}

	std::shared_ptr<Mesh> getSphereMesh(float radius, int stacks, int slices, bool hasNormals)
	{
		if (!sphereMeshExists(radius, stacks, slices, hasNormals))
			throw std::runtime_error("Requested sphere mesh does not exist in cache: " + std::to_string(static_cast<int>(radius)) + '-' + std::to_string(stacks) + '-' + std::to_string(slices) + '-' + std::to_string(hasNormals));

		return sphereMeshCache.at(std::to_string(static_cast<int>(radius)) + '-' + std::to_string(stacks) + '-' + std::to_string(slices) + '-' + std::to_string(hasNormals));
	}

	
	void loadFileMeshIntoCache(const std::string& path)
	{
		if (fileMeshExists(path))
		{
			std::cerr << "Trying to load mesh " << path << " more than once\n";
			return;
		}

		//TODO: Finish this
	}

	void loadSphereMeshIntoCache(float radius, int stacks, int slices, bool hasNormals)
	{

		const std::string sphereName{ std::to_string(static_cast<int>(radius)) + '-' + std::to_string(stacks) + '-' + std::to_string(slices) + '-' + std::to_string(hasNormals) };
		if (sphereMeshExists(radius, stacks, slices, hasNormals))
		{
			std::cerr << "Trying to load sphere " << sphereName << " more than once\n";
			return;
		}
		std::shared_ptr<Mesh> sphereMesh{ Mesh::getSphereMesh(radius, stacks, slices, hasNormals) };
		sphereMeshCache[sphereName] = sphereMesh;
	}
}
