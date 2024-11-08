/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

class Race
{
public:

	struct SettlementDisposition
	{
		std::vector<std::string> buildings;
		std::vector<glm::vec2> offsets;
	};

	//void SetRaceProperties(std::string _name, std::string _environmentalZone, std::string _foodTransportClassName);
	#pragma region Getters:
	int GetRaceId(void) const;
	std::string GetRaceName(void) const;
	std::string GetEnvironmentIdalZone(void);
	std::string GetFoodTransportClassName(void);
	std::vector<std::string> GetStrongholdDisplacements(void);
	std::vector<std::string> GetVillageDisplacements(void);
	#pragma endregion

	#pragma region Constrcutor and destructor:

	Race(void);
	Race(const uint32_t _id, const std::string& _name, const std::string& _environmentalZone, const std::string& _transportClassName, const std::vector<std::string>& _strongholdDisplacements, const std::vector<std::string>& _villageDisplacements, const std::string& _caller);
	~Race(void);
	#pragma endregion
private:
	uint32_t id;
	std::string name;
	std::string environmentalZone;
	std::string transportClassName;
	std::vector<std::string> strongholdDisplacements;
	std::vector<std::string> villageDisplacements;
};
