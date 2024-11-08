#include "surface.h"
#include "surfaceSettings.h"

#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainBrush.h>
#include <environments/game/adventure/scenario/surface/sea/sea.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>

#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/races/races_interface.h>
#include <engine.h>
#include <environments/game/adventure/scenario/surface/sea/sea_shader.h>
#include <environments/game/adventure/scenario/surface/clouds/clouds_shader.h>
#include <players/players_array.h>

Surface::Surface(const std::string& mapSize) : 
	engine(Engine::GetInstance()), 
	surfSettings(SurfaceSettings::GetInstance()),
	terrainTextureArray(std::shared_ptr<TerrainTextureArray>(new TerrainTextureArray()))
{
	// if i'm here it means that i'm creating a new (empty) scenario
	// or that i'm instancing the terrain during .zip loading

	// if mapSize is "" default values from terrainInfo.xml will be used

	this->terrain = std::shared_ptr<Terrain>(new Terrain(mapSize, false, terrainTextureArray->GetGrass1TextureId()));
}
Surface::Surface(const int seed) : 
	engine(Engine::GetInstance()), 
	surfSettings(SurfaceSettings::GetInstance()),
	terrainTextureArray(std::shared_ptr<TerrainTextureArray>(new TerrainTextureArray()))
{
	// if i'm here it means that i'm doing a random map generation

	this->terrain = std::shared_ptr<Terrain>(new Terrain(surfSettings.GetSelectedMapSize(), true, terrainTextureArray->GetEmptyTextureId()));
	this->randomSeed = seed;
}
Surface::~Surface(void) 
{ 
}

