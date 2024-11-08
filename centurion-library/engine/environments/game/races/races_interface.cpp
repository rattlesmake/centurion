#include "races_interface.h"

#include <environments/game/classes/building.h>
#include <fileservice.h>
#include <services/logservice.h>

uint32_t RacesInterface::lastRaceId = UNKNOWN_RACE_ID;
std::unordered_map<std::string, std::shared_ptr<Race>> RacesInterface::racesMap;
std::vector<std::string> RacesInterface::racesNameList;

void RacesInterface::AddUnknownRace(void)
{
	auto unknownRace = UNNKOWN_RACE_NAME;
	auto tempD = std::vector<std::string>();
	auto racePtr = std::shared_ptr<Race>(new Race(UNKNOWN_RACE_ID, unknownRace, "", "", tempD, tempD, __FUNCTION__));
	lastRaceId += 1;
	racesMap[unknownRace] = racePtr;
	racesNameList.push_back(unknownRace);
}

void RacesInterface::ReadRacesXml(tinyxml2::XMLElement* el)
{
	for (tinyxml2::XMLElement* _race = el->FirstChildElement("race"); _race != NULL; _race = _race->NextSiblingElement())
	{
		auto name = tinyxml2::TryParseStrAttribute(_race, "name");
		auto zone = tinyxml2::TryParseStrAttribute(_race, "zone");
		auto food_transport_class = tinyxml2::TryParseStrAttribute(_race, "food_transport_class");
		auto strongholdDisplacements = std::vector<std::string>();
		auto villageDisplacements = std::vector<std::string>();
		for (tinyxml2::XMLElement* _disp = _race->FirstChildElement("displacement"); _disp != NULL; _disp = _disp->NextSiblingElement())
		{
			auto displName = tinyxml2::TryParseStrAttribute(_disp, "name");
			auto _for = tinyxml2::TryParseStrAttribute(_disp, "for");
			if (_for == "village")
			{
				villageDisplacements.push_back(displName);
			}
			else if (_for == "stronghold")
			{
				strongholdDisplacements.push_back(displName);
			}
		}
		if (AddRace(name, zone, food_transport_class, strongholdDisplacements, villageDisplacements) == false)
		{
			Logger::Warn("Race " + name + " was already read!");
		}
	}
}

bool RacesInterface::AddRace(const std::string& _name, const std::string& _environmentalZone, const std::string& _foodTransportClassName, const std::vector<std::string>& _strongholdDisplacements, const std::vector<std::string>& _villageDisplacements)
{
	if (RacesInterface::lastRaceId == 0)
	{
		//The first position is for unknown race.
		AddUnknownRace();
	}
	if (racesMap.count(_name) == 1) //Race already exists.
	{
		return false;
	}
	//Add new race
	auto racePtr = std::shared_ptr<Race>(new Race(lastRaceId, _name, _environmentalZone, _foodTransportClassName, _strongholdDisplacements, _villageDisplacements, __FUNCTION__));
	lastRaceId += 1;
	racesMap[_name] = racePtr;
	racesNameList.push_back(_name);
	return true;
}

const std::vector<std::string> RacesInterface::GetAllRacesNames(void)
{
	return racesNameList;
}

std::weak_ptr<Race> RacesInterface::GetRacePtr(const std::string& raceName)
{
	return racesMap[raceName];
}

uint32_t RacesInterface::GetRaceId(const std::string& _raceName)
{
	return racesMap.count(_raceName) == 1 ? racesMap[_raceName]->GetRaceId() : UNKNOWN_RACE_ID;
}

uint32_t RacesInterface::GetRacesNumber(void)
{
	return lastRaceId;
}

std::string RacesInterface::GetUnknownRaceName(void)
{
	return UNNKOWN_RACE_NAME;
}

bool RacesInterface::DoesRaceExist(const std::string& _raceName)
{
	return racesMap.count(_raceName) == 1;
}
