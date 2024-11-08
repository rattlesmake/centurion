#include "terrainTextureChunks.h"
#include "terrainConstants.h"

#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>

#include <header.h>
#include <engine.h>
#include <math_utils.h>

#include <viewport.h>
#include <camera.h>

TerrainTextureChunks::TerrainTextureChunks(
	const uint32_t gap, 
	const uint32_t mapWidthVisible, std::shared_ptr<TerrainTextureArray>& _terrainTextureArray) :
	terrainTextureArray(_terrainTextureArray)
{
	CHUNK_SIZE_X = gap * 5; // 5 because is 10/2 (the chunk is 10x10)
	CHUNK_SIZE_Y = gap * 5;
	N_CHUNKS_X = mapWidthVisible / CHUNK_SIZE_X + 4;
	N_CHUNKS_Y = mapWidthVisible / CHUNK_SIZE_Y + 4;
	N_CHUNKS = N_CHUNKS_X * N_CHUNKS_Y;

	CHUNKS.clear();

	std::string defaultZoneStr;
	auto defaultZone = SurfaceSettings::GetInstance().GetDefaultTerrainZone();
	if (defaultZone.has_value())
		defaultZoneStr = defaultZone.value().lock()->GetName();

	for (int iy = 0; iy < (int)N_CHUNKS_Y; iy++)
	{
		for (int ix = 0; ix < (int)N_CHUNKS_X; ix++)
		{
			float startX = float(ix * (int)CHUNK_SIZE_X - (int)CHUNK_SIZE_X * 2);
			float startY = float(iy * (int)CHUNK_SIZE_Y - (int)CHUNK_SIZE_Y * 2);

			std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk> chunk = std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk>(new TerrainTextureChunk(glm::ivec2(startX, startY), CHUNK_SIZE_X, CHUNK_SIZE_Y, AreaOrigin::BottomLeft));
			chunk->SetZone(defaultZoneStr);
			CHUNKS.push_back(chunk);
		}
	}
}
TerrainTextureChunks::~TerrainTextureChunks()
{
}

