#include "terrain.h"

#include <algorithm>
#include <execution>
#include <math_utils.h>
#include <tinyxml2_utils.h>

#include <fileservice.h>
#include <zipservice.h>
#include <engine.h>
#include <environments/game/classes/unit.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>

#include <mouse.h>
#include <camera.h>

Terrain::Terrain(const std::string& mapSize, const bool isRandom, const uint32_t defaultTextureId) :
	TERRAIN_CHUNK_VERTICES_GAP_PX(64/ TERRAIN_SIZE_SCALE_PARAMETER),
	TERRAIN_CHUNK_SIZE(10),
	TERRAIN_CHUNK_SIZE_PX(640/ TERRAIN_SIZE_SCALE_PARAMETER),
	TERRAIN_CHUNK_NVERTICES_X(12),
	TERRAIN_CHUNK_NVERTICES_Y(12),
	TERRAIN_CHUNK_NVERTICES_TOT(144),
	isRandomScenario(isRandom),
	defaultTexId(defaultTextureId)
{
	VERTICES_POS = { 124, 123, 125, 126, 127, 128, 129, 130, 131, 132, 5, 3, 114, 113, 115, 116, 117, 118, 119, 120, 121, 122, 4, 6, 104,
				103, 105, 106, 107, 108, 109, 110, 111, 112, 7, 8, 94, 93, 95, 96, 97, 98, 99, 100, 101, 102, 9, 10, 84, 83, 85, 86, 87, 88, 89, 90, 91, 92, 11, 12, 74, 73, 75, 76, 77, 78,
				79, 80, 81, 82, 13, 14, 64, 63, 65, 66, 67, 68, 69, 70, 71, 72, 15, 16, 54, 53, 55, 56, 57, 58, 59, 60, 61, 62, 17, 18, 44, 43, 45, 46, 47, 48, 49, 50, 51, 52, 19, 20, 25,
				23, 26, 28, 30, 32, 34, 36, 38, 40, 21, 22, 24, 27, 29, 31, 33, 35, 37, 39, 41, 42, 2, 0, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 1, 143 };
	
	// if mapSize is empty => default size will be taken from terrainInfo.xml
	this->Initialize(mapSize);
}
Terrain::~Terrain() { }

