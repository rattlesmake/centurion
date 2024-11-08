#include "terrainChunk.h"
#include "terrain.h"
#include "terrainchunk_shader.h"

#include <environments/game/classes/unit.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>

#include <algorithm>
#include <execution>
#include <math_utils.h>

// class used just to do some calculations about vertices normals
class TerrainChunk_NormalsCalculation
{
public:
	static glm::vec3 GetRealVertexVec3(glm::vec2 offset, glm::vec3 centralVertex, Terrain* tcks)
	{
		int gap = (int)tcks->TERRAIN_CHUNK_VERTICES_GAP_PX;
		int newX = (int)centralVertex.x + gap * (int)offset.x;
		int newY = (int)centralVertex.y + gap * (int)offset.y;
		return glm::vec3(newX, newY + tcks->GetZNoiseByXYMapCoordinates(newX, newY), newY * (-1.f) + tcks->GetZNoiseByXYMapCoordinates(newX, newY));
	}
	static std::array<Math::Triangle, 6> GetTrianglesAroundVertex(glm::vec3 vertex, Terrain* tcks)
	{
		std::array<Math::Triangle, 6> triangles;
		triangles[0] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks),  GetRealVertexVec3(glm::vec2(-1, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(-1, 1), vertex, tcks) };
		triangles[1] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(-1, 1), vertex, tcks), GetRealVertexVec3(glm::vec2(0, 1), vertex, tcks) };
		triangles[2] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(0, 1), vertex, tcks), GetRealVertexVec3(glm::vec2(1, 0), vertex, tcks) };
		triangles[3] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(1, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(1, -1), vertex, tcks) };
		triangles[4] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(1, -1), vertex, tcks), GetRealVertexVec3(glm::vec2(0, -1), vertex, tcks) };
		triangles[5] = { GetRealVertexVec3(glm::vec2(0, 0), vertex, tcks), GetRealVertexVec3(glm::vec2(0, -1), vertex, tcks),  GetRealVertexVec3(glm::vec2(-1, 0), vertex, tcks) };
		return triangles;
	}
	static glm::vec3 CalculateNormals(const glm::vec3 vertex, Terrain* tcks)
	{
		std::array<Math::Triangle, 6> triangles = GetTrianglesAroundVertex(vertex, tcks);
		glm::vec3 sum(0.f);
		for (int k = 0; k < 6; k++) {
			sum += Math::calculate_triangle_normal(triangles[k]);
		}
		return glm::normalize(sum);
	}
};

