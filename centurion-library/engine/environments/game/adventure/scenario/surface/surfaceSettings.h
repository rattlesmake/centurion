/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

class RandomMapSettings;

// singleton class as Engine, Settings, ...
class SurfaceSettings
{
public:
#pragma region Singleton
	///Copy constructor and operator = are not allowed in order to avoid accidentally copies of the singleton appearing.
	SurfaceSettings(const SurfaceSettings& other) = delete;
	SurfaceSettings& operator=(SurfaceSettings const& other) = delete;
	[[nodiscard]] static SurfaceSettings& GetInstance(void);
#pragma endregion

#pragma region Terrain ZONES
	class TerrainZone
	{
		friend class SurfaceSettings;
	public:
		void AddTexturesFromXml(tinyxml2::XMLElement* el);
		void AddDecorationsFromXml(tinyxml2::XMLElement* el);
		void AddTerrainTexture(std::string texname, int freq);
		void AddDecorationArea(std::string name, int freq);
		std::string ExtractRandomTexture(int seed = -1);
		std::string ExtractRandomDecorationArea(int seed = -1);
		std::vector<std::string> GetRandomDecorationAreas(int seed = -1);
		void SetName(const std::string& _name);
		std::string GetName(void);
		~TerrainZone() {}
	private:
		TerrainZone() {}
		std::vector<std::string> textures;
		std::vector<std::string> decorationAreas;
		std::string name;
	};
	std::optional<std::weak_ptr<SurfaceSettings::TerrainZone>> GetTerrainZoneByName(const std::string& name);
	std::optional<std::weak_ptr<SurfaceSettings::TerrainZone>> GetDefaultTerrainZone(void);
#pragma endregion
 
	std::vector<std::string> GetListOfMapSizes(void);
	std::vector<std::string> GetListOfMapTypes(void);
	std::string GetSelectedMapSize(void);
	std::string GetSelectedMapType(void);
	void SetSelectedMapSize(std::string _size);
	void SetSelectedMapType(std::string _type);
	void ApplyRandomMapSettings(RandomMapSettings& randomMapSettings);

	void Initialize();
private:

	std::string selectedMapType = "continental";
	std::string selectedMapSize = "small";
	std::string zipFile = "?terrain.zip";

	void ReadRandomMapsPyScripts(void);

	void ReadRandomMapsXml(void);
	struct RandomMapsXml
	{
		std::vector<std::string> MapSizes;
		std::vector<std::string> MapTypes;
	} randomMapXml;
	
	void ReadTerrainZonesXml(void);
	std::unordered_map<std::string, std::shared_ptr<SurfaceSettings::TerrainZone>> terrainZonesMap;
	std::weak_ptr<SurfaceSettings::TerrainZone> defaultTerrainZone;

	static SurfaceSettings instance;
	SurfaceSettings();
	~SurfaceSettings();
};
