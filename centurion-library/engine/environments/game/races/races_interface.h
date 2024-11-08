/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#ifndef UNKNOWN_RACE_ID
#define UNKNOWN_RACE_ID 0
#endif 
#ifndef UNNKOWN_RACE_NAME 
#define UNNKOWN_RACE_NAME "UNDEFINED"
#endif 

#include <header.h>
#include "race.h"

class RacesInterface
{
public:
	static void ReadRacesXml(tinyxml2::XMLElement* el);

	/// <summary>
	/// This function returns all the available name of the races.
	/// </summary>
	/// <returns>A list containing alla the names of the races.</returns>
	static const std::vector<std::string> GetAllRacesNames(void);

	static std::weak_ptr<Race> GetRacePtr(const std::string& raceName);

	/// <summary>
	/// This function returns the id of a race. 
	/// </summary>
	/// <param name="_raceName">The name of the race.</param>
	/// <returns>The ID of the race or 0 if the race doesn't exits.</returns>
	static uint32_t GetRaceId(const std::string& _raceName);

	/// <summary>
	/// This function returns the number of races.
	/// </summary>
	/// <returns>The number of races</returns>
	static uint32_t GetRacesNumber(void);

	/// <summary>
	/// This function returns a name for a race when it doesn't exist.
	/// </summary>
	/// <returns>a name for a race when it doesn't exist.</returns>
	static std::string GetUnknownRaceName(void);

	/// <summary>
	/// This function checks if a race exists.
	/// </summary>
	/// <param name="_raceName">The name of the race.</param>
	/// <returns>True if the race exist, false otherwise.</returns>
	static bool DoesRaceExist(const std::string& _raceName);

private:

	/// <summary>
	/// This function adds a new race.
	/// </summary>
	/// <param name="_name">Race name.</param>
	/// <param name="_environmentalZone">Environmental Zone of the race (for instance: mediterranean, desertic).</param>
	/// <param name="_transportClassName">The class name of the unit that transports food or gold.</param>
	/// <returns>True if the race wad addes; false if the race is already present.</returns>
	static bool AddRace(const std::string& _name, const std::string& _environmentalZone, const std::string& _transportClassName, const std::vector<std::string>& _strongholdDisplacements, const std::vector<std::string>& _villageDisplacements);

	static void AddUnknownRace();

	static std::unordered_map<std::string, std::shared_ptr<Race>> racesMap;
	static std::vector<std::string> racesNameList;

	static uint32_t lastRaceId;
};
