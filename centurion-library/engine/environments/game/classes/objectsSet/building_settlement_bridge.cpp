#include "building_settlement_bridge.h"

#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>

#include <bin_data_interpreter.h>

bool BuildingSettlementBridge::AssignSettlement(classData_t&& settlementData, const std::shared_ptr<Building>& building, gobjData_t* dataSource, const std::shared_ptr<PlayersArray>& pArray)
{
	// Assertion: building MUST be valid and building MUST NOT already have one.
	assert(building && !building->GetSettlement());

	std::tuple<bool, std::shared_ptr<Settlement>, uint8_t> result;
	if (dataSource == nullptr)  // Creating GObject via editor or via PlaceObject
	{
		result = BuildingSettlementBridge::IsBuildingNearToFriendlySettlement(building, pArray);
	}
	else  // Creating GObject from XML save file or from binary save file
	{
		uint32_t settlementId = 0;
		if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
		{
			tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
			const int xmlSettlementID = tinyxml2::TryParseIntAttribute(xml, "settlementID");
			if (xmlSettlementID <= 0)
				throw BinaryDeserializerException("Invalid settlement id (" + std::to_string(xmlSettlementID) + ")");
			settlementId = static_cast<uint32_t>(xmlSettlementID);
			// Get settlement child. It's important to pass to the Settlement constructor a proper XML object
			(*dataSource) = xml->FirstChildElement("settlement");
		}
		else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
		{
			gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
			settlementId = BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second);
		}
		std::shared_ptr<Settlement> settSP = pArray->GetSettlementsCollectionRef().GetSettlementByID(settlementId);
		if (building->IsCentralBuilding() == true)
		{
			if (settSP && settSP->GetNumberOfBuildings() == 0)
				throw BinaryDeserializerException("Settlement having id " + std::to_string(settlementId) + " already existing when creating a first building");
		}
		else
		{
			if (!settSP || settSP->GetNumberOfBuildings() == 0)
				throw BinaryDeserializerException("Settlement having id " + std::to_string(settlementId) + " not existing when creating a building that isn't a first building");
		}
		result = std::tuple<bool, std::shared_ptr<Settlement>, uint8_t>{ true, settSP, static_cast<uint8_t>(GObject::PlacementErrors::e_no_error) };
	}

	const bool bFound = std::get<0>(result);
	if (bFound == true)  // Assign a settlement only if the building isn't temporary 
	{
		std::shared_ptr<Settlement> foundedSettlement = std::get<1>(result);
		const bool bIsFirstBuilding = !foundedSettlement;
		if (bIsFirstBuilding)  // It's possible only if there are no settlements and building is a central building.
		{
			// Create a new settlement.
			foundedSettlement = std::shared_ptr<Settlement>(new Settlement{ building, pArray, std::move(settlementData), dataSource });

			// Assertion: we MUST NOT have two settlements with the same id.
			assert(pArray->GetSettlementsCollectionRef().Contains(foundedSettlement->GetId()) == false);

			// A new settlement was created: store it.
			pArray->GetSettlementsCollectionRef().Insert(foundedSettlement);
		}

		// Assign the settlement to the building.
		building->SetSettlement(foundedSettlement, bIsFirstBuilding);
		// Assign the building to the settlement.
		foundedSettlement->AddBuildingToSettlement(building);
	}

	return bFound;
}

std::pair<bool, placementError_t> BuildingSettlementBridge::IsBuildingCloseToFriendlySettlement(const std::shared_ptr<Building>& building, const std::shared_ptr<PlayersArray>& pArray)
{
	auto result = BuildingSettlementBridge::IsBuildingNearToFriendlySettlement(building, pArray);
	return std::pair<bool, uint8_t>(std::get<0>(result), std::get<2>(result));
}

std::tuple<bool, std::shared_ptr<Settlement>, placementError_t> BuildingSettlementBridge::IsBuildingNearToFriendlySettlement(const std::shared_ptr<Building>& building, const std::shared_ptr<PlayersArray>& pArray)
{
	// Initially, it is empty since any settlement has not been found yet.
	std::shared_ptr<Settlement> discoveredSettlement;
	GObject::PlacementErrors placementCode{ GObject::PlacementErrors::e_no_error };
	uint32_t settlementsFound = 0;

	auto& settlementsMap = pArray->GetSettlementsCollectionRef().GetSettlementsMapCRef();
	if (settlementsMap.empty() == false)
	{
		for (auto const& settlTemp : settlementsMap)
		{
			// Assertion: if fails, we didn't remove correctly an old settlement (e.g. after a settlement deletion from editor).
			assert(settlTemp.second.expired() == false);

			const std::shared_ptr<Settlement> settlTempSP = settlTemp.second.lock();
			if (settlTempSP->CheckIfAnyBuildingIsNear(building) == true)
			{
				// Check if the building and the settlement belong to the same player
				if (building->GetPlayer() != settlTempSP->GetPlayer())
				{
					placementCode = GObject::PlacementErrors::e_enemy_settlement_around;
					settlementsFound = 0;
					break;  // Because near to a settlement belonging to another player.
				}

				// TODO - insediamento alleato (non dello stesso player)

				// Check if the building is near an other existing settlement (it's possible only during the shifting of the object)
				if (building->GetSettlement() && building->GetSettlement() != settlTempSP)
				{
					placementCode = GObject::PlacementErrors::e_another_settlement_around;
					settlementsFound = 0;
					break;  // Because near to another settlement of the same player.
				}

				discoveredSettlement = settlTempSP;
				settlementsFound += 1;  // A settlement was found
				// Even if a valid settlement was found, the for loop MUST NOT be interrupted! 
				// In fact, the cycle must be completed to verify that the building is close to only ONE valid settlement
				if (settlementsFound >= 2)
				{
					// A second valid settlement was found. So reset everything and deny the placement
					discoveredSettlement = std::shared_ptr<Settlement>();
					placementCode = GObject::PlacementErrors::e_more_possible_choices;
					settlementsFound = 0;
					break;  // Because near to ore than a valid settlement
				}
			}
		}
	}

	// Check if I'm trying to place a central building (if yes, this building will be the first building of the new settlement)
	if (settlementsFound == 0 && placementCode == GObject::PlacementErrors::e_no_error)
	{
		settlementsFound = building->IsCentralBuilding();
		if (settlementsFound == 0)
			placementCode = GObject::PlacementErrors::e_no_settlement_around;
	}

	return std::tuple<bool, std::shared_ptr<Settlement>, placementError_t>(static_cast<bool>(settlementsFound), discoveredSettlement, static_cast<placementError_t>(placementCode));
}