/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/game/classes/playable.h>
#include <environments/game/classes/objectsAttributes/buildingAttributes.h>
#include <environments/game/classes/objectsSet/settlement_collection.h>
#include <environments/game/classes/objectsSet/garrison.h>


class Player;

class Building : public Playable, public BuildingAttributes
{
public:
	#pragma region Constructors, destructor and operators:
	Building(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	Building(const Building& other) = delete;
	virtual ~Building(void);
	Building& operator=(const Building& other) = delete;
	#pragma endregion

	#pragma region To scripts methods:
	void Damage(const uint32_t damage);
	[[nodiscard]] std::shared_ptr<Garrison> GetGarrison(void) const;
	[[nodiscard]] std::shared_ptr<Settlement> GetSettlement(void) const;
	[[nodiscard]] bool HasGarrison(void) const;
	[[nodiscard]] bool IsBroken(void) const;
	[[nodiscard]] bool IsCentralBuilding(void) const;
	[[nodiscard]] bool IsFirstBuilding(void) const;
	void Repair(void);
	bool SetCommandWithTarget(const std::string& command, const std::shared_ptr<Target>& targetObj) override;
	#pragma endregion

	/// <summary>
	/// This function returns the exit point of a building. 
	/// </summary>
	/// <returns>The position of the exit point of the current building. If the exit point isn't definited, it returns the position of the current building.</returns>
	[[nodiscard]] std::shared_ptr<Point> GetExitPoint(void) const;

	/// <summary>
	/// This function checks if the building can train units, namely if it has at least a training command.
	/// </summary>
	/// <returns>True if the building can train units; false otherwise.</returns>
	[[nodiscard]] bool CanTrainUnits(void) const;

	/// <summary>
	/// This function checks if the building can research some technology, namely if it has at least a technology command.
	/// </summary>
	/// <returns>True if the building can reseacht technologies; false otherwise.</returns>
	[[nodiscard]] bool CanReseachTechnologies(void) const;


	#pragma region Inherited methods:
	void SetHealth(uint32_t _health) override;

	/// <summary>
	/// This function sets the player of current the object.
	/// </summary>
	/// <param name="par_playerID">The player number.</param>
	bool SetPlayer(const uint8_t _playerID) override;

	/// <summary>
	/// This function checks if the building is placeable.
	/// </summary>
	void CheckPlaceability(const bool bAlwaysPlaceable = false) override;

	/// <summary>
	/// This function performs the current building's game logic checks.
	/// </summary>
	void ApplyGameLogics(void) override;

	void SetDisplayedName(std::string _displayedName) override;

	bool SetPosition(const int x, const int y) override;

	void AddCommandToExecute(const std::string& commandID) override;

	void GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const override;
	#pragma endregion

	/// <summary>
	/// This function adds some health points to the current building.
	/// </summary>
	/// <param name=healthPoints">Health points</param>
	void Heal(const uint32_t healthPoints);

	/// <summary>
	/// This functionc checks if the the class of the current building is an heir of Village Hall class,
	/// and consequently if it is a central building.
	/// </summary>
	/// <returns>True if it is an heir of VillageHall class; false otherwise.</returns>
	[[nodiscard]] bool IsVillageHall(void) const;

	/// <summary>
	/// This functionc checks if the the class of the current building is an heir of Townhall class,
	/// and consequently if it is a central building.
	/// </summary>
	/// <returns>True if it is an heir of Townhall class; false otherwise.</returns>
	[[nodiscard]] bool IsTownhall(void) const;

	/// <summary>
	/// This functionc checks if the the class of the current building is an heir of Outpost class,
	/// and consequently if it is a central building.
	/// </summary>
	/// <returns>True if it is an heir of Outpost class; false otherwise.</returns>
	[[nodiscard]] bool IsOutpost(void) const;

	/// <summary>
	/// This functionc checks if the the class of the current building is an heir of Shipyward class,
	/// and consequently if it is a central building.
	/// </summary>
	/// <returns>True if it is an heir of Shipyard class; false otherwise.</returns>
	[[nodiscard]] bool IsShipyard(void) const;

	/// <summary>
	/// This function sets the path in which can be found the entity of the current building.
	/// </summary>
	/// <param name="par_ent_path">The path of the entity.</param>
	void SetEntPath(std::string par_ent_path);

	void SetSettlement(const std::shared_ptr<Settlement>& sett, const bool _bIsFirstBuilding);

	[[nodiscard]] std::list<std::shared_ptr<Building>> GetNeighbours(void) const;

	[[nodiscard]] bool IsNeighbour(const uint32_t x, const uint32_t y) const;

	bool CheckIfSettlementChainIsNotBroken(const bool thisIsBeingErased = false) const;

	void AddNeighboursToSet(std::set<std::shared_ptr<Building>>& setRef, const std::shared_ptr<Building>& firstBuilding, const std::shared_ptr<Building>& buildingToBeErased) const;
protected:
	#pragma region Inherited methods:
	std::ostream& Serialize(std::ostream& out, const bool calledByChild) const override;
	virtual void SetAttrs(const classData_t& objData, gobjData_t* dataSource) override;
	void SetObjectCommands(const classData_t& objData) override;
	#pragma endregion
private:
	std::shared_ptr<Garrison> garrison;
	
	uint16_t trainingsCommandsNumber = 0;
	uint16_t technologiesCommandsNumber = 0;

	//Indicate if the building is too damaged and requires to be repaired.
	bool bIsBroken = false;

	bool bIsOutpost = false;
	bool bIsTownhall = false;
	bool bIsVillageHall = false;
	bool bIsShipyard = false;

	//Indicate if the building is a candiate to be a first building. In addition, a settlement can be captured only capturing one of its central buildings.
	bool bIsCentralBuilding = false;

	//If true the building is the first central building inside a settlement.
	bool bIsFirstBuilding = false;

	std::string ent_path;
	std::shared_ptr<Settlement> settlement;
	
	//sound selectionSound; TODO

	//gui::Text displayedNameText;
	//gui::Circle buildingCircle; // for settlement

	#pragma region Private members:
	void SetBrokeness(const bool bIsBroken);
	#pragma endregion
};