TerrainChunk::TerrainChunk(Terrain* terrainChunksRef) :
	tcks(terrainChunksRef),
	terrainchunk_shd(terrainchunk_shader::get_instance())
{
	this->heights_textures_array = std::vector<float>(tcks->TERRAIN_CHUNK_NVERTICES_TOT, 0.f);
	this->normals_array = std::vector<float>(tcks->TERRAIN_CHUNK_NVERTICES_TOT * 3, 0.f);

	std::vector<int> v(tcks->TERRAIN_CHUNK_NVERTICES_TOT); // vector with TERRAIN_CHUNK_NVERTICES_TOT ints.
	std::iota(std::begin(v), std::end(v), 0); // Fill with 0, 1, ..., 144.
	auto p = std::execution::par_unseq;
	std::for_each(p, v.begin(), v.end(), [this](auto&& it) //Lambda
		{
			this->heights_textures_array[it] = MIN_GRASS_HEIGHT + HEIGHTS_TEXTURES_PACK_VALUE * 1.f; // 1.f = Grass
			this->normals_array[it * 3 + 0] = 0.f;
			this->normals_array[it * 3 + 1] = 0.f;
			this->normals_array[it * 3 + 2] = 0.707107f;
		});
}
void TerrainChunk::Update(const uint32_t chunkIndex, const UpdateTerrainChunkFlags flags)
{
	int chunkIndexX = chunkIndex % tcks->N_TERRAIN_CHUNKS_X;
	int chunkIndexY = chunkIndex / tcks->N_TERRAIN_CHUNKS_X;

	std::vector<int> v(tcks->TERRAIN_CHUNK_NVERTICES_TOT); // vector with TERRAIN_CHUNK_NVERTICES_TOT ints.
	std::iota(std::begin(v), std::end(v), 0); // Fill with 0, 1, ..., 144.
	auto p = std::execution::par_unseq;

	std::for_each(p, v.begin(), v.end(), [chunkIndexX, chunkIndexY, flags, this](auto&& it) //Lambda
		{
			int vertexIndexX = it % tcks->TERRAIN_CHUNK_NVERTICES_X;
			int vertexIndexY = it / tcks->TERRAIN_CHUNK_NVERTICES_X;

			glm::ivec3 vertex{};
			vertex.x = int(chunkIndexX * (int)tcks->TERRAIN_CHUNK_SIZE + vertexIndexX - 1) * (int)tcks->TERRAIN_CHUNK_VERTICES_GAP_PX;
			vertex.y = int(chunkIndexY * (int)tcks->TERRAIN_CHUNK_SIZE + vertexIndexY - 1) * (int)tcks->TERRAIN_CHUNK_VERTICES_GAP_PX;
			if ((flags & UpdateTerrainChunkFlags_Heights) || (flags & UpdateTerrainChunkFlags_Normals))
			{
				vertex.z = this->tcks->GetZNoiseByXYMapCoordinates(vertex.x, vertex.y);
			}
			if (flags & UpdateTerrainChunkFlags_Textures)
			{
				vertex.z = this->tcks->GetTextureByXYMapCoordinates(vertex.x, vertex.y);
			}

			if ((vertex.z == VERTEX_NULL) ||
				(vertex.x <= 0 || vertex.x >= (int)tcks->MAP_SIZE_X - (int)tcks->TERRAIN_CHUNK_VERTICES_GAP_PX) ||
				(vertex.y <= 0 || vertex.y >= (int)tcks->MAP_SIZE_Y - (int)tcks->TERRAIN_CHUNK_VERTICES_GAP_PX))
				return;

			if (flags & UpdateTerrainChunkFlags_Textures)
			{
				auto value = heights_textures_array[this->tcks->GetVertexIndex(it)];

				// decode
				auto m1 = HEIGHTS_TEXTURES_PACK_VALUE;
				auto texture_ = std::floor(value / m1);
				auto height_ = std::floor(value - texture_ * m1);

				vertex.z = this->tcks->GetTextureByXYMapCoordinates(vertex.x, vertex.y);

				// encode
				auto newValue = height_ + (float)vertex.z * HEIGHTS_TEXTURES_PACK_VALUE;
				heights_textures_array[this->tcks->GetVertexIndex(it)] = newValue;
			}
			if (flags & UpdateTerrainChunkFlags_Heights)
			{
				auto value = heights_textures_array[this->tcks->GetVertexIndex(it)];

				// decode
				auto m1 = HEIGHTS_TEXTURES_PACK_VALUE;
				auto texture_ = std::floor(value / m1);
				auto height_ = std::floor(value - texture_ * m1);

				vertex.z = this->tcks->GetZNoiseByXYMapCoordinates(vertex.x, vertex.y);

				// encode
				auto newValue = (float)vertex.z + texture_ * HEIGHTS_TEXTURES_PACK_VALUE;
				heights_textures_array[this->tcks->GetVertexIndex(it)] = newValue;
			}
			if (flags & UpdateTerrainChunkFlags_Normals)
			{
				glm::vec3 N = TerrainChunk_NormalsCalculation::CalculateNormals(vertex, tcks);
				normals_array[this->tcks->GetVertexIndex(it) * 3 + 0] = (float)N.x;
				normals_array[this->tcks->GetVertexIndex(it) * 3 + 1] = (float)N.y;
				normals_array[this->tcks->GetVertexIndex(it) * 3 + 2] = (float)N.z;
			}
		});
}
void TerrainChunk::Render(const uint32_t index, const int n, const int xpos, const int ypos, const bool wireframe, glm::vec2 size, const uint32_t textureArrayID, const uint32_t textureCount)
{
	if (n == 0)
	{
		terrainchunk_shd.pre_draw(textureArrayID, textureCount, size, wireframe, (float)tcks->TERRAIN_CHUNK_SIZE_PX, tcks->TRACING, tcks->MIN_Z_REACHED, tcks->MAX_Z_REACHED, PASSABLE_SEA_HEIGHT, NOT_PASSABLE_SEA_HEIGHT, MIN_GRASS_HEIGHT);
	}
	terrainchunk_shd.draw(xpos, ypos, this->heights_textures_array.data(), this->heights_textures_array.size(), this->normals_array.data(), this->normals_array.size());
}
UnitsListIt TerrainChunk::AddUnit(Unit* u)
{
	units_inside[u->GetPlayer() - 1].push_front(u);
	return units_inside[u->GetPlayer() - 1].cbegin();
}
void TerrainChunk::RemoveUnit(const uint32_t playerId, UnitsListIt listIt)
{
	units_inside[playerId - 1].erase(listIt);
}
UnitsInChunkArray* TerrainChunk::GetUnitsInside(void)
{
	return &(this->units_inside);
}
