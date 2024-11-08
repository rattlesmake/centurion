/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <environments/game/adventure/scenario/surface/terrain/terrain.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainConstants.h>
#include <environments/game/adventure/scenario/surface/sea/sea.h>
#include <environments/game/adventure/scenario/surface/clouds/clouds.h>
#include <players/players_array.h>

class Engine;
class SurfaceSettings;
class TerrainTextureArray;
class SurfaceGrid;

// this class contains shared_ptrs of surface components (terrain, sea, grid+pathfinding)
// and methods that make those classes work together.
class Surface
{
	// only Scenario can access to constructor
	friend class Scenario;
public:
	Surface(const Surface& other) = delete;
	Surface& operator=(const Surface& other) = delete;
	~Surface(void);

	[[nodiscard]] std::weak_ptr<Terrain> GetTerrainW(void) const;
	[[nodiscard]] std::shared_ptr<Terrain> GetTerrain(void) const;
	[[nodiscard]] std::weak_ptr<TerrainTextureChunks> GetTerrainTextureChunks(void) const;
	[[nodiscard]] std::weak_ptr<SurfaceGrid> GetSurfaceGrid(void) const;
	[[nodiscard]] std::weak_ptr<TerrainTextureArray> GetTerrainTextureArray(void) const;

	void AssignTerrainTexture(PlayersArray& playersArray);
	void Render(const bool activeChunks, const bool wireframe, const bool bRenderSea) const;
	void RenderSurfaceGrid(void);
	void RenderClouds(void) const;

	void UpdateAllTextureChunks(void);

	[[nodiscard]] glm::vec2 GetVisibleMapSize(void) const;
	[[nodiscard]] glm::vec2 GetTotalMapSize(void) const;
	[[nodiscard]] glm::ivec2 GetNumberOfTerrainChunks(void) const;
	[[nodiscard]] uint32_t GetTerrainChunkWideness(void) const;
	[[nodiscard]] float GetTerrainChunkWidenessPx(void) const;
	[[nodiscard]] float GetGapBetweenVerticesPx(void) const;
	[[nodiscard]] bool TryGetVertexIndexByMapCoordinates(int x, int y, unsigned int* index, bool considerOnlyVisibleVertices = false) const;
	void Initialize(void);
	static void GenerateRandom(const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface);
	void ToggleGrid(void);
private:
	// lookup references
	Engine& engine;
	SurfaceSettings& surfSettings;

	// pointer members
	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<TerrainTextureChunks> terrainTextureChunks;
	std::shared_ptr<SurfaceGrid> surfaceGrid;
	std::shared_ptr<Clouds> clouds;
	std::shared_ptr<Sea> sea;

	std::shared_ptr<TerrainTextureArray> terrainTextureArray;

	glm::ivec2 numberOfTerrainChunks{ 0,0 };
	uint32_t terrainChunkWideness = 0;
	float terrainChunkWidenessPx = 0.f;
	glm::vec2 visibleMapSize{ 0,0 };
	glm::vec2 totalMapSize{ 0,0 };
	float gapBetweenVerticesPx = 0.f;
	int randomSeed = 0;

	explicit Surface(const std::string& mapSize);
	explicit Surface(const int seed);
};
