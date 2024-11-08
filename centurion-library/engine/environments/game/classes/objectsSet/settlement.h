/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <players/players_array.h>
#include <environments/game/adventure/scenario/area.h>
#include <environments/game/classes/objectsAttributes/settlementAttributes.h>

#include <centurion_typedef.hpp>

#ifndef MAX_DISTANCE_BETWEEN_TWO_BUILDINGS
#define MAX_DISTANCE_BETWEEN_TWO_BUILDINGS	1500   //The min distance that two NEAR buildings must have in order to be part of the same settlement.
#endif 


class Building;
class Unit;
class ObjsList;


/// <summary>
/// This class represents a set of buildings that cooperate each other and are closely connected each other.
/// </summary>
class Settlement : public SettlementAttributes
{
public:
	#pragma region Constructors, destructor:
	Settlement(std::weak_ptr<Building> _firstBuilding, std::weak_ptr<PlayersArray> _playersArrayWP, classData_t _settlementData, gobjData_t* dataSource);
	Settlement(const Settlement& other) = delete;
	~Settlement(void);
	#pragma endregion

	#pragma region Operators:
	Settlement & operator=(const Settlement& other) = delete;
	friend std::ostream& operator<<(std::ostream& out, const Settlement& settlement);
	#pragma endregion

	#pragma region To Script methods:
	/// <summary>
	/// This function return the name of the current settlement.
	/// </summary>
	/// <returns>The name of the current settlement. An empty string if the settlement hasn't hot a name.</returns>
	[[nodiscard]] std::string GetSettlementName(void) const;

	/// <summary>
	/// This function sets the name of the current settlement. This name can be used into the scripts.
	/// </summary>
	/// <param name="par_settlementName">A name for the current settlement.</param>
	/// <returns>True if the name has been assigned; false otherwise.</returns>
	bool SetSettlementName(std::string _settlementName);

	[[nodiscard]] std::shared_ptr<Building> GetFirstBuilding(void) const;

	[[nodiscard]] std::shared_ptr<ObjsList> GetBuildings(void) const;

	/// <summary>
	/// This function return the player to whom the current settlement belongs to.
	/// </summary>
	/// <returns>The player number.</returns>
	[[nodiscard]] uint8_t GetPlayer(void) const;

	/// <summary>
	/// This function sets the player to whom the current settlement belongs to.
	/// </summary>
	/// <param name="par_player">The number of the player.</param>
	bool SetPlayer(const uint8_t _player);

	/// <summary>
	/// This function returns the number of buildings owned by the current settlement.
	/// </summary>
	/// <returns>The number of buildings owned by the current settlement.</returns>
	[[nodiscard]] uint32_t GetNumberOfBuildings(void) const;
	#pragma endregion

	[[nodiscard]] uint32_t GetId(void) const;

	void AddBuildingToSettlement(const std::shared_ptr<Building>& building);

	void RemoveBuildingFromSettlement(const std::shared_ptr<Building>& building);

	void RenderHitbox(void);
	void UpdateHitbox(void);
	const RectangularArea& GetHitbox(void) const;

	[[nodiscard]] int GetDefaultIntAttributesValue(const std::string attributeName);

	/// <summary>
	/// This function returns true when a building is near to the one passed via pointer
	/// </summary>
	[[nodiscard]] bool CheckIfAnyBuildingIsNear(const std::shared_ptr<Building>& building) const;

	void AddDefender(const Unit& unit);
	void RemoveDefender(const Unit& unit);

	void AddBesieger(const Unit& unit);
	void RemoveBesieger(const Unit& unit);
	void HandleLoyalty(void);

	void GetBinRepresentation(std::vector<uint8_t>& data) const;
private:
	// A smart pointer to data about Settlement class read from xml
	classData_t xmlData;

	// An unique ID of the settlement handled by Cpp.
	uniqueID_t id = 0;

	// An unique ID of the settlement handled by scripts.
	std::string idName;

	// This arrays of set contains, for each set, the unique IDs of the units that are conquering the building.
	// The number of element of the array is equal to the number of players. 
	// N.B.: In this case, player 0 is not used so player 1 has index 0 e not index 1.
	// If at least one set isn't empty, then there is some player is trying to take over the settlement.
	std::array<std::unordered_set<uniqueID_t>, PlayersArray::NUMBER_OF_PLAYERS> besiegers{};
	// The overall number of besiegers.
	uint32_t numberOfBesiegers = 0;

	// This set contains the unique IDs of the units that are defending the settlement (those enough close to the settlement).
	std::unordered_set<uniqueID_t> defenders;

	bool bCanBeConquered = true;

	// Player of the settlement.
	uint32_t player = 0;

	// A weak reference to the first central building (i.e. the central building that generated the settlement).
	std::weak_ptr<Building> firstBuilding;

	// An ObjsList containing the building of the settlement.
	std::shared_ptr<ObjsList> buildingsOfSettlement;

	std::weak_ptr<PlayersArray> playersArrayWP;

	// Hitbox 
	RectangularArea hitbox;

	// This flag indicates which attributes are being set during a loading (both from XML and from binary file).
	// If true, an exception will be thrown, when a set method is invoked, if the value to be set is not valid.
	// This indicates an error in the file.
	bool bLoading = false;

	#pragma region Private members:
	void SetSettlementAttributes(gobjData_t* dataSource);

	[[nodiscard]] const uint8_t DefineNewPlayer(void) const;
	#pragma endregion
};
