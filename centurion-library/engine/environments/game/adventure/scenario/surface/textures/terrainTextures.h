/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <glm.hpp>

class TerrainTextureArray;
namespace tinyxml2 { class XMLElement; };

class TerrainTexture
{
	friend class TerrainTextureArray;
public:
	[[nodiscard]] static std::shared_ptr<TerrainTexture> Create(tinyxml2::XMLElement* el);

	[[nodiscard]] std::string GetName(void) const;
	[[nodiscard]] std::string GetCategory(void) const;
	[[nodiscard]] glm::vec3 GetColorVec3(void) const;
	[[nodiscard]] uint32_t GetColorId(void) const;
	[[nodiscard]] uint32_t GetTextureId(void) const;
	[[nodiscard]] bool IsPassable(void) const;
	~TerrainTexture(void);
private:
	TerrainTexture(tinyxml2::XMLElement* el);
	void ClearTextureData(void);
	std::string name, category;
	glm::vec3 color{ 0.f };
	uint32_t colorId = 0;
	uint32_t textureId = 0;
	bool bPassable = true;
	uint32_t texWidth, texHeight;
	uint8_t texChannels;

	// image bytes
	std::basic_string<uint8_t> textureData;
};

class TerrainTextureArray
{
public:
	TerrainTextureArray(void);
	~TerrainTextureArray(void);

	[[nodiscard]] uint32_t GetEmptyTextureId(void) const;
	[[nodiscard]] uint32_t GetGrass1TextureId(void) const;
	[[nodiscard]] uint32_t GetOpenglTextureId(void) const;
	[[nodiscard]] uint32_t GetLength(void) const;
	[[nodiscard]] std::weak_ptr<TerrainTexture> GetTerrainTextureById(const uint16_t id) const; 
	[[nodiscard]] std::weak_ptr<TerrainTexture> GetTerrainTextureByName(const std::string& _name) const;
	[[nodiscard]] std::vector<std::weak_ptr<TerrainTexture>> GetTerrainTexturesByCategory(const std::string& category) const;
	[[nodiscard]] std::weak_ptr<TerrainTexture> GetTerrainTextureByColor(const glm::vec3& colorRGB) const;

private:
	void AddTexture(const std::shared_ptr<TerrainTexture> tt);
	void BindTextures(void);
	std::unordered_map<uint32_t, std::shared_ptr<TerrainTexture>> textures; // main container
	std::vector<uint32_t> texturesId;

	// other containers
	std::unordered_map<std::string, std::weak_ptr<TerrainTexture>> texturesByName;
	std::unordered_map<uint32_t, std::weak_ptr<TerrainTexture>> texturesByColor;
	std::unordered_map<std::string, std::vector<std::weak_ptr<TerrainTexture>>> texturesByCategory;
	
	// other members
	uint32_t emptyTextureId = 0;
	uint32_t grassTextureId = 0;
	uint32_t texWidth = 0;
	uint32_t texHeight = 0;
	uint8_t texChannels = 0;
	uint32_t texCount = 0;

	// opengl
	uint32_t openglTextureArrayId = 0;
};
