/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once


#include <environments/game/classes/playable.h>
#include <environments/game/classes/objectsAttributes/unitAttributes.h>


#ifndef MAX_DISTANCE_TO_CONQUER_SETTLEMENT
#define MAX_DISTANCE_TO_CONQUER_SETTLEMENT  650
#endif // !MAX_DISTANCE_TO_CONQUER_SETTLEMENT

#ifndef MAX_DISTANCE_TO_DEFEND_SETTLEMENT
#define MAX_DISTANCE_TO_DEFEND_SETTLEMENT  1000
#endif // !MAX_DISTANCE_TO_DEFEND_SETTLEMENT


class Building;
class Player;
class RectangleArea;
class Settlement;


class Unit : public Playable, public UnitAttributes
{
public:
	#pragma region Constructors and destructor and operators:
	Unit(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	Unit(const Unit& other) = delete;
	virtual ~Unit(void);
	Unit& operator=(const Unit& other) = delete;
	#pragma endregion

	static void ComputeAndApplyDamage(const AttackerDamageParams_s& attacker, const std::weak_ptr<Unit>& attackerWP, Playable& target);

	#pragma region To scripts methods:
	void Detach(void);

	/// <summary>
	/// This function reduces health of the current unit.
	/// </summary>
	/// <param name="damage">The damage.</param>
	void Damage(const uint32_t damage);

	void SetHealthByPercHealth(uint8_t perc_health);

	void SetHealth(uint32_t _health) override;

	/// <summary>
	/// This function returns a reference to the hero who leads the current unit.
	/// </summary>
	/// <returns>A reference to the hero.</returns>
	[[nodiscard]] std::shared_ptr<Hero> GetHero(void) const;

	[[nodiscard]] uint32_t GetKills(void) const;

	/// <summary>
	/// This function can be used to indicate a destination point for the current unit.
	/// </summary>
	/// <param name="destPt">Point that must be reached.</param>
	void GoTo(const std::shared_ptr<Point>& destPt);

	/// <summary>
	/// This function can be used to indicate a destination point from the current unit when it has to approach someone (for instance an hero in order to attach to him).
	/// If the target is moving, the destination point will be automatically updated.
	/// </summary>
	/// <param name="obj">The target to reach.</param>
	/// <param name="obj">The minimum distance the current unit should be located to reach the target.</param>
	void GoToApproach(const std::shared_ptr<Playable>& targetObj, const float minDistance);

	/// <summary>
	/// This function can be used to indicate a destination point from the current unit when it has to attack somebody.
	/// If the target is moving, the destination point will be automatically updated.
	/// </summary>
	/// <param name="obj">The target to attack.</param>
	void GoToAttack(const std::shared_ptr<Playable>& targetObj);

	void GoToConquer(const std::shared_ptr<Playable>& buildingTg);

	/// <summary>
	/// This function can be used to indicate a destination point from the current unit when it has to enter into a building.
	/// </summary>
	/// <param name="destPt">The building that must be reached.</param>
	void GoToEnter(const std::shared_ptr<Playable>& buildingTg);

	/// <summary>
	/// This function checks if the current unit is dead.
	/// </summary>
	/// <returns>True if it's dead; false otherwise.</returns>
	[[nodiscard]] bool IsDead(void) const;

	/// <summary>
	/// This function checks if the current unit is dead.
	/// </summary>
	/// <returns>True if it's dead; false otherwise.</returns>
	[[nodiscard]] bool IsEntering(void) const;

	/// <summary>
	/// This function checks if the current unit is into a buildig.
	/// </summary>
	/// <returns>True if it is into a building; false otherwise.</returns>
	[[nodiscard]] bool IsInHolder(void) const;

	/// <summary>
	/// Check if the current unit is heir of the class "Ranged".
	/// Be careful: ranged is a no_cppclass!
	/// </summary>
	/// <returns>True if the unit is ranged; false otherwise.</returns>
	[[nodiscard]] bool IsRanged(void) const;

	/// <summary>
	/// This function sets a command for the current unit. 
	/// </summary>
	/// <param name="command">The command name (= command id).</param>
	/// <returns></returns>
	bool SetCommandWithoutTarget(const std::string& command);

	/// <summary>
	/// This function sets a command for the current unit. 
	/// </summary>
	/// <param name="command">The command name (= command id).</param>
	/// <param name="targetObj">A target object</param>
	/// <returns></returns>
	bool SetCommandWithTarget(const std::string& command, const std::shared_ptr<Target>& targetObj) override;
	#pragma endregion

	#pragma region Old To scripts methods:
	/// <summary>
	/// This function stops the current unit if it is moving.
	/// </summary>
	void Stop(void);

	/// <summary>
	/// This function retrieves the building in which the current unit is.
	/// </summary>
	/// <returns>The building in which the current unit is. Nullptr if the unit isn't in a building.</returns>
	[[nodiscard]] std::shared_ptr<Building> GetHostBuilding(void) const;
	#pragma endregion


	#pragma region Inherited methods:
	/// <summary>
	/// This function sets the player of current the object.
	/// </summary>
	/// <param name="par_playerID">The player number.</param>
	bool SetPlayer(const uint8_t _playerID) override;

	/// <summary>
	/// This function checks if an unit can be positioned during its creation.
	/// </summary>
	void CheckPlaceability(const bool bAlwaysPlaceable = false) override;

	void LookForFriendlySettlements(void);

	/// <summary>
	/// This function performs the current unit's game logic checks.
	/// </summary>
	void ApplyGameLogics(void) override;

	/// <summary>
	/// This function checks if the current unit is selected, namely if the selection includes it or its hero (if it has an hero).
	/// </summary>
	/// <returns>True if the unit is selected; false otherwise.</returns>
	[[nodiscard]] bool IsSelected(void) const noexcept override;

	bool Select(const uint8_t _playerID) override;

	[[nodiscard]] Unit* LookForEnemy(void);
	
	void ExecuteMethod(const std::string& method) override;

	virtual void GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const override;
	void GetTargetAndBehaviorBinRepresentation(std::vector<byte_t>& data);
	void SetTargetAndBehaviorByBinData(std::vector<byte_t>&& data, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap);
	#pragma endregion

	/// <summary>
	/// This function sets a weak reference to the building in which the current unit is.
	/// </summary>
	/// <param name="_building">A building reference.</param>
	void SetBuilding(std::weak_ptr<Building> _building);

	/// <summary>
	/// This function sets the hero who leads the current unit.
	/// </summary>
	/// <param name="hero">A hero reference.</param>
	void SetHero(std::weak_ptr<Hero> hero);

	/// <summary>
	/// This fnction checks if the current unit stands the position.
	/// </summary>
	/// <returns>True if it stands the position; false otherwise.</returns>
	bool IsStandPositionActive(void) const;

	/// <summary>
	/// This function checks if the current unit is dead and has completed it's "Dead animation".
	/// If the caller of this function receives true, it have to delete the current unit and set its pointer to null.
	/// Theoretically, only who calls Unit::Render should call this function or who try to save the unit into a binary save file.
	/// </summary>
	/// <returns>True if the object can be deleted; false otherwise.</returns>
	[[nodiscard]] bool IsToBeErased(void) const;

protected:
	#pragma region Inherited methods:
	std::ostream& Serialize(std::ostream& out, const bool calledByChild) const override;
	virtual void SetAttrs(const classData_t& objData, gobjData_t* dataSource) override;
	void SetObjectCommands(const classData_t& objData) override;
	#pragma endregion

	/// <summary>
	/// This function checks if the current unit has been select from te selection rectangle. 
	/// If so, unit will be added to the multiple selection
	/// </summary>
	void CheckSelectionArea(const IGame& igame);

	/// <summary>
	/// This function prepares the current unit to move.
	/// </summary>
	virtual void InitializeMovement(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Point>& destPt = nullptr);
private:
	//TODO - muovere in UnitAttributes?
	bool canFly = false;

	enum class Status
	{
		E_IDLE = 0,
		E_STAND_POSITION,

		//Movement status
		E_MOVING,
		E_ENTERING,
		E_APPROACHING,
		E_GOING_TO_ATTACK,
		E_GOING_TO_CONQUER,

		E_FIGHT_PREPARATION, //Animation "To attack", before real attack animation.
		E_FIGHTING,

		E_CONQUERING,

		E_DYING, //The unit is on the point of death.
		E_TO_ERASE, //Unit is no longer part of the game it will soon be destroyed.
	};
	
	struct Behavior_s
	{
		//Indicates the curret behavior of the unit
		Status status = Status::E_IDLE;

		//A weak reference to the settlement the unit is trying to conquer. Must be not empty only if the unit is in the "conquering" status
		std::weak_ptr<Settlement> targetSettlement;

		//Contains the player ID of the target settlement at the moment in which the unit has started to conquer. 
		uint8_t targetSettlementPlayer = 0;

		//A list of point to follow when the unit is in a "movement" status
		list<glm::ivec2> path;

		//The point towards which the unit is going. It is updated by extracting a point from the previous list whenever a point is reached.
		glm::ivec2 nextPathPoint{ -1, -1 };
	}	behavior;

	//A weak pointer to the building where the unit is located (the building must have a garrison). Empty if the unit is not in any building 
	std::weak_ptr<Building> buildingInWhichIAm;

	//A weak reference to the hero the unit is attached to. Empty id the unit is not attached to any hero
	std::weak_ptr<Hero> commander;

	//This boolean indicates if the unit has completed an "attack" animation. 
	//If true, the damage should be applied (if the unit is a melee) or a bullet should be throwned (if the unit is ranged).
	//Make sense only if the unit is in the "fighting" status.
	bool bAttackCycleCompleted = false;

	//The number of units killed by this unit
	uint32_t kills = 0;

	//A set containing the IDs of the friendly settlement close to the unit.
	std::unordered_set<uint32_t> friendlySettlementNear{};

	//TODO
	int chunkInWhichIAm = -1;
	list<Unit*>::const_iterator chunkInWhichIAmListIt;

	#pragma region Private methods:
	/// <summary>
	/// This function handles the behavior of the current unit after a right click.
	/// </summary>
	void ManageRightClick(void);

	/// <summary>
	/// This function changes the direction of the current unit using the coordinates of the mouse. 
	/// </summary>
	void ChangeDirection(void);

	/// <summary>
	/// This function changes the direction of the current unit
	/// </summary>
	/// <param name="xView">Point x to watch.</param>
	/// <param name="yView">Point y to watch.</param>
	void ChangeDirection(const int xView, const int yView);

	/// <summary>
	/// Frame by frame, this function approaches the current unit to the destination point previously passed through a LUA script followig a previously computed path.
	/// </summary>
	void FollowPath(void);

	/// <summary>
	/// This function computes a path for the current unit.
	/// </summary>
	void ComputePath(void);

	void GotToDestination(void);

	[[nodiscard]] bool CheckConquerConditions(const std::shared_ptr<Playable>& targetObj) const;

	/// <summary>
	/// Units starts to attack its target (if it has a valid one).
	/// </summary>
	void StartConquering(void);

	/// <summary>
	/// Frame by frame, this function manages the conquering of an enemy settlement.
	/// </summary>
	void Conquering(void);

	[[nodiscard]] bool CheckFightConditions(const std::shared_ptr<Playable>& targetObj) const;

	/// <summary>
	/// Units starts to attack its target (if it has a valid one).
	/// </summary>
	void StartFight(void);

	/// <summary>
	/// Frame by frame, this function manages the fight between units.
	/// </summary>
	void Fighting(void);

	void IncKills(const uint32_t newKills);

	/// <summary>
	/// This function informs the current unit that it has killed anyone.
	/// </summary>
	void NotifyKill(void);

	/// <summary>
	/// This function prepares the current unit to the death.
	/// </summary>
	void MarkAsDead(void);

	void FindChunkInWhichIAm(void);
	#pragma endregion
};
