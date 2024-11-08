#ifndef MAPGENFLAG_HEIGHTMAP
#define MAPGENFLAG_HEIGHTMAP true
#endif

#ifndef MAPGENFLAG_DISPLACEMENTS
#define MAPGENFLAG_DISPLACEMENTS true
#endif

#ifndef MAPGENFLAG_OBJECTS
#define MAPGENFLAG_OBJECTS true
#endif

#ifndef MAPGENFLAG_SETTLEMENTS
#define MAPGENFLAG_SETTLEMENTS true
#endif

#ifndef MAPGENFLAG_STRONGHOLDS
#define MAPGENFLAG_STRONGHOLDS true
#endif

#ifndef MAPGENFLAG_OUTPOSTS
#define MAPGENFLAG_OUTPOSTS true
#endif

#ifndef MAPGENFLAG_DECORATIONS
#define MAPGENFLAG_DECORATIONS true
#endif

#include "randomMapGenerator.h"

#include <engine.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/classes/objectsStuff/objectGrid.h>
#include <environments/game/classes/objectsStuff/objectPoint.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/adventure/scenario/surface/textures/terrainTextures.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainConstants.h>
#include <math_utils.h>
#include <environments/game/adventure/scenario/minimap.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/races/races_interface.h>
#include <environments/game/races/race.h>
#include <encode_utils.h>
#include <random>
#include <players/player.h>
#include <png.h>

// Services
#include <services/logservice.h>
#include <services/pyservice.h>
#include <fileservice.h>
#include <zipservice.h>

// Assets
#include <xml_classes.h>

#pragma region Generator Class
RandomMapGenerator::RandomMapGenerator(classesData_t _xmlClassesSP, std::shared_ptr<Surface> _surface, const int _seed, std::string _mapType, std::string _mapSize) :
	seed(_seed),
	zipService(rattlesmake::services::zip_service::get_instance()), 
	surface(_surface),
	textureArray(_surface->GetTerrainTextureArray()),
	mapSize(std::move(_mapSize)),
	mapType(std::move(_mapType)),
	xmlClassesSP(std::move(_xmlClassesSP))
{
	this->terrain = surface->GetTerrain();
	this->gapVertices = surface->GetGapBetweenVerticesPx();
	this->chunkWidenessPx = surface->GetTerrainChunkWidenessPx();
	this->visibleMapSize = surface->GetVisibleMapSize();
}

void RandomMapGenerator::SetFlags(RandomMapFlags flags)
{
	this->flags = flags;
}

void RandomMapGenerator::InitializeProcedure(void)
{
	this->terrain->SetMapType(mapType);
	if (this->seed == RANDOM_SEED)
		this->seed = Math::runif_discrete(0, 1000000);

	this->currentMapFolder = "randommap/maps/" + mapSize + "/" + mapType + "/";
	this->data = Data(this->zipFile, mapSize, mapType, this->currentMapFolder);
	this->minHeight = MAX_REACHABLE_HEIGHT;
	this->maxHeight = 0;

	Logger::Info("Random map procedure initialization concluded successfully");
}

void RandomMapGenerator::StartProcedure(const std::shared_ptr<PlayersArray>& playersArray)
{
	rattlesmake::image::stb::flip_vertically_on_load(1);

	this->InitializeProcedure();

	if (MAPGENFLAG_HEIGHTMAP)
		this->StartHeightmapProcedure();

	if (MAPGENFLAG_DISPLACEMENTS)
		this->StartDisplacementsProcedure();

	if (MAPGENFLAG_OBJECTS)
		this->StartObjectsProcedure(playersArray, surface);
}

void RandomMapGenerator::EndProcedure(void)
{
	rattlesmake::image::stb::flip_vertically_on_load(0);
	this->surface->UpdateAllTextureChunks();
	this->terrain->UpdateAllChunks(UpdateTerrainChunkFlags_Everything);

	Logger::Info("Random map generation procedure concluded successfully");
}