std::weak_ptr<Terrain> Surface::GetTerrainW(void) const
{
	return this->terrain;
}
std::shared_ptr<Terrain> Surface::GetTerrain(void) const
{
	return this->terrain;
}
std::weak_ptr<TerrainTextureChunks> Surface::GetTerrainTextureChunks(void) const
{
	return this->terrainTextureChunks;
}
std::weak_ptr<SurfaceGrid> Surface::GetSurfaceGrid(void) const
{
	return this->surfaceGrid;
}
std::weak_ptr<TerrainTextureArray> Surface::GetTerrainTextureArray(void) const
{
	return this->terrainTextureArray;
}
void Surface::Initialize(void)
{
	this->visibleMapSize = glm::vec2(this->terrain->GetTerrainInfo(TerrainInfo_MapWidthPixel), this->terrain->GetTerrainInfo(TerrainInfo_MapHeightPixel));
	this->gapBetweenVerticesPx = (float)this->terrain->GetVerticesGapPixel();
	this->numberOfTerrainChunks = glm::ivec2(this->terrain->GetTerrainInfo(TerrainInfo_NumberOfTerrainChunksX), this->terrain->GetTerrainInfo(TerrainInfo_NumberOfTerrainChunksY));
	this->terrainChunkWideness = (uint32_t)this->terrain->GetTerrainInfo(TerrainInfo_TerrainChunkSize);
	this->terrainChunkWidenessPx = (float)this->terrain->GetTerrainInfo(TerrainInfo_TerrainChunkSizePixel);
	this->totalMapSize = glm::vec2(this->terrain->GetMapTotalWidth(), this->terrain->GetMapTotalHeight());

	surfSettings.SetSelectedMapSize(this->terrain->GetMapSize()); // to be sure that the size is the same
	this->terrainTextureChunks = std::shared_ptr<TerrainTextureChunks>(new TerrainTextureChunks(uint32_t(this->gapBetweenVerticesPx), uint32_t(this->visibleMapSize.x), this->terrainTextureArray));
	this->sea = std::shared_ptr<Sea>(new Sea(uint32_t(this->visibleMapSize.x), uint32_t(this->visibleMapSize.y)));
	this->clouds = std::shared_ptr<Clouds>(new Clouds(uint32_t(this->visibleMapSize.x), uint32_t(this->visibleMapSize.y)));
	this->surfaceGrid = SurfaceGrid::Create(this->visibleMapSize, this->terrain->GetMapSize());

	// set shortcuts
	this->terrain->surfaceGrid = this->surfaceGrid;
	this->terrain->texArrayOpenglID = this->terrainTextureArray->GetOpenglTextureId();
	this->terrain->texArrayLength = this->terrainTextureArray->GetLength();
	this->terrainTextureChunks->terrain = this->terrain;

	// set shaders
	sea_shader::get_instance().set_surface_constants(this->visibleMapSize.x, this->visibleMapSize.y, SEA_HEIGHT);
	clouds_shader::get_instance().set_surface_constants(this->visibleMapSize.x, this->visibleMapSize.y, CLOUDS_HEIGHT);
}
void Surface::GenerateRandom(const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface)
{
	assert(playersArray);
	// run generator
	RandomMapGenerator generator{ playersArray->GetClassesDataSp(), surface, surface->randomSeed, surface->surfSettings.GetSelectedMapType(), surface->surfSettings.GetSelectedMapSize() };
	generator.SetFlags(RandomMapFlags_Heightmap | RandomMapFlags_Mountains | RandomMapFlags_Townhalls);
	assert(surface);
	generator.StartProcedure(playersArray);
	generator.EndProcedure();
}
void Surface::ToggleGrid(void)
{
	if (this->surfaceGrid->IsGridEnabled() == true)
	{
		this->surfaceGrid->DisableGrid();
	}
	else
	{
		this->surfaceGrid->EnableGrid();
	}
	
}
void Surface::AssignTerrainTexture(PlayersArray& playersArray)
{
	const std::unordered_map<uint32_t, std::weak_ptr<Settlement>>& settlements = playersArray.GetSettlementsCollectionRef().GetSettlementsMapCRef();

	std::vector<glm::vec2> chunksPos;
	std::vector<std::string> chunksZone;

	// get 1 chunk for each townhall
	int processedChunks = 0;
	for (auto const& settl : settlements)
	{
		//Assertion: all the provided settlements MUST exists.
		assert(settl.second.expired() == false);

		const std::shared_ptr<Settlement> sp = settl.second.lock();
		const std::shared_ptr<Building> centralBuilding = sp->GetFirstBuilding();
		if (centralBuilding->IsHeirOf("basetownhall") == false)
			continue;

		const float xTownhall = centralBuilding->GetPositionX();
		const float yTownhall = centralBuilding->GetPositionY();
		const std::string race = centralBuilding->GetRaceName();
		if (RacesInterface::DoesRaceExist(race) == false || RacesInterface::GetRacePtr(race).expired() == true)
			throw std::exception("Race doesn't exist!!");

		const std::string zone = RacesInterface::GetRacePtr(race).lock()->GetEnvironmentIdalZone();
		auto chunk = this->terrainTextureChunks->GetTextureChunkByMapCoordinates((int)xTownhall, (int)yTownhall);
		assert(chunk.has_value() && chunk.value().expired() == false);

		chunk.value().lock()->SetZone(zone);
		processedChunks++;
		chunksPos.push_back(chunk.value().lock()->GetBottomLeftPosition());
		chunksZone.push_back(chunk.value().lock()->GetZone());
	}

	if (chunksZone.size() == 0)
	{
		auto chunk = this->terrainTextureChunks->GetTextureChunkByMapCoordinates(int(this->visibleMapSize.x / 2), int(this->visibleMapSize.y / 2));
		assert(chunk.has_value() && chunk.value().expired() == false);

		chunk.value().lock()->SetZone("mediterranean");
		processedChunks++;
		chunksPos.push_back(chunk.value().lock()->GetBottomLeftPosition());
		chunksZone.push_back(chunk.value().lock()->GetZone());
	}

	// algorithm

	bool condition = true;
	int counter = 1;
	int delta = this->terrainTextureChunks->GetTextureChunkSize();

	std::function<void(glm::vec2* chunksPos, glm::ivec2 delta, int* processedChunks, std::string& chunkZone)> fun =
		[this]
	(glm::vec2* chunksPos, glm::ivec2 delta, int* processedChunks, std::string& chunkZone)
	{
		glm::vec2 chunkPos = (*chunksPos);
		auto nextChunk = this->terrainTextureChunks->GetTextureChunkByMapCoordinates((int)chunkPos.x + delta.x, (int)chunkPos.y + delta.y);
		(*chunksPos) = glm::vec2(chunkPos.x + delta.x, chunkPos.y + delta.y);
		
		//assert(nextChunk.has_value() && nextChunk.value().expired() == false);
		if (nextChunk.has_value() == false || nextChunk.value().expired())
			return;

		if (nextChunk.value().lock()->HasZone() == false)
		{
			nextChunk.value().lock()->SetZone(chunkZone);
		}
		(*processedChunks)++;
	};

	while (condition)
	{
		for (int j = 0; j < chunksPos.size(); j++)
		{
			for (int i = 0; i < counter; ++i)  // move right
				fun(&(chunksPos[j]), glm::ivec2(delta, 0), &processedChunks, chunksZone[j]);

			for (int i = 0; i < counter - 1; ++i) // move down right. Note N-1
				fun(&(chunksPos[j]), glm::ivec2(0, delta), &processedChunks, chunksZone[j]);

			for (int i = 0; i < counter; ++i) // move down left
				fun(&(chunksPos[j]), glm::ivec2(-1 * delta, delta), &processedChunks, chunksZone[j]);

			for (int i = 0; i < counter; ++i) // move left
				fun(&(chunksPos[j]), glm::ivec2(-1 * delta, 0), &processedChunks, chunksZone[j]);

			for (int i = 0; i < counter; ++i) // move up left
				fun(&(chunksPos[j]), glm::ivec2(0, -1 * delta), &processedChunks, chunksZone[j]);

			for (int i = 0; i < counter; ++i) // move up right
				fun(&(chunksPos[j]), glm::ivec2(delta, -1 * delta), &processedChunks, chunksZone[j]);
		}

		condition = (processedChunks < (int)this->terrainTextureChunks->GetNumberOfTextureChunks());
		//condition = counter <= 3;

		counter++;
	}
}

