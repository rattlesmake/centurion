/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#ifndef MAX_REACHABLE_HEIGHT
#define MAX_REACHABLE_HEIGHT 1000
#endif

#ifndef MAX_REACHABLE_HEIGHT_BRUSH
#define MAX_REACHABLE_HEIGHT_BRUSH 200
#endif

#ifndef MIN_REACHABLE_HEIGHT
#define MIN_REACHABLE_HEIGHT 0
#endif

#ifndef MIN_GRASS_HEIGHT
#define MIN_GRASS_HEIGHT 100
#endif

#ifndef SEA_HEIGHT
#define SEA_HEIGHT 90
#endif

#ifndef CLOUDS_HEIGHT
#define CLOUDS_HEIGHT 10000
#endif

#ifndef PASSABLE_SEA_HEIGHT
#define PASSABLE_SEA_HEIGHT 80
#endif 

#ifndef NOT_PASSABLE_SEA_HEIGHT
#define NOT_PASSABLE_SEA_HEIGHT 20
#endif 

#include <unordered_map>
#include <tinyxml2_utils.h>

typedef int TerrainInfo;
enum TerrainInfo_
{
	TerrainInfo_MapWidthPixel = 0,
	TerrainInfo_MapHeightPixel = 1,
	TerrainInfo_TerrainChunkSizePixel = 2,
	TerrainInfo_NumberOfTerrainChunksX = 3,
	TerrainInfo_NumberOfTerrainChunksY = 4,
	TerrainInfo_TerrainChunkSize = 5,
};

typedef int UpdateTerrainChunkFlags;
enum UpdateTerrainChunkFlags_
{
	UpdateTerrainChunkFlags_None = 0,
	UpdateTerrainChunkFlags_Normals = 1,
	UpdateTerrainChunkFlags_Heights = 2,
	UpdateTerrainChunkFlags_Textures = 3,
	UpdateTerrainChunkFlags_Everything = UpdateTerrainChunkFlags_Normals | UpdateTerrainChunkFlags_Heights | UpdateTerrainChunkFlags_Textures
};


class TerrainInfoXml // this class contains the information about terrain info from terrainInfo.xml
{
public:
	TerrainInfoXml() {}
	TerrainInfoXml(tinyxml2::XMLElement* el)
	{
		this->name = el->Attribute("name");
		this->isDefault = el->BoolAttribute("isDefault");
		this->nTerrainChunksX = el->FirstChildElement("sizeX")->IntText();
		this->nTerrainChunksY = el->FirstChildElement("sizeY")->IntText();
	}
	uint32_t nTerrainChunksX = 0;
	uint32_t nTerrainChunksY = 0;
	std::string name = "";
	bool isDefault = false;
};
typedef std::unordered_map<std::string, TerrainInfoXml> TerrainInfoXmlData;