bool Terrain::IsVertexInsideMap(const int x, const int y)
{
	return ((x < 0 || x >= (int)MAP_SIZE_X) || (y < 0 || y >= (int)MAP_SIZE_Y)) == false;
}
bool Terrain::TryGetVertexIndexByMapCoordinates(const int x, const int y, uint32_t* index, bool considerOnlyVisibleVertices)
{
	uint32_t CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;

	if (IsVertexInsideMap(x, y))
	{
		if (considerOnlyVisibleVertices == false)
			(*index) = x / TERRAIN_CHUNK_VERTICES_GAP_PX + y / TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
		else
			(*index) = (x + CHUNK_SIZE_FIXED) / TERRAIN_CHUNK_VERTICES_GAP_PX + (y + CHUNK_SIZE_FIXED) / TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
		return true;
	}
	return false;
}
void Terrain::SetMapSize(std::string mapSize)
{
	auto terrainInfoData = this->ReadTerrainInfoXML();
	auto& terrainInfo = std::get<0>(terrainInfoData);
	if (mapSize.empty())
		mapSize = std::get<1>(terrainInfoData);
	if (terrainInfo.contains(mapSize) == false)
		mapSize = "medium";
	N_TERRAIN_CHUNKS_X = terrainInfo[mapSize].nTerrainChunksX;
	N_TERRAIN_CHUNKS_Y = terrainInfo[mapSize].nTerrainChunksY;
	N_TERRAIN_CHUNKS_X_VISIBLE = terrainInfo[mapSize].nTerrainChunksX - 2;
	N_TERRAIN_CHUNKS_Y_VISIBLE = terrainInfo[mapSize].nTerrainChunksY - 2;
	this->MAP_SIZE = std::move(mapSize);
	N_TERRAIN_CHUNKS = N_TERRAIN_CHUNKS_X * N_TERRAIN_CHUNKS_Y;
	MAP_SIZE_X_VISIBLE = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X_VISIBLE;
	MAP_SIZE_Y_VISIBLE = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_Y_VISIBLE;
	MAP_SIZE_X = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X;
	MAP_SIZE_Y = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_Y;
	MAP_NVERTICES = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_X * N_TERRAIN_CHUNKS_Y;
	MAP_NVERTICES_X = TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_X;
	MAP_NVERTICES_Y = TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_Y;
	this->Reset();
}
void Terrain::SetTerrainVariables(void)
{
	N_TERRAIN_CHUNKS = N_TERRAIN_CHUNKS_X * N_TERRAIN_CHUNKS_Y;
	MAP_SIZE_X = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X;
	MAP_SIZE_Y = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_Y;
	MAP_SIZE_X_VISIBLE = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_X_VISIBLE;
	MAP_SIZE_Y_VISIBLE = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX * N_TERRAIN_CHUNKS_Y_VISIBLE;
	MAP_NVERTICES = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_X * N_TERRAIN_CHUNKS_Y;
	MAP_NVERTICES_X = TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_X;
	MAP_NVERTICES_Y = TERRAIN_CHUNK_SIZE * N_TERRAIN_CHUNKS_Y;
}
void Terrain::SetMapType(std::string& mapType)
{
	this->MAP_TYPE = mapType;
}
void Terrain::SetTerrainDefaultValues(void)
{
	N_TERRAIN_CHUNKS_X = 48;
	N_TERRAIN_CHUNKS_X_VISIBLE = 46;
	N_TERRAIN_CHUNKS_Y = 32;
	N_TERRAIN_CHUNKS_Y_VISIBLE = 30;
	this->SetTerrainVariables();
}
std::string Terrain::GetMapType(void)
{
	return this->MAP_TYPE;
}
std::string Terrain::GetMapSize(void)
{
	return this->MAP_SIZE;
}
void Terrain::Reset(void)
{
	TERRAIN_CHUNKS.clear();
	TERRAIN_CHUNKS = std::vector<TerrainChunk>(N_TERRAIN_CHUNKS, TerrainChunk(this));

	TERRAIN_TEXTURE_ARRAY.clear();
	TERRAIN_TEXTURE_ARRAY = std::vector<uint32_t>(MAP_NVERTICES, defaultTexId);

	TERRAIN_ZNOISE_ARRAY.clear();
	TERRAIN_ZNOISE_ARRAY = std::vector<int>(MAP_NVERTICES, (int)MIN_GRASS_HEIGHT);

	MIN_Z_REACHED = MIN_GRASS_HEIGHT;
	MAX_Z_REACHED = MIN_GRASS_HEIGHT;
}
void Terrain::Initialize(const std::string& mapSize)
{
	// if mapSize is empty => default size will be taken from terrainInfo.xml
	this->SetMapSize(mapSize);
	MIN_Z_REACHED = MIN_GRASS_HEIGHT;
	MAX_Z_REACHED = MIN_GRASS_HEIGHT;
}
void Terrain::FillTerrainWithTexture(const uint32_t texId)
{
	std::fill(TERRAIN_TEXTURE_ARRAY.begin(), TERRAIN_TEXTURE_ARRAY.end(), texId);
}
void Terrain::SetMinZ(const float z)
{
	this->MIN_Z_REACHED = std::min(this->MIN_Z_REACHED, z);
}
void Terrain::SetMaxZ(const float z)
{
	this->MAX_Z_REACHED = std::max(this->MAX_Z_REACHED, z);
}
float Terrain::GetMinZ(void)
{
	return this->MIN_Z_REACHED;
}
float Terrain::GetMaxZ(void)
{
	return this->MAX_Z_REACHED;
}
float Terrain::GetNoiseEstimate(const float x, const float y)
{
	int gap = this->TERRAIN_CHUNK_VERTICES_GAP_PX;

	// bottom left
	int x0 = (int)x;
	int y0 = (int)y;
	uint32_t j0;
	float z0 = 0.f;
	if (this->TryGetVertexIndexByMapCoordinates(x0, y0, &j0, true))
	{
		z0 = (float)this->GetVertexHeightByIndex(j0);
	}
	else
	{
		return 0.f;
	}

	// bottom right
	int x1 = x0 + gap;
	int y1 = y0;
	uint32_t j1;
	float z1 = 0.f;
	if (this->TryGetVertexIndexByMapCoordinates(x1, y1, &j1, true))
	{
		z1 = (float)this->GetVertexHeightByIndex(j1);
	}
	else
	{
		return 0.f;
	}

	// top right
	int x2 = x1;
	int y2 = y1 + gap;
	uint32_t j2;
	float z2 = 0.f;
	if (this->TryGetVertexIndexByMapCoordinates(x2, y2, &j2, true))
	{
		z2 = (float)this->GetVertexHeightByIndex(j2);
	}
	else
	{
		return 0.f;
	}

	// top left
	int x3 = x0;
	int y3 = y2;
	uint32_t j3;
	float z3 = 0.f;
	if (this->TryGetVertexIndexByMapCoordinates(x3, y3, &j3, true))
	{
		z3 = (float)this->GetVertexHeightByIndex(j3);
	}
	else
	{
		return 0.f;
	}

	// deltas
	float dx = x / gap - x0 / gap;
	float dy = y / gap - y0 / gap;

	// values 
	float v0 = z0 * (1 - dx) + z1 * dx; // bottom
	float v1 = z1 * (1 - dy) + z2 * dy; // right
	float v2 = z3 * (1 - dx) + z2 * dx; // top
	float v3 = z0 * (1 - dy) + z3 * dy; // left

	// average
	float zHat = (v0 * (1 - dy) + v1 * dx + v2 * dy + v3 * (1 - dx)) / 2.f;

	return zHat - MIN_GRASS_HEIGHT;
}
uint32_t Terrain::GetVertexIndex(const uint32_t index)
{
	if (index < 0 || index >= TERRAIN_CHUNK_NVERTICES_TOT) return 0;
	return VERTICES_POS[index];
}
int Terrain::GetVertexHeightByIndex(const uint32_t index)
{
	return TERRAIN_ZNOISE_ARRAY[index];
}
int Terrain::GetVertexTextureByIndex(const uint32_t index)
{
	return TERRAIN_TEXTURE_ARRAY[index];
}
void Terrain::SetVertexTextureAndPassable(const float value, const float xpos, const float ypos, const bool passable)
{
	uint32_t CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	uint32_t vertexIndexXInMap = (uint32_t)(std::round(xpos / TERRAIN_CHUNK_VERTICES_GAP_PX) * TERRAIN_CHUNK_VERTICES_GAP_PX);
	uint32_t vertexIndexYInMap = (uint32_t)(std::round(ypos / TERRAIN_CHUNK_VERTICES_GAP_PX) * TERRAIN_CHUNK_VERTICES_GAP_PX);

	uint32_t idx;
	if (TryGetVertexIndexByMapCoordinates(vertexIndexXInMap, vertexIndexYInMap, &idx, true))
	{
		SetVertexTextureByIndex(idx, (int)value, passable);
	}
}
void Terrain::SetVertexTexture(const float value, const float xpos, const float ypos)
{
	uint32_t CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	uint32_t vertexIndexXInMap = (uint32_t)(std::round(xpos / TERRAIN_CHUNK_VERTICES_GAP_PX) * TERRAIN_CHUNK_VERTICES_GAP_PX);
	uint32_t vertexIndexYInMap = (uint32_t)(std::round(ypos / TERRAIN_CHUNK_VERTICES_GAP_PX) * TERRAIN_CHUNK_VERTICES_GAP_PX);

	uint32_t idx;
	if (TryGetVertexIndexByMapCoordinates(vertexIndexXInMap, vertexIndexYInMap, &idx, true))
	{
		SetVertexTextureByIndex(idx, (int)value);
	}
}
void Terrain::SetVertexTextureByIndex(const uint32_t index, const int value, const bool passable)
{
	if (TERRAIN_TEXTURE_ARRAY[index] == value)
		return;

	uint32_t nverticesX = N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
	int xpos = (index % nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	int ypos = (index / nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	
	if (xpos >= 0 && ypos >= 0)
		this->surfaceGrid.lock()->SetGridCellValueByMapCoords(xpos, ypos, passable, true, GridOperations::E_GRID_OVERLAP);

	TERRAIN_TEXTURE_ARRAY[index] = value;
}
void Terrain::SetVertexTextureByIndex(const uint32_t index, const int value)
{
	if (TERRAIN_TEXTURE_ARRAY[index] == value)
		return;

	TERRAIN_TEXTURE_ARRAY[index] = value;
}
void Terrain::SetVertexZNoiseByIndex(const uint32_t index, const int value)
{
	this->SetMaxZ((float)value);
	this->SetMinZ((float)value);
	this->TERRAIN_ZNOISE_ARRAY[index] = value;
}
void Terrain::IncreaseVertexZNoiseByIndex(const uint32_t index, const int value, const int maxValue)
{
	int newValue = TERRAIN_ZNOISE_ARRAY[index] + value;
	TERRAIN_ZNOISE_ARRAY[index] = std::min(maxValue, newValue);

	bool passable = TERRAIN_ZNOISE_ARRAY[index] > SEA_HEIGHT;
	uint32_t nverticesX = N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
	int xpos = (index % nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	int ypos = (index / nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;

	SetMaxZ((float)TERRAIN_ZNOISE_ARRAY[index]);
	SetMinZ((float)TERRAIN_ZNOISE_ARRAY[index]);
	if (xpos >= 0 && ypos >= 0)
		this->surfaceGrid.lock()->SetGridCellValueByMapCoords(xpos, ypos, passable, true, GridOperations::E_GRID_OVERLAP);
}
void Terrain::DecreaseVertexZNoiseByIndex(const uint32_t index, const int value, const int minValue)
{
	int newValue = TERRAIN_ZNOISE_ARRAY[index] - value;
	TERRAIN_ZNOISE_ARRAY[index] = std::max(minValue, newValue);

	bool passable = TERRAIN_ZNOISE_ARRAY[index] > SEA_HEIGHT;
	const uint32_t nverticesX = N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
	const int xpos = (index % nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	const int ypos = (index / nverticesX) * TERRAIN_CHUNK_VERTICES_GAP_PX - TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;

	SetMinZ((float)TERRAIN_ZNOISE_ARRAY[index]);
	SetMaxZ((float)TERRAIN_ZNOISE_ARRAY[index]);
	if (xpos >= 0 && ypos >= 0)
		this->surfaceGrid.lock()->SetGridCellValueByMapCoords(xpos, ypos, passable, true, GridOperations::E_GRID_OVERLAP);
}
int Terrain::GetTerrainInfo(TerrainInfo flag)
{
	switch (flag)
	{
	case TerrainInfo_MapWidthPixel:
		return MAP_SIZE_X_VISIBLE;
	case TerrainInfo_MapHeightPixel:
		return MAP_SIZE_Y_VISIBLE;
	case TerrainInfo_TerrainChunkSizePixel:
		return TERRAIN_CHUNK_SIZE_PX;
	case TerrainInfo_NumberOfTerrainChunksX:
		return N_TERRAIN_CHUNKS_X;
	case TerrainInfo_NumberOfTerrainChunksY:
		return N_TERRAIN_CHUNKS_Y;
	case TerrainInfo_TerrainChunkSize:
		return TERRAIN_CHUNK_SIZE;
	default:
		return 0;
	}
}
uint32_t Terrain::GetVerticesGapPixel(void)
{
	return this->TERRAIN_CHUNK_VERTICES_GAP_PX;
}
uint32_t Terrain::GetNumberOfMapVertices()
{
	return this->MAP_NVERTICES;
}
uint32_t Terrain::GetNumberOfMapVerticesX()
{
	return this->MAP_NVERTICES_X;
}
uint32_t Terrain::GetNumberOfMapVerticesY()
{
	return this->MAP_NVERTICES_Y;
}
glm::vec2 Terrain::GetVertexCoordinatesByIndex(const uint32_t index)
{
	uint32_t nVerticesX = N_TERRAIN_CHUNKS_X * TERRAIN_CHUNK_SIZE;
	uint32_t CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;

	const float x = float((int)TERRAIN_CHUNK_VERTICES_GAP_PX * (index % nVerticesX) - CHUNK_SIZE_FIXED);
	const float y = float((int)TERRAIN_CHUNK_VERTICES_GAP_PX * (index / nVerticesX) - CHUNK_SIZE_FIXED);
	glm::vec2 coords{ x,y };

	return coords;
}
float Terrain::GetNoiseFromRGBColor(const float color)
{
	float z = (MAX_Z_REACHED - MIN_Z_REACHED) * (color / 255.0f) + MIN_Z_REACHED - MIN_GRASS_HEIGHT;
	//std::cout << "[DEBUG] Max Z: " << MAX_Z_REACHED << ", Min Z: " << MIN_Z_REACHED << ", Clicked Z: " << z << std::endl;
	return z;
}
int Terrain::GetMapTotalWidth()
{
	return MAP_SIZE_X;
}
int Terrain::GetMapTotalHeight()
{
	return MAP_SIZE_Y;
}
void Terrain::EnableTracing(void)
{
	this->TRACING = true;
}
void Terrain::DisableTracing(void)
{
	this->TRACING = false;
}
void Terrain::UpdateAllChunks(const UpdateTerrainChunkFlags flags)
{
	int CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;

	std::vector<int> v(N_TERRAIN_CHUNKS); // vector with TERRAIN_CHUNK_NVERTICES_TOT ints.
	std::iota(std::begin(v), std::end(v), 0); // Fill with 0, 1, ..., 99.
	auto p = std::execution::par_unseq;

	std::for_each(p, v.begin(), v.end(), [CHUNK_SIZE_FIXED, flags, this](auto&& index) //Lambda
	{
		uint32_t ix = (index % N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED;
		uint32_t iy = (index / N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED;
		TERRAIN_CHUNKS[index].Update(index, flags);
	});
}
void Terrain::UpdateChunksAroundPoint(const float x, const float y, const UpdateTerrainChunkFlags flags)
{
	auto chunks = Get4NearestChunksIndicesByMapCoords(x, y);
	auto p = std::execution::par_unseq;

	std::for_each(p, chunks.begin(), chunks.end(), [flags, this](auto&& idx) //Lambda
	{
		TERRAIN_CHUNKS[idx].Update(idx, flags);
	});
}
void Terrain::RenderAllChunks(void)
{
	int N = 0;
	int CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	for (int index = 0; index < (int)N_TERRAIN_CHUNKS; index++)
	{
		uint32_t ix = (index % N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED - CHUNK_SIZE_FIXED;
		uint32_t iy = (index / N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED - CHUNK_SIZE_FIXED;
		assert(index >= 0 && index < TERRAIN_CHUNKS.size());
		
		TERRAIN_CHUNKS[index].Render(index, N, ix, iy, false, glm::vec2(TERRAIN_CHUNK_VERTICES_GAP_PX, TERRAIN_CHUNK_VERTICES_GAP_PX), texArrayOpenglID, texArrayLength);
		N++;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void Terrain::RenderActiveChunks(const bool wireframe)
{
	int N = 0;
	int CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	int startX = int(rattlesmake::peripherals::camera::get_instance().GetXPosition() / CHUNK_SIZE_FIXED) * CHUNK_SIZE_FIXED;
	int startY = int(rattlesmake::peripherals::camera::get_instance().GetYPosition() / CHUNK_SIZE_FIXED) * CHUNK_SIZE_FIXED - CHUNK_SIZE_FIXED;
	int endX = startX + (int)rattlesmake::peripherals::viewport::get_instance().GetWidthZoomed() + CHUNK_SIZE_FIXED * 2;
	int endY = startY + (int)rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() + CHUNK_SIZE_FIXED * 2;

	for (int iy = startY; iy <= endY; iy += CHUNK_SIZE_FIXED)
	{
		for (int ix = startX; ix <= endX; ix += CHUNK_SIZE_FIXED)
		{
			auto idx_x = (ix + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED;
			auto idx_y = (iy + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED;
			if(!(idx_x >= 0 && idx_x < (int)N_TERRAIN_CHUNKS_X && idx_y >= 0 && idx_y < (int)N_TERRAIN_CHUNKS_Y))
				continue;

			int index = idx_x + idx_y * N_TERRAIN_CHUNKS_X;
			if(!(index >= 0 && index < TERRAIN_CHUNKS.size()))
				continue;

			// renderings
			TERRAIN_CHUNKS[index].Render(index, N, ix, iy, wireframe, glm::vec2(TERRAIN_CHUNK_VERTICES_GAP_PX, TERRAIN_CHUNK_VERTICES_GAP_PX), texArrayOpenglID, texArrayLength);
			N++;
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
std::vector<uint32_t>* Terrain::GetTerrainTexturesArrayPtr()
{
	return &TERRAIN_TEXTURE_ARRAY;
}
std::vector<int>* Terrain::GetTerrainHeightsArrayPtr()
{
	return &TERRAIN_ZNOISE_ARRAY;
}
void Terrain::RenderHoveredChunk(void)
{
	int CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	uint32_t idx = GetChunkIndexByMapCoords((int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate(), (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate());
	if (idx < 0 || idx >= TERRAIN_CHUNKS.size()) return;

	uint32_t ix = (idx % N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED - CHUNK_SIZE_FIXED;
	uint32_t iy = (idx / N_TERRAIN_CHUNKS_X) * CHUNK_SIZE_FIXED - CHUNK_SIZE_FIXED;
	TERRAIN_CHUNKS[idx].Render(idx, 0, ix, iy, false, glm::vec2(TERRAIN_CHUNK_VERTICES_GAP_PX, TERRAIN_CHUNK_VERTICES_GAP_PX), texArrayOpenglID, texArrayLength);
}
UnitsListIt Terrain::AddUnitToChunk(const uint32_t chunkId, Unit* u)
{
	if (chunkId < 0 || chunkId >= TERRAIN_CHUNKS.size()) return UnitsListIt();
	return TERRAIN_CHUNKS[chunkId].AddUnit(u);
}
void Terrain::RemoveUnitFromChunk(const uint32_t chunkId, Unit* u, UnitsListIt listIt)
{
	if (chunkId < 0 || chunkId >= TERRAIN_CHUNKS.size()) return;
	TERRAIN_CHUNKS[chunkId].RemoveUnit(u->GetPlayer(), listIt);
}
UnitsInChunkArray* Terrain::GetUnitsInsideChunk(const uint32_t chunkId)
{
	if (chunkId < 0 || chunkId >= TERRAIN_CHUNKS.size()) return nullptr;
	return TERRAIN_CHUNKS[chunkId].GetUnitsInside();
}
TerrainChunk* Terrain::GetTerrainChunkByIndex(const uint32_t idx)
{
	return &(TERRAIN_CHUNKS[idx]);
}
std::tuple<TerrainInfoXmlData, std::string> Terrain::ReadTerrainInfoXML()
{
	auto xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?terrain.zip", "terrainInfo.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS) return std::tuple<TerrainInfoXmlData, std::string>();

	auto _terrainInfoArray = xmlFile.FirstChildElement("terrainInfoArray");
	auto defaultSize = std::string("medium");
	auto data = TerrainInfoXmlData();
	for (tinyxml2::XMLElement* _terrainInfo = _terrainInfoArray->FirstChildElement(); _terrainInfo != NULL; _terrainInfo = _terrainInfo->NextSiblingElement())
	{
		TerrainInfoXml info = TerrainInfoXml(_terrainInfo);
		if (info.isDefault) defaultSize = info.name;
		data[info.name] = info;
	}
	return std::tuple<TerrainInfoXmlData, std::string>{data, defaultSize};
}
std::vector<uint32_t> Terrain::Get4NearestChunksIndicesByMapCoords(const float xCoord, const float yCoord)
{
	uint32_t idx = GetNearestChunkIndexByMapCoords(xCoord, yCoord);
	auto vec = std::vector<uint32_t>();
	if (idx < 0 || idx >= TERRAIN_CHUNKS.size()) return vec;
	vec.push_back(idx);
	vec.push_back(idx - N_TERRAIN_CHUNKS_X); // bottom
	vec.push_back(idx - N_TERRAIN_CHUNKS_X - 1); // bottom-left
	vec.push_back(idx - 1); // left
	return vec;
}
std::vector<uint32_t> Terrain::Get8AdjacentChunksIndicesByIndex(const int idx)
{
	auto vec = std::vector<uint32_t>();
	if (idx < 0 || idx >= TERRAIN_CHUNKS.size()) return vec;

	vec.push_back(idx - 1); // left
	vec.push_back(idx + N_TERRAIN_CHUNKS_X - 1); // top-left
	vec.push_back(idx + N_TERRAIN_CHUNKS_X); // top
	vec.push_back(idx + N_TERRAIN_CHUNKS_X + 1); // top-right
	vec.push_back(idx + 1); // right
	vec.push_back(idx - N_TERRAIN_CHUNKS_X + 1); // bottom-right
	vec.push_back(idx - N_TERRAIN_CHUNKS_X); // bottom
	vec.push_back(idx - N_TERRAIN_CHUNKS_X - 1); // bottom-left
	return vec;
}
uint32_t Terrain::GetNearestChunkIndexByMapCoords(const float xCoord, const float yCoord)
{
	float CHUNK_SIZE_FIXED = 1.f * TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	int index = int(std::round((xCoord + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED)) + int(std::round((yCoord + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED)) * N_TERRAIN_CHUNKS_X;
	return index;
}
uint32_t Terrain::GetChunkIndexByMapCoords(const int xCoord, const int yCoord)
{
	uint32_t CHUNK_SIZE_FIXED = TERRAIN_CHUNK_SIZE * TERRAIN_CHUNK_VERTICES_GAP_PX;
	int index = (xCoord + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED + (yCoord + CHUNK_SIZE_FIXED) / CHUNK_SIZE_FIXED * N_TERRAIN_CHUNKS_X;
	return index;
}
int Terrain::GetTextureByXYMapCoordinates(const uint32_t x, const uint32_t y)
{
	uint32_t index;
	if (TryGetVertexIndexByMapCoordinates(x, y, &index))
	{
		if (index < 0 || index >= TERRAIN_TEXTURE_ARRAY.size()) return VERTEX_NULL;
		return TERRAIN_TEXTURE_ARRAY[index];
	}
	return VERTEX_NULL;
}
int Terrain::GetZNoiseByXYMapCoordinates(const uint32_t x, const uint32_t y)
{
	uint32_t index;
	if (TryGetVertexIndexByMapCoordinates(x, y, &index))
	{
		if (index < 0 || index >= TERRAIN_ZNOISE_ARRAY.size()) return VERTEX_NULL;
		return TERRAIN_ZNOISE_ARRAY[index];
	}
	return VERTEX_NULL;
}
