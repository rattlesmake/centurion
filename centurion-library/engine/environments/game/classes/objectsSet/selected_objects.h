/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <environments/game/classes/objectsAttributes/unitAttributes.h>
#include <environments/game/classes/objectsSet/ordered_units_list.h>
#include <environments/game/classes/objectsStuff/objectCommand.h>
#include <players/players_array.h>

class ObjsList;

class SelectedObjects : public OrderedUnitsList
{
public:
	#pragma region Constructors, destructor and operator:
	SelectedObjects(const SelectedObjects& other) = delete;
	SelectedObjects& operator=(const SelectedObjects& other) = delete;
	~SelectedObjects(void);
	#pragma endregion


	#pragma region Factory method:
	[[nodiscard]] static std::shared_ptr<SelectedObjects> Create(const uint8_t _playerID);
	#pragma endregion


	#pragma region To scripts methods:
	[[nodiscard]] uint64_t GetArmorPierce(void) const;
	[[nodiscard]] uint64_t GetArmorSlash(void) const;
	[[nodiscard]] uint64_t GetFood(void) const;
	[[nodiscard]] uint64_t GetHealth(void) const;
	[[nodiscard]] uint64_t GetLevel(void) const;
	[[nodiscard]] uint64_t GetMinAttack(void) const;
	[[nodiscard]] uint64_t GetMaxAttack(void) const;
	[[nodiscard]] uint64_t GetMaxFood(void) const;
	[[nodiscard]] uint64_t GetMaxStamina(void) const;
	[[nodiscard]] uint64_t GetMaxHealth(void) const;
	[[nodiscard]] uint8_t GetPercHealth(void) const;
	[[nodiscard]] uint8_t GetPercStamina(void) const;
	[[nodiscard]] uint64_t GetRange(void) const;
	[[nodiscard]] uint64_t GetStamina(void) const;
	[[nodiscard]] uint64_t GetSight(void) const;
	[[nodiscard]] uint64_t GetSpeed(void) const;
	[[nodiscard]] bool IsRanged(void) const;
	#pragma endregion


	void UpdateAttributes(const UnitAttributes::UnitValues& attr, const uint32_t _sight, const bool bIsRanged);
	void ResetAttributes(void);
	void Reset(void);
	void Add(const std::shared_ptr<GObject>& object);
	void Remove(const std::shared_ptr<GObject>& object);
	[[nodiscard]] std::shared_ptr<GObject> Get(const uint32_t index) const;
	void Filter(const std::string& className);
	[[nodiscard]] uint32_t Count(void) const;
	[[nodiscard]] std::shared_ptr<GObject> GetSelectedObject(void) const;
	[[nodiscard]] uint32_t GetNumberOfCommands(void) const;
	[[nodiscard]] std::shared_ptr<Command> GetCommand(const uint8_t index) const;
	[[nodiscard]] DamageTypes GetDamageType(void) const;
	[[nodiscard]] bool ContainsOnlyItsOwnGObjects(void) const;
	[[nodiscard]] bool ContainsOnlyUnits(void) const;
	void UpdateObjsByPlayerCounter(const uint8_t playerIdToDecrease, const uint8_t playerIdToIncrement);


	#pragma region Inherited methods
	void GetBinRepresentation(std::vector<byte_t>& data) const override;
	void InitByBinData(std::vector<byte_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap) override;
	#pragma endregion
private:
	explicit SelectedObjects(const uint8_t _playerID);

	// Each SelectedObjects object has a weak reference to itself.
	std::weak_ptr<SelectedObjects> me;

	// Contains the info about the environment in which we are.
	const IEnvironment::Environments environment;

	// The player owning the selection.
	const uint8_t playerID;

	// The selected object (if selection has only an element).
	std::weak_ptr<GObject> selectedObject;

	typedef std::pair<uint32_t, std::shared_ptr<Command>> cmdOccurences_t;
	// For each command ID (a string) contains the number of object having the command and the command itself.
	std::unordered_map<std::string, cmdOccurences_t> ownersPerCommand;

	// The common commands belonging to the multipleSelection.
	std::vector<std::shared_ptr<Command>> commonCommandsVec;

	// The number of common commands.
	uint8_t nCommands = 0;

	// Number of selected GObjects per player.
	std::array<uint32_t, PlayersArray::NUMBER_OF_PLAYERS + 1> selectedObjsPerClass{ 0 };

	// The "values" associated to the selection.
	uint64_t armorPierce = 0;
	uint64_t armorSlash = 0;
	uint64_t food = 0;
	uint64_t health = 0;
	uint64_t level = 0;
	uint64_t minAttack = 0;
	uint64_t maxAttack = 0;
	uint64_t maxFood = 0;
	uint64_t maxHealth = 0;
	uint64_t range = 0;
	uint64_t stamina = 0;
	uint64_t sight = 0;
	uint64_t speed = 0;
	DamageTypes damageType = DamageTypes::e_undefined;
	uint32_t rangedUnitsCounter = 0;

	[[nodiscard]] bool CheckIfResetIsNeeded(const GObject& objToInsert) const;
	void BindSelection(const uint32_t numberOfSelectedObjects) const;
	void SetCommonCommands(void);
};
