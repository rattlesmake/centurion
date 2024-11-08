/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "terrainConstants.h"
#include <array>
#include <vector>
#include <list>
#include <glm.hpp>

#ifndef VERTEX_NULL
#define VERTEX_NULL 999
#endif

#ifndef HEIGHTS_TEXTURES_PACK_VALUE
#define HEIGHTS_TEXTURES_PACK_VALUE 64000.f
#endif

class Terrain;
class Unit;
class terrainchunk_shader;

typedef std::list<Unit*> UnitsList;
typedef std::list<Unit*>::const_iterator UnitsListIt;
typedef std::array<UnitsList, 8> UnitsInChunkArray;


// this class contains information about one chunk
class TerrainChunk
{
	friend class Terrain;
public:
	~TerrainChunk() {}
private:
	TerrainChunk(Terrain* terrainChunksRef);
	TerrainChunk() = delete;

	void Update(const uint32_t chunkIndex, const UpdateTerrainChunkFlags flags);
	void Render(const uint32_t index, const int n, const int xpos, const int ypos, const bool wireframe, glm::vec2 size, const uint32_t textureArrayID, const uint32_t textureCount);
	UnitsListIt AddUnit(Unit* u);
	UnitsInChunkArray* GetUnitsInside(void);
	void RemoveUnit(const uint32_t playerId, UnitsListIt listIt);

	Terrain* tcks = nullptr;
	std::vector<float> heights_textures_array; // len = 144 ; heights and textures are packed in one float
	std::vector<float> normals_array; // len = 144
	UnitsInChunkArray units_inside{};
	uint32_t texArrayId = 0, texCount = 0;

	terrainchunk_shader& terrainchunk_shd;
};

typedef std::vector<TerrainChunk> TerrainChunks;
