/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

#include <centurion_typedef.hpp>

class Building;
class Settlement;
class PlayersArray;

class BuildingSettlementBridge
{
public:
	BuildingSettlementBridge(void) = delete;
	BuildingSettlementBridge(const BuildingSettlementBridge& other) = delete;
	BuildingSettlementBridge& operator=(const BuildingSettlementBridge& other) = delete;
	~BuildingSettlementBridge(void) = delete;

	static bool AssignSettlement(classData_t && settlementData, const std::shared_ptr<Building>& building, gobjData_t* dataSource, const std::shared_ptr<PlayersArray>& pArray);
	[[nodiscard]] static std::pair<bool, placementError_t> IsBuildingCloseToFriendlySettlement(const std::shared_ptr<Building>& building, const std::shared_ptr<PlayersArray>& pArray);
private:
	[[nodiscard]] static std::tuple<bool, std::shared_ptr<Settlement>, placementError_t> IsBuildingNearToFriendlySettlement(const std::shared_ptr<Building>& building, const std::shared_ptr<PlayersArray>& pArray);
};
