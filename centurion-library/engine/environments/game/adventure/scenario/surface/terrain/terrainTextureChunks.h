/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <environments/game/adventure/scenario/area.h>

class Terrain;
class TerrainTextureArray;

class TerrainTextureChunks
{
	friend class Surface;

public:

	class TerrainTextureChunk : public RectangularArea
	{
	public:
		TerrainTextureChunk(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin);
		std::string GetZone() { return this->zone; }
		std::string GetSecondZone() { return this->zone2; }
		void SetZone(const std::string zone) { this->zone = zone; }
		void SetSecondZone(const std::string zone) { this->zone2 = zone; }
		bool HasZone() { return this->zone.empty() == false; }
		bool HasSecondZone() { return this->zone2.empty() == false; }
		bool IsNearToTownhall() { return this->bNearTownhall; }
		void SetAsNearToTownhall() { this->bNearTownhall = true; }
		bool IsNearToMountain() { return this->bNearMountain; }
		void SetAsNearToMountain() { this->bNearMountain = true; }
		int GetId() { return this->id; }
	private:
		std::string zone = "";
		std::string zone2 = "";
		bool bNearTownhall = false;
		bool bNearMountain = false;
		int id = 0;
	};

	void SetTextureChunks();
	void ToggleTextureChunks();
	void RenderTextureChunks();
	unsigned int GetNumberOfTextureChunks();
	int GetTextureChunkSize();
	std::optional<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> GetTextureChunkByMapCoordinates(const int xCoord, const int yCoord);
	std::optional<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> GetTextureChunkByIndex(const int idx);

	int GetTextureChunkIndexByMapCoordinates(const int xCoord, const int yCoord);
	std::vector<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> GetAdjacentTextureChunksByMapCoordinates(const int xCoord, const int yCoord);

	~TerrainTextureChunks();

private:
	unsigned int CHUNK_SIZE_X = 320;
	unsigned int CHUNK_SIZE_Y = 320;
	unsigned int N_CHUNKS_X = 95;
	unsigned int N_CHUNKS_Y = 65;
	unsigned int N_CHUNKS = N_CHUNKS_X * N_CHUNKS_Y;
	std::vector<std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk>> CHUNKS;
	bool CHUNKS_VISIBLE = false;

	void FillMixedChunkTerrainTexture(std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk> chunk, std::string zone1, std::string zone2);
	void FillChunkTerrainTexture(std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk> chunk, std::string zone);

	std::weak_ptr<Terrain> terrain;
	std::weak_ptr<TerrainTextureArray> terrainTextureArray;

	TerrainTextureChunks(const uint32_t gap, const uint32_t mapWidthVisible, std::shared_ptr<TerrainTextureArray>& _terrainTextureArray);
};

