#include "Material.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

Material::Material(const std::string& filePath)
{
	std::ifstream file{ filePath };
	nlohmann::json materialJson;
	file >> materialJson;

	if (!materialJson.contains("specularIntensity"))
	{
		std::cerr << "Material " << filePath << " contains no specular intensity\n";
		std::exit(EXIT_FAILURE);
	}

	if (!materialJson.contains("shine"))
	{
		std::cerr << "Material " << filePath << " contains no shine\n";
		std::exit(EXIT_FAILURE);
	}

	m_specularIntensity = materialJson["specularIntensity"];
	m_shininess = materialJson["shine"];
}

void Material::useMaterial(GLuint specularIntensityLocation, GLuint shininessLocation) const
{
    glUniform1f(specularIntensityLocation, m_specularIntensity);
    glUniform1f(shininessLocation, m_shininess);
}
