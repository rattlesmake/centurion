/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <header.h>
#include <engine.h>

class GObject;
class Unit;
class Hero;
class ObjsList;

class OrderedUnitsList
{
public:
	#pragma region Constructors and operators:
	OrderedUnitsList(const OrderedUnitsList& other) = delete;
	OrderedUnitsList& operator=(const OrderedUnitsList& other) = delete;
	#pragma endregion

	enum class Operations
	{
		e_insert = 0,	//Insertion in the List.
		e_remove,		//Removal from the list.
	};

	struct OrderedUnitsListInfo
	{
	public:
		OrderedUnitsListInfo(const OrderedUnitsListInfo& other) = delete;
		OrderedUnitsListInfo& operator=(const OrderedUnitsListInfo& other) = delete;
		OrderedUnitsListInfo(const std::string& iconName, const std::weak_ptr<Hero>& _hero, std::string& _id);
		OrderedUnitsListInfo(OrderedUnitsListInfo&& other) noexcept;

		const std::string id; //The classname or, in case of heroes, className + uniqueId.
		std::string iconName;
		uint32_t counter = 0;
		std::weak_ptr<Hero> heroWRef;
	};

	class HealthPerClassMap
	{
	public:
		typedef uint32_t perHealth_t;
		HealthPerClassMap(void) = default;
		HealthPerClassMap(const HealthPerClassMap& other) = delete;
		HealthPerClassMap& operator=(const HealthPerClassMap& other) = delete;

		void UpdateHealthMap(perHealth_t& tot, const std::shared_ptr<Unit>& unit, const OrderedUnitsList::Operations operationToDo);
	private:
		typedef std::pair<perHealth_t, std::unordered_set<uniqueID_t>> hmapValue_t;
		typedef std::unordered_map<std::string, hmapValue_t> healthMap_t;
		healthMap_t hMap;

		[[nodiscard]] const hmapValue_t& At(const std::string& key) const;

		friend OrderedUnitsList;
	};

	typedef double egressTimeInstant_t;
	typedef std::unordered_map<std::string, egressTimeInstant_t> egressTimeByClassMap_t;
	typedef std::map<egressTimeInstant_t, OrderedUnitsList::OrderedUnitsListInfo> counterByEgressTimeMap_t;

	#pragma region To scripts members:
	[[nodiscard]] bool Contains(const std::shared_ptr<GObject>& obj);
	[[nodiscard]] float GetPercHealth(const std::string& className) const;
	[[nodiscard]] std::shared_ptr<ObjsList> GetTroops(void) const;
	#pragma endregion

	#pragma region Public members:
	[[nodiscard]] const egressTimeByClassMap_t& GetEgressTimeByClassMapCRef(void) const;
	[[nodiscard]] const counterByEgressTimeMap_t& GetCounterByEgressTimeMapCRef(void) const;
	virtual void HandleHealthChangement(const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new);
	#pragma endregion
protected:
	OrderedUnitsList(void);

	struct troops_s
	{
		std::shared_ptr<ObjsList> units;
		HealthPerClassMap healthPerClass;
		HealthPerClassMap::perHealth_t totHealthPerc = 0;
	};
	std::shared_ptr<ObjsList>& troops;

	virtual void GetBinRepresentation(std::vector<uint8_t>& data) const;
	virtual void InitByBinData(std::vector<uint8_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap) = 0;

	void Update(const std::shared_ptr<Unit>& unit, const Operations operationToDo, const std::optional<egressTimeInstant_t>&& egressTime);
	[[nodiscard]] static float GetPercHealth(const troops_s& troopsStruct, const std::string& className);
	static void HandleHealthChangement(HealthPerClassMap& _hMap, HealthPerClassMap::perHealth_t& _tot, const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new);
private:
	Engine& engine;

	troops_s troopsInfo;

	//A map having a class name as key and the time istant, in which the class name was inserted, as value.
	//For heroes, key is className + uniqueID (e.g. ehero4)
	egressTimeByClassMap_t egressTimeByClass;

	//A map having an istant time as key and some info (about the class associated to this time) as value.
	counterByEgressTimeMap_t counterByEgressTime;
};