void TerrainTextureChunks::FillChunkTerrainTexture(std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk> chunk, std::string zone)
{
	if (chunk == nullptr || zone.empty()) return;
	assert(terrainTextureArray.expired() == false);
	const auto sp_terrainTextureArray = terrainTextureArray.lock();

	auto tZone = SurfaceSettings::GetInstance().GetTerrainZoneByName(zone);
	assert(tZone.has_value() && !tZone.value().expired());

	for (auto const& idx : chunk->GetVerticesInside())
	{
		if (this->terrain.lock()->GetVertexTextureByIndex(idx) != (int)sp_terrainTextureArray->GetEmptyTextureId()) continue;
		auto rndomTexName = tZone.value().lock()->ExtractRandomTexture();

		auto newTex = sp_terrainTextureArray->GetTerrainTextureByName(rndomTexName);
		if (newTex.expired() == false)
		{
			int newTexID = newTex.lock()->GetTextureId();
			int newTexPassable = newTex.lock()->IsPassable();
			this->terrain.lock()->SetVertexTextureByIndex(idx, newTexID, newTexPassable);
		}
	}
}
void TerrainTextureChunks::FillMixedChunkTerrainTexture(std::shared_ptr<TerrainTextureChunks::TerrainTextureChunk> chunk, std::string zone1, std::string zone2)
{
	if (chunk == nullptr || zone1.empty() || zone2.empty()) return;

	std::optional<std::weak_ptr<SurfaceSettings::TerrainZone>> tZones[2] = { SurfaceSettings::GetInstance().GetTerrainZoneByName(zone1), SurfaceSettings::GetInstance().GetTerrainZoneByName(zone2) };
	assert(tZones[0].has_value() && !tZones[0].value().expired());
	assert(tZones[1].has_value() && !tZones[1].value().expired());

	auto textureArray = Engine::GetInstance().GetSurface().lock()->GetTerrainTextureArray();
	assert(textureArray.expired() == false);
	
	for (auto const& idx : chunk->GetVerticesInside())
	{
		if (this->terrain.lock()->GetVertexTextureByIndex(idx) != (int)textureArray.lock()->GetEmptyTextureId()) continue;

		int which = Math::runif_discrete(0, 1);
		auto rndomTexName = tZones[which].value().lock()->ExtractRandomTexture();

		auto newTex = textureArray.lock()->GetTerrainTextureByName(rndomTexName);
		assert(newTex.expired() == false);

		int newTexID = newTex.lock()->GetTextureId();
		int newTexPassable = newTex.lock()->IsPassable();
		this->terrain.lock()->SetVertexTextureByIndex(idx, newTexID, newTexPassable);
	}
}
void TerrainTextureChunks::SetTextureChunks()
{
	for (int i = 0; i < (int)N_CHUNKS; i++)
	{
		// check if the chunk is "borderline"
		/*TerrainTextureChunks::TerrainTextureChunk* rightChunk = GetTextureChunkByMapCoordinates(CHUNKS[i].GetDrawingStartCoords().x + GetTextureChunkSize(), CHUNKS[i].GetDrawingStartCoords().y);
		TerrainTextureChunks::TerrainTextureChunk* upChunk = GetTextureChunkByMapCoordinates(CHUNKS[i].GetDrawingStartCoords().x, CHUNKS[i].GetDrawingStartCoords().y + GetTextureChunkSize());
		TerrainTextureChunks::TerrainTextureChunk* leftChunk = GetTextureChunkByMapCoordinates(CHUNKS[i].GetDrawingStartCoords().x - GetTextureChunkSize(), CHUNKS[i].GetDrawingStartCoords().y);
		TerrainTextureChunks::TerrainTextureChunk* downChunk = GetTextureChunkByMapCoordinates(CHUNKS[i].GetDrawingStartCoords().x, CHUNKS[i].GetDrawingStartCoords().y - GetTextureChunkSize());

		if (rightChunk != nullptr && CHUNKS[i].GetZone() != rightChunk->GetZone()) CHUNKS[i].SetSecondZone(rightChunk->GetZone());
		else if (upChunk != nullptr && CHUNKS[i].GetZone() != upChunk->GetZone()) CHUNKS[i].SetSecondZone(upChunk->GetZone());
		else if (leftChunk != nullptr && CHUNKS[i].GetZone() != leftChunk->GetZone()) CHUNKS[i].SetSecondZone(leftChunk->GetZone());
		else if (downChunk != nullptr && CHUNKS[i].GetZone() != downChunk->GetZone()) CHUNKS[i].SetSecondZone(downChunk->GetZone());*/

		FillChunkTerrainTexture(CHUNKS[i], CHUNKS[i]->GetZone());

		// update textures
		/*if (CHUNKS[i].HasSecondZone() == false)
		{
			FillChunkTerrainTexture(&(CHUNKS[i]), CHUNKS[i].GetZone());
		}
		else
		{
			FillMixedChunkTerrainTexture(&(CHUNKS[i]), CHUNKS[i].GetZone(), CHUNKS[i].GetSecondZone());
		}*/
	}
}
void TerrainTextureChunks::ToggleTextureChunks()
{
	CHUNKS_VISIBLE = !CHUNKS_VISIBLE;
}
void TerrainTextureChunks::RenderTextureChunks()
{
	if (CHUNKS_VISIBLE)
	{
		int startX = int(rattlesmake::peripherals::camera::get_instance().GetXPosition() / CHUNK_SIZE_X) * CHUNK_SIZE_X;
		int startY = int(rattlesmake::peripherals::camera::get_instance().GetYPosition() / CHUNK_SIZE_Y) * CHUNK_SIZE_Y;
		for (int iy = startY - CHUNK_SIZE_Y * 2; iy <= startY + rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() + CHUNK_SIZE_Y * 2; iy += CHUNK_SIZE_Y)
		{
			//if (iy < 0) continue;
			for (int ix = startX - CHUNK_SIZE_X * 2; ix <= startX + rattlesmake::peripherals::viewport::get_instance().GetWidthZoomed() + CHUNK_SIZE_X * 2; ix += CHUNK_SIZE_X)
			{
				auto chunk = GetTextureChunkByMapCoordinates(ix, iy);
				assert(chunk.has_value()); // nullptr chunk !
				
				chunk.value().lock()->Render();
			}
		}
	}
}
unsigned int TerrainTextureChunks::GetNumberOfTextureChunks()
{
	return N_CHUNKS;
}
int TerrainTextureChunks::GetTextureChunkSize()
{
	return CHUNK_SIZE_X;
}
std::optional<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> TerrainTextureChunks::GetTextureChunkByMapCoordinates(const int xCoord, const int yCoord)
{
	auto idx = GetTextureChunkIndexByMapCoordinates(xCoord, yCoord);
	if (idx < 0 || idx >= CHUNKS.size()) return {};

	return CHUNKS[idx];
}
std::optional<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> TerrainTextureChunks::GetTextureChunkByIndex(const int idx)
{
	if (idx < 0 || idx >= (int)N_CHUNKS) return {};
	return CHUNKS[idx];
}
int TerrainTextureChunks::GetTextureChunkIndexByMapCoordinates(const int xCoord, const int yCoord)
{
	int ix = xCoord / (int)CHUNK_SIZE_X + 2;
	int iy = yCoord / (int)CHUNK_SIZE_Y + 2;
	if (ix < 0 || ix >(int)N_CHUNKS_X - 1) return -1;
	if (iy < 0 || iy >(int)N_CHUNKS_Y - 1) return -1;
	int idx = iy * N_CHUNKS_X + ix;
	return idx;
}
std::vector<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>> TerrainTextureChunks::GetAdjacentTextureChunksByMapCoordinates(const int xCoord, const int yCoord)
{
	auto v = std::vector<std::weak_ptr<TerrainTextureChunks::TerrainTextureChunk>>();
	auto idx = GetTextureChunkIndexByMapCoordinates(xCoord, yCoord);
	if (idx < 0 || idx >= CHUNKS.size()) return v;

	v.push_back(CHUNKS[idx - 1]); // left
	v.push_back(CHUNKS[idx + N_CHUNKS_X - 1]); // top-left
	v.push_back(CHUNKS[idx + N_CHUNKS_X]); // top
	v.push_back(CHUNKS[idx + N_CHUNKS_X + 1]); // top-right
	v.push_back(CHUNKS[idx + 1]); // right
	v.push_back(CHUNKS[idx - N_CHUNKS_X + 1]); // bottom-right
	v.push_back(CHUNKS[idx - N_CHUNKS_X]); // bottom
	v.push_back(CHUNKS[idx - N_CHUNKS_X - 1]); // bottom-left
	return v;
}

TerrainTextureChunks::TerrainTextureChunk::TerrainTextureChunk(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin) 
	: RectangularArea(position, width, height, origin)
{
}
