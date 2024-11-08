/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#ifndef TERRAIN_SIZE_SCALE_PARAMETER
#define TERRAIN_SIZE_SCALE_PARAMETER 1
#endif


#pragma once

#include <header.h>
#include "terrainChunk.h"

class SurfaceGrid;
class TerrainBrush;

// class "container" of vector of chunks
class Terrain
{
	friend class TerrainBrush;
	friend class TerrainChunk;
	friend class TerrainChunk_NormalsCalculation;

	// only Surface can access to constructor
	friend class Surface;
public:
	bool IsVertexInsideMap(const int x, const int y);	
	void SetMapSize(std::string mapSize = "");
	void SetMapType(std::string& mapType);
	std::string GetMapType(void);
	std::string GetMapSize(void);
	void Reset(void);

	bool TryGetVertexIndexByMapCoordinates(const int x, const int y, uint32_t* index, bool considerOnlyVisibleVertices = false);
	void FillTerrainWithTexture(const uint32_t texId);
	void SetMinZ(const float z);
	void SetMaxZ(const float z);
	float GetMinZ(void);
	float GetMaxZ(void);
	float GetNoiseEstimate(const float x, const float y);
	int GetVertexHeightByIndex(const uint32_t index);
	int GetVertexTextureByIndex(const uint32_t index);
	void SetVertexTextureAndPassable(const float value, const float xpos, const float ypos, const bool passable);
	void SetVertexTexture(const float value, const float xpos, const float ypos);
	void SetVertexTextureByIndex(const uint32_t index, const int value, const bool passable);
	void SetVertexTextureByIndex(const uint32_t index, const int value);
	void SetVertexZNoiseByIndex(const uint32_t index, const int value);
	void IncreaseVertexZNoiseByIndex(const uint32_t index, const int value, const int maxValue = MAX_REACHABLE_HEIGHT);
	void DecreaseVertexZNoiseByIndex(const uint32_t index, const int value, const int minValue = MIN_GRASS_HEIGHT);
	
	uint32_t GetNumberOfMapVerticesX();
	uint32_t GetNumberOfMapVerticesY();
	uint32_t GetNumberOfMapVertices();
	glm::vec2 GetVertexCoordinatesByIndex(const uint32_t index);

	float GetNoiseFromRGBColor(const float color);

	int GetMapTotalWidth();
	int GetMapTotalHeight();

	void EnableTracing(void);
	void DisableTracing(void);

	void UpdateAllChunks(const UpdateTerrainChunkFlags flags);
	void UpdateChunksAroundPoint(const float x, const float y, const UpdateTerrainChunkFlags flags);
	void RenderAllChunks(void);
	void RenderActiveChunks(const bool wireframe);
	void RenderHoveredChunk(void);

	std::vector<uint32_t>* GetTerrainTexturesArrayPtr();
	std::vector<int>* GetTerrainHeightsArrayPtr();

	UnitsListIt AddUnitToChunk(const uint32_t chunkId, Unit* u);
	void RemoveUnitFromChunk(const uint32_t chunkId, Unit* u, UnitsListIt listIt);
	UnitsInChunkArray* GetUnitsInsideChunk(const uint32_t chunkId);

	std::vector<uint32_t> Get4NearestChunksIndicesByMapCoords(const float xCoord, const float yCoord);
	std::vector<uint32_t> Get8AdjacentChunksIndicesByIndex(const int idx);
	uint32_t GetNearestChunkIndexByMapCoords(const float xCoord, const float yCoord);
	uint32_t GetChunkIndexByMapCoords(const int xCoord, const int yCoord);

	// these const variables are initialized in the constructor
	// in the .cpp
	const uint32_t TERRAIN_CHUNK_VERTICES_GAP_PX; // spazio tra i vertici del chunk orizzontalmente o verticalmente
	const uint32_t TERRAIN_CHUNK_SIZE; // ampiezza del chunk 
	const uint32_t TERRAIN_CHUNK_SIZE_PX;
	const uint32_t TERRAIN_CHUNK_NVERTICES_X; // numero di vertici nel chunk lungo l'asse x (giunture incluse)
	const uint32_t TERRAIN_CHUNK_NVERTICES_Y; // numero di vertici nel chunk lungo l'asse y (giunture incluse)
	const uint32_t TERRAIN_CHUNK_NVERTICES_TOT; // numero di vertici nel chunk totali (giunture incluse)

	~Terrain();

private:
	
	int GetTerrainInfo(TerrainInfo flag);
	uint32_t GetVerticesGapPixel(void);

	void Initialize(const std::string& mapSize);

	// there variables depend on the map size
	uint32_t N_TERRAIN_CHUNKS_X = 48* TERRAIN_SIZE_SCALE_PARAMETER; // numero di chunks lungo l'asse x
	uint32_t N_TERRAIN_CHUNKS_X_VISIBLE = 48* TERRAIN_SIZE_SCALE_PARAMETER -2; // numero di chunks lungo l'asse x
	uint32_t N_TERRAIN_CHUNKS_Y = 32* TERRAIN_SIZE_SCALE_PARAMETER; // numero di chunks lungo l'asse y
	uint32_t N_TERRAIN_CHUNKS_Y_VISIBLE = 32* TERRAIN_SIZE_SCALE_PARAMETER -2; // numero di chunks lungo l'asse y
	uint32_t N_TERRAIN_CHUNKS; // numero di chunks totali nella mappa
	uint32_t MAP_SIZE_X; // larghezza effettiva della mappa
	uint32_t MAP_SIZE_Y; // altezza effettiva della mappa
	uint32_t MAP_SIZE_X_VISIBLE; // larghezza effettiva della mappa
	uint32_t MAP_SIZE_Y_VISIBLE; // altezza effettiva della mappa
	uint32_t MAP_NVERTICES;
	uint32_t MAP_NVERTICES_X;
	uint32_t MAP_NVERTICES_Y;
	void SetTerrainVariables(void);
	void SetTerrainDefaultValues(void);

	// terrain data
	std::vector<uint32_t> VERTICES_POS; // length = 144 (1 chunk)
	uint32_t GetVertexIndex(const uint32_t index);

	// Z-Noise array
	std::vector<int> TERRAIN_ZNOISE_ARRAY; // length = 144 * nchunks
	int GetZNoiseByXYMapCoordinates(const uint32_t x, const uint32_t y);

	// Texture array
	std::vector<uint32_t> TERRAIN_TEXTURE_ARRAY; // length = 144 * nchunks
	int GetTextureByXYMapCoordinates(const uint32_t x, const uint32_t y);

	// Scenario data
	float MIN_Z_REACHED = 0.f;
	float MAX_Z_REACHED = 0.f;

	// Tracing
	bool TRACING = false;

	bool isRandomScenario = false;

	std::string MAP_TYPE = ""; // continente/montagnosa/...
	std::string MAP_SIZE = ""; // small/normal/huge

	// Vector of TerrainChunk
	TerrainChunks TERRAIN_CHUNKS;
	TerrainChunk* GetTerrainChunkByIndex(const uint32_t idx);

	std::tuple<TerrainInfoXmlData, std::string> ReadTerrainInfoXML();

	// Texture Array Data (for rendering)
	uint32_t texArrayOpenglID = 0;
	uint32_t texArrayLength = 0;
	uint32_t defaultTexId = 0;

	std::weak_ptr<SurfaceGrid> surfaceGrid;

	Terrain(const std::string& mapSize, const bool isRandom, const uint32_t defaultTextureId);
};
