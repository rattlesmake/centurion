/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <header.h>  // To remove as soon as

class PlayersArray;
class GObject;
class Hero;
class Target;

typedef std::unordered_set<std::weak_ptr<GObject>, hash_weakPtr, equalCMP_weakPtr> gobjsSPUSet_t;
class ObjsList
{
public:

	#pragma region Constructors
	ObjsList(void) = delete;
	ObjsList(const ObjsList& other) = delete;
	explicit ObjsList(const bool _bScriptsCanEdit);
	#pragma endregion

	#pragma region Operators:
	ObjsList& operator=(const ObjsList& other) = delete;
	//friend std::ostream& operator<<(std::ostream& out, const ObjsList& objsList);
	//[[nodiscard]] ObjsList operator+(const ObjsList& other);
	//[[nodiscard]] ObjsList operator-(const ObjsList& other);
	//[[nodiscard]] bool operator==(const ObjsList& other);
	#pragma endregion


	#pragma region Iterators:	
	typedef std::list<std::weak_ptr<GObject>>::iterator objs_iterator;
	typedef std::list<std::weak_ptr<GObject>>::const_iterator objs_const_iterator;
	[[nodiscard]] objs_iterator begin(void) noexcept;
	[[nodiscard]] objs_const_iterator cbegin(void) const noexcept;
	[[nodiscard]] objs_iterator end(void) noexcept;
	[[nodiscard]] objs_const_iterator cend(void) const noexcept;
	#pragma endregion


	#pragma region Static members:
	static void SetPlayersArrayWeakRef(const std::weak_ptr<PlayersArray>& weakRef);
	#pragma endregion

	#pragma region Functions that MUST BE used only by external scripts (CPP CODE MUST NOT USE THEM!!!)
	bool Add(const std::shared_ptr<GObject>& object);
	bool Remove(const std::shared_ptr<GObject>& object);
	bool Clear(void);
	#pragma endregion


	#pragma region Function used by external scripts (even CPP code can use them)
	[[nodiscard]] bool Contains(const std::shared_ptr<GObject>& object) const;
	[[nodiscard]] uint32_t Count(void) const;
	[[nodiscard]] uint32_t CountByClass(std::string className) const;
	[[nodiscard]] std::shared_ptr<GObject> Get(const uint32_t index) const;
	[[nodiscard]] uint32_t GetNumberOfDifferentClasses(void) const;
	[[nodiscard]] bool IsEmpty(void) const;
	void Select(const uint8_t playerID);
	void SelectByClass(const uint8_t playerID, const std::string& className);
	void SelectHero(const uint8_t playerID, const std::shared_ptr<Hero>& hero);
	void SetHealthByPercHealth(uint8_t perc_health);
	void SetLevel(uint16_t level);
	void SetPlayer(const uint8_t playerID);
	void SetStaminaByPercStamina(uint8_t stamina);
	uint32_t SetCommandWithTarget(const std::string& commandID, const std::shared_ptr<Target>& targetObj);
	#pragma endregion


	#pragma region Functions that MUST BE used only by CPP code (EXTERNAL SCRIPTS MUST NOT USE THEM!!!)
	[[nodiscard]] const std::list<std::weak_ptr<GObject>>& GetObjsCRef(void) const;
	void ClearAll(void);
	void Insert(const std::shared_ptr<GObject>& object);
	void GetOut(const std::shared_ptr<GObject>& object);
	void ExecuteCommand(const std::string& commandID, const bool bTestOnlyCondition);
	void ExecuteMethod(const std::string& methodID);

	[[nodiscard]] const std::unordered_map<std::string, gobjsSPUSet_t>& GetObjsByClassMapCRef(void) const;
	#pragma endregion
private:
	typedef std::list<std::weak_ptr<GObject>>::iterator iteratorToObj_t;

	// TODO - An id representing uniquely an ObjsList.
	uint32_t id = 0;

	// A flag that indicates if the ObjsList can be modified by external scripts.
	bool bScriptsCanEdit = false;

	// List of all the ID of the GObject inside the ObjsList.
	std::list<std::weak_ptr<GObject>> objs;

	// For each GObject, identified by an unique id, this map stores an iterator to O(1) access to the GObject inside objs.
	std::unordered_map<uniqueID_t, iteratorToObj_t> iteratorsMap;

	// A set containing all GObjects inside the ObjsList ordered by class:
	std::unordered_map<std::string, gobjsSPUSet_t> objsByClass;

	uint32_t units_number = 0;
	uint32_t buildings_number = 0;
	uint32_t decorations_number = 0;
	uint32_t heroes_number = 0;
	uint32_t wagons_number = 0;
	uint32_t druids_numbers = 0;

	static std::weak_ptr<PlayersArray> playersArrayWP;
};
