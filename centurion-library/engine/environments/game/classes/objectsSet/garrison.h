/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <engine.h>
#include <environments/game/classes/objectsSet/ordered_units_list.h>

class Building;
class Army;

class Garrison : public OrderedUnitsList
{
public:
	#pragma region Constructors and operators:
	Garrison(const Garrison& other) = delete;
	Garrison& operator=(const Garrison& other) = delete;
	~Garrison(void);
	#pragma endregion

	#pragma region Static members:
	[[nodiscard]] static std::shared_ptr<Garrison> CreateGarrison(const std::shared_ptr<Building>& _building, const uint32_t _maxNumberOfTroops);
	#pragma endregion

	#pragma region To scripts members:
	[[nodiscard]] uint32_t GetNumberOfUnits(void) const;
	[[nodiscard]] uint32_t GetMaxNumberOfUnits(void) const;
	#pragma endregion

	bool AddUnit(const std::shared_ptr<Unit>& unit, const bool bChangeCounter, std::optional<egressTimeInstant_t>&& egressTime);
	bool RemoveUnit(const std::shared_ptr<Unit>& unit, const bool bChangeCounter = true);

	void DecreaseNumberOfTroops(const uint32_t n);

	void GetBinRepresentation(std::vector<uint8_t>& data) const override;
	void InitByBinData(std::vector<uint8_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap) override;
protected:
	Garrison(const std::weak_ptr<Building>& _building, const uint32_t _maxNumberOfTroops);
private:
	//The building protected by this garrison.
	std::weak_ptr<Building> building;

	//The effective number of units inside the building (it, as opposed to troops.Count(), includes also the units attached to the heroes inside).
	uint32_t numberOfTroops = 0;

	//The upper limit of troops 
	uint32_t maxNumberOfTroops = 0;

	void RemoveUnits(const std::shared_ptr<ObjsList>& units, const std::shared_ptr<Army>& heroArmy);
};
