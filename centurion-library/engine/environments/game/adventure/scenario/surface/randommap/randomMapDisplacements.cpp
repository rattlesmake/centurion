#include "randomMapDisplacements.h"

#include <engine.h>
#include <environments/game/igame.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <fileservice.h>
#include <zipservice.h>
#include <png.h>

void RandomMapDisplacements::Initialize(const std::shared_ptr<Surface>& surface)
{
	this->GetDisplacementsFromAreas(surface);
}
float make_even(float n)
{
	return (float)((int)n - (int)n % 2);
}
void AddTextures(unsigned char* texturesImage, int nChannels, glm::ivec2 areaCenter, float areaWidth, float areaHeight, int gapVertices, glm::vec2 totalMapSize, std::vector<RandomMapDisplacements::Texture>* objArrayTextures)
{
	areaWidth = make_even(areaWidth);
	areaHeight = make_even(areaHeight);

	for (int ix = areaCenter.x - areaWidth / 2 - 1; ix <= areaCenter.x + areaWidth / 2; ix++)
	{
		for (int iy = areaCenter.y - areaHeight / 2 - 1; iy <= areaCenter.y + areaHeight / 2; iy++)
		{
			RandomMapDisplacements::Texture t = RandomMapDisplacements::Texture();
			t.Offset = glm::vec2((ix - areaCenter.x) * gapVertices, (iy - areaCenter.y) * gapVertices);

			int idx = ix + iy * (int)totalMapSize.x / gapVertices;

			t.Color.r = texturesImage[idx * nChannels];
			t.Color.g = texturesImage[idx * nChannels + 1];
			t.Color.b = texturesImage[idx * nChannels + 2];
			if (t.Color.r != 0 || t.Color.g != 0 || t.Color.b != 0)
			{
				objArrayTextures->push_back(t);
			}
		}
	}
}

void GetDisplacements(const std::shared_ptr<Surface>& surface, unsigned char* texturesImage, int nChannels, tinyxml2::XMLElement* area, std::unordered_map<std::string, RandomMapDisplacements::ObjectArray> * objectArray, const std::string& areaName)
{
	auto totalMapSize = surface->GetTotalMapSize();
	auto gapVertices = int(surface->GetGapBetweenVerticesPx());
	auto chunkWideness = int(surface->GetTerrainChunkWidenessPx() / gapVertices);

	(*objectArray)[areaName] = RandomMapDisplacements::ObjectArray();
	(*objectArray)[areaName].AreaSize = glm::ivec2(tinyxml2::TryParseIntAttribute(area, "width"), tinyxml2::TryParseIntAttribute(area, "height"));
	if ((*objectArray)[areaName].AreaSize.x <= 0 || (*objectArray)[areaName].AreaSize.y <= 0)
		return;

	// save objects offset
	for (tinyxml2::XMLElement* _o = area->FirstChildElement("object"); _o != NULL; _o = _o->NextSiblingElement())
	{
		RandomMapDisplacements::Object o = RandomMapDisplacements::Object();
		o.Class = _o->Attribute("class");
		o.Offset = glm::vec2(_o->IntAttribute("xOffset"), _o->IntAttribute("yOffset"));

		(*objectArray)[areaName].Displacements.push_back(o);
	}

	// save textures offset
	int areaWidth = (int)std::round(area->IntAttribute("width") / gapVertices);
	int areaHeight = (int)std::round(area->IntAttribute("height") / gapVertices);
	auto areaCenter = glm::ivec2(
		std::round(area->IntAttribute("x") / gapVertices) + chunkWideness,
		std::round(area->IntAttribute("y") / gapVertices) + chunkWideness);

	AddTextures(texturesImage, nChannels, areaCenter, areaWidth, areaHeight, gapVertices, totalMapSize, &((*objectArray)[areaName].Textures));
}

void RandomMapDisplacements::GetDisplacementsFromAreas(const std::shared_ptr<Surface>& surface)
{
	auto totalMapSize = surface->GetTotalMapSize();
	auto gapVertices = int(surface->GetGapBetweenVerticesPx());
	auto chunkWideness = int(surface->GetTerrainChunkWidenessPx() / gapVertices);

	std::string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "randommap/areaArray.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS) return;

	// read textures
	rattlesmake::image::stb::flip_vertically_on_load(1);
	int imageXSize, imageYSize, n;
	unsigned char* texturesImage = rattlesmake::services::zip_service::get_instance().get_image_data("?data.zip", "randommap/textures.png", &imageXSize, &imageYSize, &n, 0);

	auto areas = xmlFile.FirstChildElement("areas");
	assert(areas != NULL);

	// for cycle in areas
	for (tinyxml2::XMLElement* area = areas->FirstChildElement("area"); area != NULL; area = area->NextSiblingElement())
	{
		std::string areaName = area->Attribute("name");

		if (areaName.starts_with("DecorationsArea"))
		{
			GetDisplacements(surface, texturesImage, n, area, &(this->DecorationsArea), areaName);
		}
		else if (areaName.starts_with("Stronghold"))
		{
			GetDisplacements(surface, texturesImage, n, area, &(this->Strongholds), areaName);
		}
		else if (areaName.starts_with("Village"))
		{
			GetDisplacements(surface, texturesImage, n, area, &(this->Villages), areaName);
		}
	}

	rattlesmake::image::stb::free(texturesImage);
	rattlesmake::image::stb::flip_vertically_on_load(0);
}

RandomMapDisplacements::ObjectArray::ObjectArray()
{
	Displacements = std::vector<Object>();
	Textures = std::vector<Texture>();
}



