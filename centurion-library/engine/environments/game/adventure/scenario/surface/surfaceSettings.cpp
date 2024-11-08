#include "surfaceSettings.h"
#include <header.h>
#include <environments/game/editor/editor.h>
#include <math_utils.h>
#include <encode_utils.h>
#include <random>
#include <settings.h>

// Services
#include <fileservice.h>
#include <services/pyservice.h>
#include <zipservice.h>


SurfaceSettings SurfaceSettings::instance;
SurfaceSettings& SurfaceSettings::GetInstance(void)
{
	return SurfaceSettings::instance;
}
SurfaceSettings::SurfaceSettings()
{
}
SurfaceSettings::~SurfaceSettings()
{
}
std::string SurfaceSettings::GetSelectedMapSize(void)
{
	return this->selectedMapSize;
}

std::string SurfaceSettings::GetSelectedMapType(void)
{
	return this->selectedMapType;
}

void SurfaceSettings::SetSelectedMapSize(std::string _size)
{
	this->selectedMapSize = _size;
}

void SurfaceSettings::SetSelectedMapType(std::string _type)
{
	this->selectedMapType = _type;
}

void SurfaceSettings::ApplyRandomMapSettings(RandomMapSettings& randomMapSettings)
{
	this->SetSelectedMapSize(randomMapSettings.ScenarioSize);
	this->SetSelectedMapType(randomMapSettings.ScenarioType);
}

void SurfaceSettings::Initialize()
{
	//zipFile = rattlesmake::services::file_service::get_instance().get_folder_shortcut("assets") + zipFile;
	this->ReadRandomMapsXml();
	this->ReadRandomMapsPyScripts();
	this->ReadTerrainZonesXml();
}

#pragma region TERRAIN ZONES

std::optional<std::weak_ptr<SurfaceSettings::TerrainZone>> SurfaceSettings::GetTerrainZoneByName(const std::string& name)
{
	if (this->terrainZonesMap.contains(name) == false) return {};
	return this->terrainZonesMap[name];
}
std::optional<std::weak_ptr<SurfaceSettings::TerrainZone>> SurfaceSettings::GetDefaultTerrainZone(void)
{
	if (this->defaultTerrainZone.expired()) return {};
	return this->defaultTerrainZone;
}
void SurfaceSettings::ReadTerrainZonesXml(void)
{
	auto xmlText = rattlesmake::services::zip_service::get_instance().get_text_file(zipFile, "randommap/terrainZones.xml");

	tinyxml2::XMLDocument xmlFile;
	auto error = xmlFile.Parse(xmlText.c_str());
	if (error != tinyxml2::XML_SUCCESS)
		throw std::exception("Terrain Zones XML file is corrupted.");

	tinyxml2::XMLElement* _zones = xmlFile.FirstChildElement("terrainZones");
	for (tinyxml2::XMLElement* _zone = _zones->FirstChildElement(); _zone != nullptr; _zone = _zone->NextSiblingElement())
	{
		auto zone = std::shared_ptr<TerrainZone>(new TerrainZone());
		auto name = tinyxml2::TryParseStrAttribute(_zone, "name");
		auto isDefault = tinyxml2::TryParseBoolAttribute(_zone, "isDefault");
		assert(name.empty() == false);
		Encode::ToLowercase(&name);

		zone->SetName(name);
		zone->AddTexturesFromXml(_zone->FirstChildElement("terrainTextures"));
		zone->AddDecorationsFromXml(_zone->FirstChildElement("decorationAreas"));

		this->terrainZonesMap[name] = zone;
		if (isDefault)
			this->defaultTerrainZone = zone;
	}
}
void SurfaceSettings::TerrainZone::AddTexturesFromXml(tinyxml2::XMLElement* el)
{
	if (el == nullptr) return;
	for (tinyxml2::XMLElement* _tex = el->FirstChildElement(); _tex != nullptr; _tex = _tex->NextSiblingElement())
	{
		std::string name = _tex->Attribute("name");
		assert(name.empty() == false);
		Encode::ToLowercase(&name);
		int freq = _tex->IntAttribute("frequency");
		this->AddTerrainTexture(name, freq);
	}
}
void SurfaceSettings::TerrainZone::AddDecorationsFromXml(tinyxml2::XMLElement* el)
{
	if (el == nullptr) return;
	for (tinyxml2::XMLElement* _d = el->FirstChildElement(); _d != nullptr; _d = _d->NextSiblingElement())
	{
		std::string name = _d->Attribute("className");
		int freq = _d->IntAttribute("frequency");
		this->AddDecorationArea(name, freq);
	}
}
void SurfaceSettings::TerrainZone::AddTerrainTexture(std::string texname, int freq)
{
	for (int i = 0; i < freq; i++)
		this->textures.push_back(texname);
}
void SurfaceSettings::TerrainZone::AddDecorationArea(std::string name, int freq)
{
	for (int i = 0; i < freq; i++)
		this->decorationAreas.push_back(name);
}
std::string SurfaceSettings::TerrainZone::ExtractRandomTexture(int seed)
{
	auto randomizer = Math::Randomizer<std::string>();
	return randomizer.Extract(this->textures, seed);
}
std::string SurfaceSettings::TerrainZone::ExtractRandomDecorationArea(int seed)
{
	auto randomizer = Math::Randomizer<std::string>();
	return randomizer.Extract(this->decorationAreas, seed);
}

