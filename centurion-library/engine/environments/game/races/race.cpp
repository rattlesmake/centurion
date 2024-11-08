#include "race.h"
#include <memory_logger.h>

using namespace std;

#pragma region Getters
int Race::GetRaceId(void) const
{
	return this->id;
}

std::string Race::GetRaceName(void) const
{
	return this->name;
}

std::string Race::GetEnvironmentIdalZone(void)
{
	return this->environmentalZone;
}

std::string Race::GetFoodTransportClassName(void)
{
	return this->transportClassName;
}

std::vector<std::string> Race::GetStrongholdDisplacements(void)
{
	return this->strongholdDisplacements;
}

std::vector<std::string> Race::GetVillageDisplacements(void)
{
	return this->villageDisplacements;
}

#pragma endregion

#pragma region Constructor and destructor
Race::Race(void)
{
	MemoryLogger::AddMemoryConstructionLog("Race", (uint32_t*)this, "UNDEFINED");
}

Race::Race(const uint32_t _id, const std::string& _name, const std::string& _environmentalZone, const std::string& _transportClassName, const std::vector<std::string>& _strongholdDisplacements, const std::vector<std::string>& _villageDisplacements, const std::string& _caller)
	: id(_id), name(_name), environmentalZone(_environmentalZone), transportClassName(_transportClassName), strongholdDisplacements(_strongholdDisplacements), villageDisplacements(_villageDisplacements)
{
	MemoryLogger::memoryLoggerParams param;
	param.push_back(std::make_pair("id", "" + _id));
	param.push_back(std::make_pair("name", "" + _name));
	MemoryLogger::AddMemoryConstructionLog("Race", (uint32_t*)this, _caller, param);
}

Race::~Race(void) 
{ 
	MemoryLogger::memoryLoggerParams param;
	param.push_back(std::make_pair("id", "" + this->id));
	param.push_back(std::make_pair("name", "" + this->name));
	MemoryLogger::AddMemoryDestructionLog("Race", (uint32_t*)this, param);
};
#pragma endregion