void Surface::Render(const bool activeChunks, const bool wireframe, const bool bRenderSea) const
{
	if (activeChunks)
	{
		this->terrain->RenderActiveChunks(wireframe);
		//this->terrain->RenderHoveredChunk();
	}
	else
	{
		this->terrain->RenderAllChunks();
	}
	if (wireframe == true) 
		return;

	if (bRenderSea)
		this->sea->Render();
}

void Surface::RenderSurfaceGrid(void)
{
	this->surfaceGrid->RenderGrid();
}

void Surface::RenderClouds(void) const
{
	this->clouds->Render();
}

void Surface::UpdateAllTextureChunks(void)
{
	this->terrainTextureChunks->SetTextureChunks();
}

glm::vec2 Surface::GetVisibleMapSize(void) const
{
	return this->visibleMapSize;
}

glm::vec2 Surface::GetTotalMapSize(void) const
{
	return this->totalMapSize;
}

glm::ivec2 Surface::GetNumberOfTerrainChunks(void) const
{
	return this->numberOfTerrainChunks;
}

uint32_t Surface::GetTerrainChunkWideness(void) const
{
	return this->terrainChunkWideness;
}

float Surface::GetTerrainChunkWidenessPx(void) const
{
	return this->terrainChunkWidenessPx;
}

float Surface::GetGapBetweenVerticesPx(void) const
{
	return this->gapBetweenVerticesPx;
}

bool Surface::TryGetVertexIndexByMapCoordinates(int x, int y, unsigned int* index, bool considerOnlyVisibleVertices) const
{
	return this->terrain->TryGetVertexIndexByMapCoordinates(x, y, index, considerOnlyVisibleVertices);;
}