std::vector<std::string> SurfaceSettings::TerrainZone::GetRandomDecorationAreas(int seed)
{
	auto areas = this->decorationAreas;
	std::shuffle(areas.begin(), areas.end(), std::default_random_engine(seed));
	return areas;
}

void SurfaceSettings::TerrainZone::SetName(const std::string& _name)
{
	this->name = _name;
}

std::string SurfaceSettings::TerrainZone::GetName(void)
{
	return this->name;
}

#pragma endregion

#pragma region RANDOMMAPS
void SurfaceSettings::ReadRandomMapsPyScripts(void)
{
	std::string pyText = rattlesmake::services::zip_service::get_instance().get_text_file(zipFile, "randommap/randomMapFunctions.py");
	PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(pyText);
}
void SurfaceSettings::ReadRandomMapsXml(void)
{
	std::string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file(zipFile, "randommap/randomMaps.xml");

	tinyxml2::XMLDocument xmlFile;
	auto error = xmlFile.Parse(xmlText.c_str());
	if (error != tinyxml2::XML_SUCCESS)
		throw std::exception("Random maps XML file is corrupted.");

	tinyxml2::XMLElement* rndMapsXml = xmlFile.FirstChildElement("RandomMaps");

	if (rndMapsXml->FirstChildElement("Sizes") != nullptr)
	{
		for (tinyxml2::XMLElement* el = rndMapsXml->FirstChildElement("Sizes")->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			auto size = tinyxml2::TryParseStrAttribute(el, "name");
			if (size.empty() == false)
				this->randomMapXml.MapSizes.push_back(size);
		}
	}
	if (rndMapsXml->FirstChildElement("Types") != nullptr)
	{
		for (tinyxml2::XMLElement* el = rndMapsXml->FirstChildElement("Types")->FirstChildElement(); el != nullptr; el = el->NextSiblingElement())
		{
			auto type = tinyxml2::TryParseStrAttribute(el, "name");
			if (type.empty() == false)
				this->randomMapXml.MapTypes.push_back(type);
		}
	}
}
std::vector<std::string> SurfaceSettings::GetListOfMapSizes(void)
{
	return this->randomMapXml.MapSizes;
}

std::vector<std::string> SurfaceSettings::GetListOfMapTypes(void)
{
	return this->randomMapXml.MapTypes;
}
#pragma endregion
