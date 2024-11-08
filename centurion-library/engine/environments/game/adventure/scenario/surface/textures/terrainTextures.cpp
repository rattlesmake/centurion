#include "terrainTextures.h"

#include <math_utils.h>
#include <encode_utils.h>
#include <tinyxml2.h>
#include <tinyxml2_utils.h>
#include <zipservice.h>
#include <fileservice.h>
#include <png.h>
#include <environments/game/editor/editor.h>

#pragma region Texture
std::shared_ptr<TerrainTexture> TerrainTexture::Create(tinyxml2::XMLElement* el)
{
	auto tt = std::shared_ptr<TerrainTexture>(new TerrainTexture(el));
	return tt;
}
TerrainTexture::TerrainTexture(tinyxml2::XMLElement* el)
{
	this->textureId = tinyxml2::TryParseIntAttribute(el, "id");
	this->name = tinyxml2::TryParseStrAttribute(el, "name");
	Encode::ToLowerCase(this->name);
	this->category = tinyxml2::TryParseStrAttribute(el, "category");
	this->bPassable = tinyxml2::TryParseBoolAttribute(el, "passable");

	// color
	auto _color = el->FirstChildElement("color");
	if (_color != NULL)
	{
		glm::vec3 color{ 0.f };
		color.r = (float)tinyxml2::TryParseIntAttribute(_color, "r");
		color.g = (float)tinyxml2::TryParseIntAttribute(_color, "g");
		color.b = (float)tinyxml2::TryParseIntAttribute(_color, "b");
		this->color = color;
	}
	this->colorId = Math::ConvertColorToId(this->color);

	// read texture
	auto path = tinyxml2::TryParseStrAttribute(el, "path");
	int width, height, n_channels;
	uint8_t* data = rattlesmake::services::zip_service::get_instance().get_image_data("?terrain.zip", path, &width, &height, &n_channels, (int)rattlesmake::image::png_type::RGBA);
	this->texWidth = (uint32_t)width;
	this->texHeight = (uint32_t)height;
	this->texChannels = (uint8_t)n_channels;
	this->textureData = std::basic_string<uint8_t>(data, (size_t)width * height * n_channels);
	rattlesmake::image::stb::free(data);
}
TerrainTexture::~TerrainTexture(void)
{
}
void TerrainTexture::ClearTextureData(void)
{
	this->textureData.clear();
}
std::string TerrainTexture::GetName(void) const
{
	return this->name;
}
std::string TerrainTexture::GetCategory(void) const
{
	return this->category;
}
glm::vec3 TerrainTexture::GetColorVec3(void) const
{
	return this->color;
}
uint32_t TerrainTexture::GetColorId(void) const
{
	return this->colorId;
}
uint32_t TerrainTexture::GetTextureId(void) const
{
	return this->textureId;
}
bool TerrainTexture::IsPassable(void) const
{
	return this->bPassable;
}
#pragma endregion

TerrainTextureArray::TerrainTextureArray(void)
{
	auto xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?terrain.zip", "textures/terrainTextures.xml");

	tinyxml2::XMLDocument xmlFile;
	auto error = xmlFile.Parse(xmlText.c_str());
	if (error != tinyxml2::XML_SUCCESS)
		throw std::exception("Textures XML file is corrupted.");

	// read and prepare textures
	tinyxml2::XMLElement* _terrains = xmlFile.FirstChildElement("terrainTextures");
	for (tinyxml2::XMLElement* _terr = _terrains->FirstChildElement(); _terr != NULL; _terr = _terr->NextSiblingElement())
	{
		auto tt = TerrainTexture::Create(_terr);
		this->AddTexture(tt);
	}

	// sort texture ids
	std::sort(this->texturesId.begin(), this->texturesId.end());

	// bind textures
	this->BindTextures();
}
TerrainTextureArray::~TerrainTextureArray(void)
{
	glDeleteTextures(1, &this->openglTextureArrayId);
}
void TerrainTextureArray::AddTexture(const std::shared_ptr<TerrainTexture> tt)
{
	if (tt->name == "empty_texture") this->emptyTextureId = tt->textureId;
	if (tt->name == "grass_1") this->grassTextureId = tt->textureId;

	this->textures[tt->textureId] = tt;
	this->texturesByName[tt->name] = tt;
	this->texturesByColor[tt->colorId] = tt;
	this->texturesByCategory[tt->category].push_back(tt);
	this->texturesId.push_back(tt->textureId);

	if (this->textures.size() == 1)
	{
		this->texWidth = tt->texWidth;
		this->texHeight = tt->texHeight;
		this->texChannels = tt->texChannels;
	}

	// all textures MUST be of the same format
	assert(this->texWidth == tt->texWidth);
	assert(this->texHeight == tt->texHeight);
	assert(this->texChannels == tt->texChannels);

	if (tt->category != "none") Editor::AddEditorTerrainTreeElement(tt->GetCategory(), tt->GetName());

	this->texCount++;
}

void TerrainTextureArray::BindTextures(void)
{
	GLsizei width = this->texWidth;
	GLsizei height = this->texHeight;
	GLsizei layerCount = this->texCount;
	GLsizei mipLevelCount = 1;

	glGenTextures(1, &this->openglTextureArrayId);
	glBindTexture(GL_TEXTURE_2D_ARRAY, this->openglTextureArrayId);
	
	// Allocate the storage.
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, width, height, layerCount);

	// Upload pixel data.
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, width, height, layerCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	uint32_t counter = 0;
	for (uint32_t const textureId : this->texturesId)
	{
		auto data = this->textures[textureId]->textureData.data();
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, counter, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		this->textures[textureId]->ClearTextureData();
		counter++;
	}

	// Always set reasonable texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

#pragma region Texture Array Getters

uint32_t TerrainTextureArray::GetEmptyTextureId(void) const
{
	return this->emptyTextureId;
}
uint32_t TerrainTextureArray::GetGrass1TextureId(void) const
{
	return this->grassTextureId;
}
uint32_t TerrainTextureArray::GetOpenglTextureId(void) const
{
	return this->openglTextureArrayId;
}
uint32_t TerrainTextureArray::GetLength(void) const
{
	return this->texCount;
}
std::weak_ptr<TerrainTexture> TerrainTextureArray::GetTerrainTextureById(const uint16_t id) const
{
	if (id >= this->textures.size())
		return std::weak_ptr<TerrainTexture>();
	return this->textures.at(id);
}
std::weak_ptr<TerrainTexture> TerrainTextureArray::GetTerrainTextureByName(const std::string& _name) const
{
	if (this->texturesByName.contains(_name) == false)
		return std::weak_ptr<TerrainTexture>();
	return this->texturesByName.at(_name);
}
std::vector<std::weak_ptr<TerrainTexture>> TerrainTextureArray::GetTerrainTexturesByCategory(const std::string& category) const
{
	if (this->texturesByCategory.contains(category) == false)
		return std::vector<std::weak_ptr<TerrainTexture>>();
	return this->texturesByCategory.at(category);
}
std::weak_ptr<TerrainTexture> TerrainTextureArray::GetTerrainTextureByColor(const glm::vec3& colorRGB) const
{
	auto colorID = Math::ConvertColorToId(colorRGB);
	if (this->texturesByColor.contains(colorID) == false)
		return std::weak_ptr<TerrainTexture>();
	return this->texturesByColor.at(colorID);
}
#pragma endregion


