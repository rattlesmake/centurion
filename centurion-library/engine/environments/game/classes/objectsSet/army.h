/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <environments/game/classes/objectsSet/ordered_units_list.h>
#include <environments/game/classes/objectsSet/objects_list.h>


#ifndef MAX_MAXARMY_VALUE
#define MAX_MAXARMY_VALUE 1000
#endif 


class Army : public OrderedUnitsList
{
public:
	#pragma region Constructors and operators:
	Army(const Army& other) = delete;
	Army& operator=(const Army& other) = delete;
	#pragma endregion

	#pragma region Static members:
	[[nodiscard]] static std::shared_ptr<Army> CreateArmy(const std::shared_ptr<Hero>& _hero, uint32_t _maxNumberOfTroops);
	#pragma endregion

	#pragma region To scripts members:
	[[nodiscard]] uint32_t GetNumberOfUnits(void) const;
	[[nodiscard]] uint32_t GetMaxNumberOfUnits(void) const;
	[[nodiscard]] std::shared_ptr<ObjsList> GetTroopsInTheSameBuilding(void) const;
	[[nodiscard]] bool IsFull(void) const;
	[[nodiscard]] float GetPercHealthUnitsInTheSameGarrison(const std::string& className) const;
	#pragma endregion

	#pragma region Inherited methods:
	void HandleHealthChangement(const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new) override;
	void GetBinRepresentation(std::vector<uint8_t>& data) const override;
	void InitByBinData(std::vector<uint8_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap) override;
	#pragma endregion

	bool AssignUnit(const std::shared_ptr<Unit>& unit, std::optional<egressTimeInstant_t>&& egressTime);
	bool DetachUnit(const std::shared_ptr<Unit>& unit);

	bool MarkUnitAsInTheSameBuilding(const std::shared_ptr<Unit>& unit);
	bool UnmarkUnitAsInTheSameBuilding(const std::shared_ptr<Unit>& unit);

	const std::map<uint8_t, gobjsSPUSet_t>& GetArmyByFormationPriorityCRef(void) const;
protected:
	Army(const std::weak_ptr<Hero>& _hero, const uint16_t _maxNumberOfTroops);
private:
	//The hero who has the ownership of this army.
	std::weak_ptr<Hero> hero;

	//An ObjsList containg all the troops attached to the hero that are in the same building in which the hero is.
	troops_s troopsInTheSameBuilding;

	//The upper limit of troops 
	uint16_t maxNumberOfTroops = 0;

	//A map containing the units ordered by the formation priority value.
	std::map<uint8_t, gobjsSPUSet_t> formPriority;
};