void RandomMapGenerator::StartHeightmapProcedure(void)
{
	if ((flags & RandomMapFlags_Heightmap) == false) 
		return;

	auto nx = this->terrain->GetNumberOfMapVerticesX(); // 480
	auto ny = this->terrain->GetNumberOfMapVerticesY(); // 320

	// evaluate python function
	std::stringstream ss;
	ss << "get_heightmap(" << this->seed << ", " << nx << ", " << ny << ")";
	this->seed++;
	auto _t = PyInterpreter::PyEvaluationTypes::FloatVector;
	std::vector<float> data;
	PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(ss.str(), _t, (void*)&data);

	for (int i = 0; i < data.size(); i++)
	{
		unsigned int height = (unsigned int)(data[i] * this->data.MaxHeight);
		unsigned int idx = i;
		terrain->IncreaseVertexZNoiseByIndex(idx, height);
		this->minHeight = std::min(this->minHeight, terrain->GetVertexHeightByIndex(idx));
		this->maxHeight = std::max(this->maxHeight, terrain->GetVertexHeightByIndex(idx));
	}
	//this->NormalizeHeights();

	Logger::Info("Random map heightmap generation procedure concluded successfully");
}

void RandomMapGenerator::StartDisplacementsProcedure(void)
{
	int w, h, n;
	int displacementIdx = Math::runif_discrete(0, (int)this->data.Displacements.size() - 1, this->seed);
	this->seed++;
	std::string path = this->data.Displacements[displacementIdx];

	unsigned char* imageData = zipService.get_image_data(zipFile, path, &w, &h, &n, 0);
	for (int ix = 0; ix < w; ix++)
		for (int iy = 0; iy < h; iy++)
		{
			int k = iy * w + ix;
			auto xCoord = this->gapVertices * ix;
			auto yCoord = this->gapVertices * iy;

			if (terrain->IsVertexInsideMap((int)xCoord, (int)yCoord))
			{
				RandomMapPoint rmp = RandomMapPoint(int(imageData[k * n]), int(imageData[k * n + 1]), int(imageData[k * n + 2]));

				// mountain ranges
				if (rmp == this->data.RandomMapPoints["MountainRange"])
					this->StartMountainsProcedure(xCoord, yCoord);

				// strongholds
				if (rmp == this->data.RandomMapPoints["Stronghold"])
					this->strongholdSlots.push_back(glm::vec2(xCoord, yCoord));

				// villages
				if (rmp == this->data.RandomMapPoints["Village"])
					this->villagesSlots.push_back(glm::vec2(xCoord, yCoord));

				// decoration area 
				if (rmp == this->data.RandomMapPoints["GenericSlot"])
					this->genericSlots.push_back(glm::vec2(xCoord, yCoord));

				// sea
				if (rmp == this->data.RandomMapPoints["Sea"])
					this->GenerateSea(xCoord, yCoord, SEA_HEIGHT);

				// deep sea 1 (smoothing)
				if (rmp == this->data.RandomMapPoints["DeepSea1"])
					this->GenerateSea(xCoord, yCoord, (NOT_PASSABLE_SEA_HEIGHT + SEA_HEIGHT) / 2);

				// deep sea 2
				if (rmp == this->data.RandomMapPoints["DeepSea2"])
					this->GenerateSea(xCoord, yCoord, NOT_PASSABLE_SEA_HEIGHT);
			}
		}

	this->SmoothSea();

	//std::shuffle(this->strongHoldsCoordinates.begin(), this->strongHoldsCoordinates.end(), std::default_random_engine(RANDOM_SEED));
	this->seed++;

	rattlesmake::image::stb::free(imageData);

	Logger::Info("Random map displacements generation procedure concluded successfully");
}
void RandomMapGenerator::StartMountainsProcedure(const float xCoord, const float yCoord)
{
	int w, h, n;
	auto mountainRanges = zipService.get_all_files_within_folder(zipFile, "randommap/mountainranges/");
	int idx1 = Math::runif_discrete(0, (int)mountainRanges.size() - 1, this->seed);
	std::string path1 = mountainRanges[idx1];
	this->seed++;

	unsigned char* imageData = zipService.get_image_data(zipFile, path1, &w, &h, &n, 0);
	for (int ix = 0; ix < w; ix++)
		for (int iy = 0; iy < h; iy++)
		{
			int k = iy * w + ix;
			int r = int(imageData[k * 3]);
			int g = int(imageData[k * 3 + 1]);
			int b = int(imageData[k * 3 + 2]);

			if (r == 255 && g == 255 && b == 255)
			{
				auto xPos = xCoord + (ix - w/2) * this->gapVertices;
				auto yPos = yCoord + (iy - h/2) * this->gapVertices;
				this->GenerateMountain(xPos, yPos, 0.5f);
			}
		}
	rattlesmake::image::stb::free(imageData);
}
void RandomMapGenerator::GenerateSea(const float xCoord, const float yCoord, const int value)
{
	unsigned int idx;
	if (terrain->TryGetVertexIndexByMapCoordinates((int)xCoord, (int)yCoord, &idx))
	{
		auto listOfSands = textureArray.lock()->GetTerrainTexturesByCategory("sand");
		
		auto randomizer = Math::Randomizer<std::weak_ptr<TerrainTexture>>();
		auto sand = randomizer.Extract(listOfSands, seed);
		if (sand.expired() == false)
		{
			terrain->SetVertexTextureByIndex(idx, sand.lock()->GetTextureId(), sand.lock()->IsPassable());
			terrain->SetVertexZNoiseByIndex(idx, value);
			this->hasSea = true;
		}
	}
}
void RandomMapGenerator::SmoothSea(void)
{
	if (this->hasSea == false) 
		return;

	int nVert = terrain->GetNumberOfMapVertices();
	int nVertX = terrain->GetNumberOfMapVerticesX();
	
	auto listOfSands = textureArray.lock()->GetTerrainTexturesByCategory("sand");
	
	auto randomizer = Math::Randomizer<std::weak_ptr<TerrainTexture>>();
	auto sand = randomizer.Extract(listOfSands, seed);
	if (sand.expired()) 
		return;

	for (int idx = 0; idx < nVert; idx++)
	{
		auto ix = idx % nVertX;
		auto iy = idx / nVertX;
		int h0 = terrain->GetVertexHeightByIndex(idx);

		if (ix == 0 || ix == nVertX - 1 || iy == 0 || iy == nVert / nVertX - 1 || h0 != SEA_HEIGHT) continue;

		const uint8_t nDir = 8;
		int DIR[nDir] = { -1, 1, nVertX, -nVertX, nVertX - 1, nVertX + 1, -nVertX - 1, -nVertX + 1 };
		bool bDIR[nDir] = { false };
		uint8_t STEPS[nDir] = { 4, 4, 4, 4, 4, 4, 4, 4 };

		for (int i = 0; i < nDir; i++)
			bDIR[i] = terrain->GetVertexHeightByIndex(idx + DIR[i]) > SEA_HEIGHT;

		// smoothing
		int nDirTrue = 0;
		for (int i = 0; i < nDir; i++)
		{
			if (bDIR[i] == false) 
				continue;

			float previous = -1.f;
			for (int step = 1; step <= STEPS[i]; step++)
			{
				bool diag = (i == 4 || i == 7) && nDirTrue == 0;

				auto iidx = idx + DIR[i] * step;
				auto iix = iidx % nVertX;
				auto iiy = iidx / nVertX;
				if (iix < 0 || iix >= nVertX || iiy < 0 || iiy >= nVert / nVertX) 
					break;

				if (step <= 2)
					terrain->SetVertexTextureByIndex(iidx, sand.lock()->GetTextureId(), sand.lock()->IsPassable());

				int current = terrain->GetVertexHeightByIndex(iidx);
				if (current <= previous)
					break;

				auto currentNew = float(SEA_HEIGHT + 10 * step);
				if (step == 1 && diag)
					currentNew -= 5;

				if (step == 1)
					currentNew -= Math::runif_discrete(0, 2);

				if (current <= currentNew) 
					continue;

				terrain->SetVertexZNoiseByIndex(iidx, (int)currentNew);
				previous = currentNew;
			}
			nDirTrue++;
		}
	}
}
void RandomMapGenerator::GenerateMountain(const float xMapCoord, const float yMapCoord, const float heightCoef)
{
	int w, h, n;
	auto mountains = zipService.get_all_files_within_folder(zipFile, "randommap/mountains/");
	int idx1 = Math::runif_discrete(0, (int)mountains.size() - 1, this->seed);
	std::string path1 = mountains[idx1];
	this->seed++;

	auto randomizer = Math::Randomizer<std::weak_ptr<TerrainTexture>>();
	auto surfaceGrid = this->surface->GetSurfaceGrid().lock();

	unsigned char* imageData = zipService.get_image_data(zipFile, path1, &w, &h, &n, 0);
	for (int ix = 0; ix < w; ix++)
		for (int iy = 0; iy < h; iy++)
		{
			int imageIndex = iy * w + ix;
			float vertexHeight = float(imageData[imageIndex * 3]);

			auto x1 = int(ix * gapVertices + xMapCoord - w * gapVertices / 2);
			auto y1 = int(iy * gapVertices + yMapCoord - h * gapVertices / 2);

			unsigned int vertexIndex;
			if (terrain->TryGetVertexIndexByMapCoordinates(x1, y1, &vertexIndex))
			{
				terrain->IncreaseVertexZNoiseByIndex(vertexIndex, int(vertexHeight * heightCoef));

				if (vertexHeight >= 32)
				{
					auto listOfRocks = textureArray.lock()->GetTerrainTexturesByCategory("rock");

					auto rock = randomizer.Extract(listOfRocks, seed);
					if (rock.expired()) 
						continue;
					
					// save info for later use
					//MountainTexture mt = MountainTexture();
					//mt.vertexIndex = vertexIndex;
					//mt.textureId = rock.lock()->GetId();
					//mt.bPassable = rock.lock()->IsPassable();
					//this->mountainTextures.push_back(mt);

					// set as not passable
					//surfaceGrid->SetGridCellValueByMapCoords(x1, y1, false, true, GridOperations::E_GRID_OVERLAP);
					terrain->SetVertexTextureByIndex(vertexIndex, rock.lock()->GetTextureId(), rock.lock()->IsPassable());

					auto chunk = this->surface->GetTerrainTextureChunks().lock()->GetTextureChunkByMapCoordinates(x1, y1);
					if (chunk.has_value() && chunk.value().expired() == false) 
						chunk.value().lock()->SetAsNearToMountain();
				}
			}
		}
	rattlesmake::image::stb::free(imageData);
}
void RandomMapGenerator::NormalizeHeights(void)
{
	for (unsigned int idx = 0; idx < terrain->GetNumberOfMapVertices(); idx++)
	{
		auto h = (float)terrain->GetVertexHeightByIndex(idx);
		auto normValue = (h - (float)MIN_GRASS_HEIGHT) / this->maxHeight * 200.f + MIN_GRASS_HEIGHT;
		int delta = h - normValue;
		terrain->DecreaseVertexZNoiseByIndex(idx, delta);
	}
	this->minHeight = terrain->GetMinZ();
	this->maxHeight = terrain->GetMaxZ();
}
void RandomMapGenerator::StartObjectsProcedure(const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface)
{
	RandomMapDisplacements displacements = RandomMapDisplacements();
	displacements.Initialize(surface);

	// temporary class

	if (MAPGENFLAG_SETTLEMENTS)
	{
		if (MAPGENFLAG_STRONGHOLDS)
		{
			this->GenerateStrongholds(displacements, playersArray, surface);
			this->GenerateStrongholdsVillages(displacements, surface);
		}

		if (MAPGENFLAG_OUTPOSTS)
		{
			this->GenerateOutposts(surface);
		}
	}
	
	// assign terrain textures 
	this->surface->AssignTerrainTexture((*playersArray));

	if (MAPGENFLAG_DECORATIONS)
	{
		this->GenerateDecorations(displacements, surface);
	}
}
void RandomMapGenerator::GenerateStrongholds(RandomMapDisplacements& displacements, const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface)
{
	// Strongholds
	const uint8_t nPlayers = playersArray->GetEffectiveNumberOfPlayers();
	Math::Randomizer<std::string> randomizer;

	for (uint8_t playerID = 1; playerID <= nPlayers; playerID++)
	{
		auto player_ref = playersArray->GetPlayerRef(playerID).lock();
		auto race_name = player_ref->GetRace();
		if (RacesInterface::DoesRaceExist(race_name) == false || RacesInterface::GetRacePtr(race_name).expired() == true)
			throw std::exception("Race doesn't exist!!");

		this->races[playerID] = race_name;

		// find the best point to create the stronghold
		auto idx = 0;
		glm::vec2 pt = this->strongholdSlots[0];
		if (playerID > 1)
		{
			auto idx_pt = this->FindBestPointFarFromSettlements(this->strongholdSlots, 0.f);
			idx = idx_pt.first;
			pt = idx_pt.second;
		}

		// respect margin
		float margin = 2000.f;
		pt = glm::vec2(std::min(std::max(pt.x, margin), this->visibleMapSize.x - margin), std::min(std::max(pt.y, margin), this->visibleMapSize.y - margin));

		// remove the vec2 from the available coordinates:
		this->strongholdSlots.erase(this->strongholdSlots.begin() + idx);

		// create stronghold with all buildings
		auto strongholds = RacesInterface::GetRacePtr(race_name).lock()->GetStrongholdDisplacements();
		auto stronghold = randomizer.Extract(strongholds, seed);
		this->seed++;

		assert(displacements.Strongholds.contains(stronghold));

		auto& strongholdDisplacement = displacements.Strongholds[stronghold].Displacements;
		auto& strongholdTextures = displacements.Strongholds[stronghold].Textures;
		auto hitbox = this->GenerateSettlement(playerID, pt, strongholdDisplacement, strongholdTextures, surface);

		this->hitboxes.push_back(hitbox);
		this->strongholdHitboxes.push_back(hitbox);
	}
}
void RandomMapGenerator::GenerateStrongholdsVillages(RandomMapDisplacements& displacements, const std::shared_ptr<Surface>& surface)
{
	Math::Randomizer<glm::vec2> randomizer_vec2;
	Math::Randomizer<std::string> randomizer_str;

	for (uint32_t playerID = 1; playerID <= this->strongholdHitboxes.size(); playerID++)
	{
		auto& hb = this->strongholdHitboxes[playerID - 1];
		auto& race_name = this->races[playerID];
		auto coords = hb.GetListOfCoordinates();
		auto strongholdCenter = hb.GetCenter();

		glm::vec2 pt;

		// find best point 
		// look for the nearest point available (stronghold >> village)
		float maxDistance = this->visibleMapSize.x;
		int idx = -1;
		for (int i = 0; i < this->villagesSlots.size(); i++)
		{
			auto d = Math::euclidean_distance(strongholdCenter.x, strongholdCenter.y, this->villagesSlots[i].x, this->villagesSlots[i].y);
			if (d < maxDistance)
			{
				maxDistance = d;
				idx = i;
				pt = this->villagesSlots[i];
			}
		}
		if (idx == -1)
		{
			if (CENTURION_DEBUG_MODE == true)
			{
				std::cout << "[DEBUG] no village slot has been found!" << std::endl;
			}
			break;
		}

		// if the point is too near
		// keep the same direction but increase the distance
		float minDistanceToKeep = 3000.f;
		if (maxDistance < minDistanceToKeep)
		{
			float slope = (pt.y - strongholdCenter.y) / (pt.x - strongholdCenter.x);

			// given distance, given the slope, find the new point
			float dx = (minDistanceToKeep / std::sqrt(1 + (slope * slope)));
			float dy = slope * dx;

			pt = glm::vec2((int)(strongholdCenter.x + dx), (int)(strongholdCenter.y + dy));
		}

		// respect margin
		float margin = 1000.f;
		pt = glm::vec2(std::min(std::max(pt.x, margin), this->visibleMapSize.x - margin), std::min(std::max(pt.y, margin), this->visibleMapSize.y - margin));

		// remove the vec2 from the available coordinates:
		this->villagesSlots.erase(this->villagesSlots.begin() + idx);

		// create stronghold with all buildings
		auto villages = RacesInterface::GetRacePtr(race_name).lock()->GetVillageDisplacements();
		auto village = randomizer_str.Extract(villages, seed);
		this->seed++;

		assert(displacements.Villages.contains(village));

		auto& villageDisplacements = displacements.Villages[village].Displacements;
		auto& villageTextures = displacements.Villages[village].Textures;

		auto hitbox = this->GenerateSettlement(playerID, pt, villageDisplacements, villageTextures, surface);
		this->hitboxes.push_back(hitbox);
	}
}
RectangularArea RandomMapGenerator::GenerateSettlement(const uint32_t playerID,  const glm::vec2& pt, std::vector<RandomMapDisplacements::Object>& displacements, std::vector<RandomMapDisplacements::Texture>& textures, const std::shared_ptr<Surface>& surface)
{
	std::shared_ptr<Building> firstBuilding;
	float noise = 0.f;

	// find max offset to center the settlement
	int maxOffsetX = 0;
	int maxOffsetY = 0;
	for (auto& o : displacements)
	{
		if (o.Offset.x > maxOffsetX) maxOffsetX = (int)o.Offset.x / 2.f;
		if (o.Offset.y > maxOffsetY) maxOffsetY = (int)o.Offset.y / 2.f;
	}

	// create objects
	for (auto& o : displacements)
	{
		auto pos = glm::vec2(o.Offset.x - maxOffsetX + pt.x, o.Offset.y - maxOffsetY + pt.y);
		
		// find the y noise to reduce the vertical impact of heightmap
		if (firstBuilding == nullptr)
		{
			noise = this->terrain->GetNoiseEstimate(pos.x, pos.y);
		}
		pos.y -= noise;

		if (pos.x < 0 || pos.x >= visibleMapSize.x || pos.y < 0 || pos.y >= visibleMapSize.y)
			continue;

		std::list<std::pair<glm::vec2, bool>> point{ { {pos.x,pos.y}, true } };
		auto objCreated = ObjsCollection::CreateGObject(this->xmlClassesSP, o.Class, point, playerID, false, surface);
		assert(objCreated);

		// save first created building
		if (objCreated->IsBuilding() && std::static_pointer_cast<Building>(objCreated)->IsCentralBuilding())
		{
			firstBuilding = std::static_pointer_cast<Building>(objCreated);
		}
	}

	glm::vec2 point{ pt.x - maxOffsetX, pt.y - maxOffsetY - noise };

	// textures
	for (auto const& t : textures)
	{
		this->SetTexture(t, point);
	}

	// return settlement hitboxes
	// after all buildings creation to have the right and updated hitbox
	assert(firstBuilding->GetSettlement() != nullptr);
	return firstBuilding->GetSettlement()->GetHitbox();
}
void RandomMapGenerator::SetTexture(const RandomMapDisplacements::Texture &t, const glm::vec2& pt)
{
	auto pos = glm::vec2(t.Offset.x + pt.x, t.Offset.y + pt.y);

	//auto pos = t.Offset + pt;
	if (pos.x < 0 || pos.x >= visibleMapSize.x || pos.y < 0 || pos.y >= visibleMapSize.y)
		return;

	auto texByCol = textureArray.lock()->GetTerrainTextureByColor(t.Color);
	if (texByCol.expired())
		return;

	auto texId = 1.f * texByCol.lock()->GetTextureId();
	
	terrain->SetVertexTexture(texId, pos.x, pos.y);
}
void RandomMapGenerator::GenerateOutposts(const std::shared_ptr<Surface>& surface)
{
	const size_t outpostsSize = static_cast<uint32_t>(this->data.Outposts.size());
	if (outpostsSize == 0)
		return;

	auto points = this->genericSlots;

	for (size_t i = 0; i < outpostsSize; i++)
	{
		std::string outpostClassname = this->data.Outposts[i].className;
		int minOutpostFrequency = this->data.Outposts[i].minFrequency;
		int maxOutpostFrequency = this->data.Outposts[i].maxFrequency;

		int outpostFrequency = Math::runif_discrete(minOutpostFrequency, maxOutpostFrequency, this->seed);
		this->seed++;

		while (outpostFrequency > 0)
		{
			auto idx_pt = this->FindBestPointFarFromSettlements(points, 0.7f); //todo?
			auto idx = idx_pt.first;

			if (idx == -1)
				break;

			// remove the vec2 from the available coordinates:
			points.erase(points.begin() + idx);

			if (idx_pt.second.x - SETTL_HITBOX_SIZE < 0 || idx_pt.second.x + SETTL_HITBOX_SIZE >= visibleMapSize.x ||
				idx_pt.second.y - SETTL_HITBOX_SIZE < 0 || idx_pt.second.y + SETTL_HITBOX_SIZE >= visibleMapSize.y)
				continue;

			// check if settlements are too near
			if (this->CheckPointWithHitboxes(idx_pt.second, SETTL_HITBOX_SIZE, SETTL_HITBOX_SIZE) == false)
				continue;

			float noise = this->terrain->GetNoiseEstimate(idx_pt.second.x, idx_pt.second.y);

			std::list<std::pair<glm::vec2, bool>> point{ { {idx_pt.second.x, idx_pt.second.y - noise}, true } };
			auto objCreated = ObjsCollection::CreateGObject(this->xmlClassesSP, outpostClassname, point, 1, false, surface);  // todo PLAYER
			assert(objCreated);

			auto bld = std::static_pointer_cast<Building>(objCreated);
			assert(bld->GetSettlement() != nullptr);
			this->hitboxes.push_back(bld->GetSettlement()->GetHitbox());

			outpostFrequency--;
		}
	}
}
void RandomMapGenerator::GenerateDecorations(RandomMapDisplacements& displacements, const std::shared_ptr<Surface>& surface)
{
	auto surfaceGrid = this->surface->GetSurfaceGrid().lock();

	// Decoration Areas
	for (auto const& vec : this->genericSlots)
	{
		auto texChunk = this->surface->GetTerrainTextureChunks().lock()->GetTextureChunkByMapCoordinates((int)vec.x, (int)vec.y);
		if (texChunk.has_value() == false || texChunk.value().expired())
			continue;
		auto zoneStr = texChunk.value().lock()->GetZone();

		auto zonePtr = SurfaceSettings::GetInstance().GetTerrainZoneByName(zoneStr);
		if (zonePtr.has_value() == false || zonePtr.value().expired()) 
			continue;


		auto decAreaName = zonePtr.value().lock()->ExtractRandomDecorationArea(this->seed);
		this->seed++;

		auto& decArea = displacements.DecorationsArea[decAreaName];
		auto& decDisplacements = decArea.Displacements;
		auto& decTextures = decArea.Textures;
		auto& decAreaSize = decArea.AreaSize;

		// check if area is inside map
		{
			float xLimits[2] = { vec.x - decArea.AreaSize.x / 2.f, vec.x + decArea.AreaSize.x / 2.f };
			float yLimits[2] = { vec.y - decArea.AreaSize.y / 2.f, vec.y + decArea.AreaSize.y / 2.f };
			float margin = 50;
			if (xLimits[0] < margin || xLimits[1] > this->visibleMapSize.x - margin || yLimits[0] < margin || yLimits[1] > this->visibleMapSize.y - margin)
			{
				continue;
			}
		}
		
		// check if settlements are too near
		if (this->CheckPointWithHitboxes(vec, decAreaSize.x, decAreaSize.y) == false)
			continue;

		// check grid inside area
		bool gridOK = true;
		{
			auto startX = vec.x - decArea.AreaSize.x / 2.f;
			auto endX = vec.x + decArea.AreaSize.x / 2.f;
			auto startY = vec.y - decArea.AreaSize.y / 2.f;
			auto endY = vec.y + decArea.AreaSize.y / 2.f;

			for (int ix = startX; ix <= endX; ix += GRID_CELL_SIZE)
			{
				for (int iy = startY; iy <= endY; iy += GRID_CELL_SIZE)
				{
					if (surfaceGrid->IsPointAvailableFromMapCoords(ix, iy) == false)
					{
						gridOK = false;
						break;
					}
				}
			}
		}
		if (gridOK == false) 
			continue;

		// GObjects
		for (auto& o : decDisplacements)
		{
			auto pos = o.Offset + vec;
			if (pos.x < 0 || pos.x >= visibleMapSize.x || pos.y < 0 || pos.y >= visibleMapSize.y) 
				continue;
			unsigned int idx = 0;
			if (surface->TryGetVertexIndexByMapCoordinates((int)pos.x, (int)pos.y, &idx, true))
				if (terrain->GetVertexTextureByIndex(idx) != textureArray.lock()->GetEmptyTextureId())
					continue;

			std::list<std::pair<glm::vec2, bool>> point{ { {pos.x,pos.y}, true } };
			auto objCreated = ObjsCollection::CreateGObject(this->xmlClassesSP, o.Class, point, 0, false, surface);
			assert(objCreated);
		}

		// textures
		for (auto const& t : decTextures)
		{
			this->SetTexture(t, vec);
		}
	}
}
bool RandomMapGenerator::CheckPointWithHitboxes(const glm::vec2 pt, const uint32_t hbSizeX, const uint32_t hbSizeY)
{
	for (const auto& hb : this->hitboxes)
	{
		auto hb2 = RectangularArea(pt, hbSizeX, hbSizeY, AreaOrigin::Center);
		if (hb.Intersect(hb2))
		{
			return false;
		}
	}
	return true;
}
std::pair<uint32_t, glm::vec2> RandomMapGenerator::FindBestPointFarFromSettlements(std::vector<glm::vec2>& points, const float farFromAnglesWeight)
{
	glm::vec2 pt{ -1, -1 };
	auto idx = -1;
	auto distance = 0.f; // to establish the best point
	
	// cycle in points
	for (int j = 0; j < points.size(); j++)
	{
		glm::vec2 currentPt = points[j];
			
		auto average_distance = 0.f;
		auto number_of_settlements = (float)this->hitboxes.size(); // here we 

		// cycle in settlements
		// to calculate the average distance
		for (auto const& hb : this->hitboxes)
		{
			auto d = (float)Math::euclidean_distance(currentPt.x, currentPt.y, hb.GetCenter().x, hb.GetCenter().y);
			average_distance += d;
		}

		average_distance = average_distance / (number_of_settlements);

		// add also distance from the 4 angles
		// with more weight
		{
			auto max_d = (float)Math::euclidean_distance(0.f, 0.f, this->visibleMapSize.x, this->visibleMapSize.y);

			auto d1 = (float)Math::euclidean_distance(currentPt.x, currentPt.y, 0.f, 0.f);
			auto d2 = (float)Math::euclidean_distance(currentPt.x, currentPt.y, 0.f, this->visibleMapSize.y);
			auto d3 = (float)Math::euclidean_distance(currentPt.x, currentPt.y, this->visibleMapSize.x, 0.f);
			auto d4 = (float)Math::euclidean_distance(currentPt.x, currentPt.y, this->visibleMapSize.x, this->visibleMapSize.y);
			
			float angles_dist = max_d - (d1 + d2 + d3 + d4) / 4.f;
			average_distance = average_distance * (1 - farFromAnglesWeight) + angles_dist * farFromAnglesWeight;
		}

		// update values if the current average distance is greater
		if (average_distance > distance)
		{
			distance = average_distance;
			pt = currentPt;
			idx = j;
		}
	}

	// it should always return a point.
	//assert(idx != -1);

	return std::pair<uint32_t, glm::vec2>{ idx, pt };
}



