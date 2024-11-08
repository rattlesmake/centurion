/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once


#ifndef RANDOM_SEED
#define RANDOM_SEED -1
#endif // !RANDOM_SEED

#include <cstdint>
#include <memory>
#include <string>
#include <utility>  // Includes std::pair<T1, T2>
#include <unordered_map>
#include <vector>

#include <GL/glew.h>
#include <centurion_typedef.hpp>

#include <players/players_array.h>
#include <environments/game/adventure/scenario/area.h>
#include "randomMapDisplacements.h"


namespace rattlesmake { namespace services { class zip_service; }; };
class Terrain;
class Building;
class Surface;
class TerrainTextureArray;
class RandomMapDisplacements;


typedef int RandomMapFlags;
enum RandomMapFlags_
{
	RandomMapFlags_Mountains = 0,
	RandomMapFlags_Townhalls = 1,
	RandomMapFlags_Heightmap = 2,
};


class RandomMapGenerator
{
public:
	RandomMapGenerator(classesData_t _xmlClassesSP, std::shared_ptr<Surface> _surface, const int _seed, std::string _mapType, std::string _mapSize);
	void SetFlags(RandomMapFlags flags);
	void StartProcedure(const std::shared_ptr<PlayersArray>& playersArray);
	void EndProcedure(void);
private:
	void InitializeProcedure(void);
	void StartHeightmapProcedure(void);
	void StartDisplacementsProcedure(void);
	void StartMountainsProcedure(const float xCoord, const float yCoord);
	void GenerateSea(const float xCoord, const float yCoord, const int value);
	void SmoothSea(void);
	void GenerateMountain(const float xMapCoord, const float yMapCoord, const float heightCoef);
	void NormalizeHeights(void);
	void StartObjectsProcedure(const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface);

	void GenerateStrongholds(RandomMapDisplacements& displacements, const std::shared_ptr<PlayersArray>& playersArray, const std::shared_ptr<Surface>& surface);
	void GenerateStrongholdsVillages(RandomMapDisplacements& displacements, const std::shared_ptr<Surface>& surface);
	RectangularArea GenerateSettlement(const uint32_t playerID, const glm::vec2& pt, std::vector<RandomMapDisplacements::Object>& displacements, std::vector<RandomMapDisplacements::Texture>& textures, const std::shared_ptr<Surface>& surface);
	void GenerateOutposts(const std::shared_ptr<Surface>& surface);
	void GenerateDecorations(RandomMapDisplacements& displacements, const std::shared_ptr<Surface>& surface);
	bool CheckPointWithHitboxes(const glm::vec2 pt, const uint32_t hbSizeX, const uint32_t hbSizeY);

	std::pair<uint32_t, glm::vec2> FindBestPointFarFromSettlements(std::vector<glm::vec2>& points, const float farFromAnglesWeight);

	rattlesmake::services::zip_service& zipService;
	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<Surface> surface;
	std::weak_ptr<TerrainTextureArray> textureArray;
	
	std::string zipFile{ "?terrain.zip" };

	std::string currentMapFolder;
	std::string mapType;
	std::string mapSize;
	std::vector<glm::vec2> strongholdSlots;
	std::vector<glm::vec2> villagesSlots;
	std::vector<glm::vec2> genericSlots;

	std::vector<RectangularArea> strongholdHitboxes;
	std::vector<RectangularArea> hitboxes;
	std::unordered_map<uint32_t, std::string> races;

	RandomMapFlags flags;
	int seed = 0;
	int maxHeight = 0;
	int minHeight = 0;
	bool hasSea = false;
	float gapVertices = 0.f;
	float chunkWidenessPx = 0.f;
	glm::vec2 visibleMapSize;

	assets::xmlClassesSP_t xmlClassesSP;

	void SetTexture(const RandomMapDisplacements::Texture& t, const glm::vec2& pt);

	class RandomMapPoint
	{
	public:
		bool operator==(const RandomMapPoint& rmp) const;
		RandomMapPoint() {}
		RandomMapPoint(int R, int G, int B);
		~RandomMapPoint() {}
	private:
		int r = 0, g = 0, b = 0;
	};

	class Data
	{
	public:

		struct Outpost
		{
			std::string className;
			int minFrequency = 0;
			int maxFrequency = 0;
		};

		Data() { }
		Data(std::string& zipFile, std::string& mapSize, std::string& mapType, std::string& currentMapFolder);
		std::unordered_map<std::string, RandomMapPoint> RandomMapPoints;
		std::vector<std::string> HeightMaps;
		std::vector<std::string> Displacements;
		std::vector<Outpost> Outposts;
		float MaxHeight = 0.f;
	}	data;
};
