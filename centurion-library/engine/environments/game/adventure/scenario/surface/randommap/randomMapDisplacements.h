/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

class Surface;

class RandomMapDisplacements
{
public:
	struct Object
	{
		std::string Class;
		glm::vec2 Offset;
	};

	struct Texture
	{
		glm::vec3 Color;
		glm::vec2 Offset;
	};

	class ObjectArray
	{
	public:
		ObjectArray();
		std::vector<Object> Displacements;
		std::vector<Texture> Textures;
		glm::ivec2 AreaSize;
	};

	RandomMapDisplacements() {}
	void Initialize(const std::shared_ptr<Surface>& surface);

	std::unordered_map<std::string, ObjectArray> Strongholds;
	std::unordered_map<std::string, ObjectArray> Villages;
	std::unordered_map<std::string, ObjectArray> DecorationsArea;

private:
	void GetDisplacementsFromAreas(const std::shared_ptr<Surface>& surface);
};