#pragma endregion

#pragma region Data Class
RandomMapGenerator::Data::Data(std::string& zipFile, std::string& mapSize, std::string& mapType, std::string& currentMapFolder)
{
	auto& zipService = rattlesmake::services::zip_service::get_instance();

	auto xmlText = zipService.get_text_file(zipFile, currentMapFolder + "randommap.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS) return;

	tinyxml2::XMLElement* _randomMapConstantsXml = xmlFile.FirstChildElement("RandomMapData");

	this->MaxHeight = _randomMapConstantsXml->FirstChildElement("HeightMaps")->FloatAttribute("maxHeight");

	// HEIGHTMAPS
	// TODO: to remove
	tinyxml2::XMLElement* heightmapsxml = _randomMapConstantsXml->FirstChildElement("HeightMaps");
	if (heightmapsxml != nullptr)
	{
		for (tinyxml2::XMLElement* el = heightmapsxml->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			std::string path = tinyxml2::TryParseStrAttribute(el, "path");
			if (path.empty()) 
				continue;
			path = currentMapFolder + path;
			Encode::NormalizePath(path);
			this->HeightMaps.push_back(path);
		}
	}

	// DISPLACEMENTS
	tinyxml2::XMLElement* displacementsxml = _randomMapConstantsXml->FirstChildElement("Displacements");
	if (displacementsxml != nullptr)
	{
		for (tinyxml2::XMLElement* el = displacementsxml->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			std::string path = tinyxml2::TryParseStrAttribute(el, "path");
			if (path.empty()) 
				continue;
			path = currentMapFolder + path;
			Encode::NormalizePath(path);
			this->Displacements.push_back(path);
		}
	}

	// OUTPOSTS
	tinyxml2::XMLElement* outpostsxml = _randomMapConstantsXml->FirstChildElement("Outposts");
	if (outpostsxml != nullptr)
	{
		for (tinyxml2::XMLElement* el = outpostsxml->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			Data::Outpost op = Data::Outpost();
			op.className = tinyxml2::TryParseStrAttribute(el, "class");
			op.minFrequency = tinyxml2::TryParseIntAttribute(el, "minFrequency");
			op.maxFrequency = tinyxml2::TryParseIntAttribute(el, "maxFrequency");

			if (op.className.empty() == false)
			{
				this->Outposts.push_back(op);
			}
		}
	}

	// RANDOM MAP POINTS
	auto xmlText2 = zipService.get_text_file(zipFile, "randommap/randomMapPoints.xml");

	tinyxml2::XMLDocument rmpXmlFile;
	if (rmpXmlFile.Parse(xmlText2.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	tinyxml2::XMLElement* rmpxml = rmpXmlFile.FirstChildElement("RandomMapPoints");
	if (rmpxml != nullptr)
	{
		for (tinyxml2::XMLElement* el = rmpxml->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			std::string name = tinyxml2::TryParseStrAttribute(el, "name");
			RandomMapPoint rmp = RandomMapPoint(TryParseIntAttribute(el, "r"), TryParseIntAttribute(el, "g"), TryParseIntAttribute(el, "b"));
			if (name.empty() == false) this->RandomMapPoints[name] = rmp;
		}
	}
}
bool RandomMapGenerator::RandomMapPoint::operator==(const RandomMapPoint& rmp) const
{
	return (this->r == rmp.r && this->g == rmp.g && this->b == rmp.b);
}
RandomMapGenerator::RandomMapPoint::RandomMapPoint(int R, int G, int B) : r(R), g(G), b(B) {}
#pragma endregion
