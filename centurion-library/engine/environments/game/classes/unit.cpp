#include "unit.h"
#include "hero.h"
#include "building.h"

#include <environments/game/classes/VFX/parabola.h>

#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/classes/objectsSet/settlement.h>

#include <engine.h>
#include <environments/game/editor/editor.h>
#include <environments/game/match/pathfinding/jpsplus/pathfinding.h>  // TMP
#include <services/logservice.h>
#include <dialogWindows.h>

#include <bin_data_interpreter.h>
#include <mouse.h>
#include <keyboard.h>

#include <math_utils.h>

// Assets
#include <xml_class.h>
#include <xml_class_method.h>
#include <xml_script.h>
#include <xml_entity.h>


#pragma region Constructors and destructor:
Unit::Unit(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource) :
	Playable(_player, std::move(_pos), _bTemporary, _objData, surface, dataSource)
{
	this->SetPosition(_pos.x, _pos.y);

	const float defaultDirection = round((float)GObject::Directions::bottom / 360 * xml_entity->get_number_of_directions());
	this->SetCurrentDirection(static_cast<uint8_t>(defaultDirection));

	// TODO
	this->canFly = (this->GetZOffeset() >= 0);
}

Unit::~Unit(void)
{
	//if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match)
		//terrain.lock()->RemoveUnitFromChunk(this->chunkInWhichIAm, this, this->chunkInWhichIAmListIt);
}
#pragma endregion


#pragma region Static members:
void Unit::ComputeAndApplyDamage(const AttackerDamageParams_s& attacker, const std::weak_ptr<Unit>& attackerWP, Playable& target)
{
	int32_t damage = 0;
	if (target.IsUnit() == true)
	{
		Unit& defender = static_cast<Unit&>(target);
		// Max(Attacco Minimo, [Danno tipo attacco * Livello attaccante - Tipo difesa * Livello difensore])
		if (attacker.damageType == DamageTypes::e_pierce)
		{
			damage = attacker.maxAttack * attacker.level - defender.GetArmorPierce() * defender.GetLevel();
		}
		else  // Slash
		{
			damage = attacker.maxAttack * attacker.level - defender.GetArmorSlash() * defender.GetLevel();
		}
		damage = std::max(damage, static_cast<int32_t>(attacker.minAttack));

		// Apply damage
		defender.Damage(static_cast<uint32_t>(damage));

		// Manage the post damage 
		auto attackerSP = attackerWP.lock();
		const bool bAttackerIsStillValid = attackerSP && attackerSP->IsDead() == false;
		if (defender.IsDead() == true && bAttackerIsStillValid)
		{
			// Defender is dead and attacker is still alive, so notify the kill to the attacker.
			attackerSP->NotifyKill();
		}
	}
	else if (target.IsBuilding() == true)
	{
		// TODO - Formula temporanea
		damage = attacker.maxAttack;
		static_cast<Building&>(target).Damage(static_cast<uint32_t>(damage));
	}
}
#pragma endregion


#pragma region To Scripts members:
void Unit::Damage(const uint32_t damage)
{
	if (damage == 0 || this->IsDead() == true)
		return;

	if (damage >= this->health)
		this->SetHealth(0);  // Kill unit
	else
		this->SetHealth(this->health - damage);
}

void Unit::SetHealthByPercHealth(uint8_t perc_health)
{
	if (this->IsDead() == true)
		return;

	if (perc_health > 100)
		perc_health = 100;

	if (perc_health > 0)
		this->SetHealth(this->GetMaxHealth() * perc_health / 100);  // Heal unit
	else
		this->SetHealth(0);  // Kill unit

	// In order to fix percent_health setted by Unit::SetHealth
	this->UnitAttributes::SetPercentHealth(perc_health);
}

void Unit::SetHealth(uint32_t _health)
{
	// Unit is dead, its health can't change
	if (this->IsDead() == true)
		return;

	uint32_t maxHealth = this->GetMaxHealth();
	if (_health > maxHealth)
		_health = maxHealth;

	if (_health == 0)
	{
		if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor)
		{
			// In editor, it is senseless to have units with health equal to 0. There are two reasons:
			// 1) the unit would die immediately, so there would be no point in even placing it in the editor;
			// 2) if I set the health equal to 0 to make the unit die "scenically", it would make more sense to invoke a script like "Unit::Kill".
			// So simple ignore this situation.
			return;
		}

		// Unit now will die (ALWAYS first operation to do if _health == 0)
		this->MarkAsDead(); // Unit will soon destroyed.
	}

	// Update health.
	this->health = _health;
	const uint8_t oldPerHelth = this->GetPercentHealth();
	this->SetPercentHealth(static_cast<uint8_t>((static_cast<float>(_health) / static_cast<float>(maxHealth) * 100.0)));
	const uint8_t newPercentHealth = this->GetPercentHealth();

	/// Updating various health attributes.
	/// When a unit dies (health = 0), this part is not executed, since Unit::MarkAsDeas() removes the unit from all the collections.
	if (this->commander.expired() == false)
	{	// Update health for the hero leads the unit
		this->commander.lock()->GetArmy()->HandleHealthChangement(this->className, this->GetUniqueID(), oldPerHelth, newPercentHealth);
	}
	if (this->buildingInWhichIAm.expired() == false)
	{	// Update health for the garrison containg the unit
		this->buildingInWhichIAm.lock()->GetGarrison()->HandleHealthChangement(this->className, this->GetUniqueID(), oldPerHelth, newPercentHealth);
	}
	{	// For each player that has selected the unit, update the health of the selection
		auto playersArray = GObject::playersWeakRef.lock();
		for (auto const& playerID_it : this->playersThatSelectedMe)
		{
			playersArray->GetPlayerRef(playerID_it).lock()->GetSelection()->HandleHealthChangement(this->className, this->GetUniqueID(), oldPerHelth, newPercentHealth);
		}
	}
}

void Unit::Detach(void)
{
	auto leader = this->commander.lock();
	if (leader)
	{
		leader->GetArmy()->DetachUnit(std::static_pointer_cast<Unit>(this->me.lock()));
	}
}

std::shared_ptr<Hero> Unit::GetHero(void) const
{
	return this->commander.lock();
}

uint32_t Unit::GetKills(void) const
{
	return this->kills;
}

void Unit::GoTo(const std::shared_ptr<Point>& destPt)
{
	//Destination point must exist and must be valid.
	if (!destPt)
		return;
	//It is foolish to make a unit move in the same place in which it already is.
	if (destPt == this->GetPosition())
		return;

	this->GetMyTarget()->SetMinDistanceRequiredFromTarget(0);
	this->behavior.status = Status::E_MOVING;
	this->InitializeMovement(std::shared_ptr<Playable>(), destPt);
}

void Unit::GoToApproach(const std::shared_ptr<Playable>& targetObj, const float minDistance)
{
	//Target must exist a must be a unit.
	if (!targetObj || targetObj->IsUnit() == false)
		return;
	//The target to approach must be a different unit than 'this'
	if (targetObj->GetUniqueID() == this->GetUniqueID())
		return;

	this->behavior.status = Status::E_APPROACHING;

	//The unit is already quite close to its target
	if (minDistance >= 0 && this->DistTo(targetObj) <= minDistance)
	{
		this->GotToDestination();
		return;
	}


	this->GetMyTarget()->SetMinDistanceRequiredFromTarget(static_cast<uint16_t>(minDistance));
	this->InitializeMovement(targetObj);
}

void Unit::GoToAttack(const std::shared_ptr<Playable>& targetObj)
{
	if (this->CheckFightConditions(targetObj) == false)
		return;

	this->behavior.status = Status::E_GOING_TO_ATTACK;

	if (this->IsRanged() == true) 	//Ranged units don't need to get too close to its target...
		this->GetMyTarget()->SetMinDistanceRequiredFromTarget(this->GetRange());
	else //... but also melee units must keep a certain distance
		this->GetMyTarget()->SetMinDistanceRequiredFromTarget(MIN_RANGE_VALUE);

	this->InitializeMovement(targetObj);
}

void Unit::GoToConquer(const std::shared_ptr<Playable>& buildingTg)
{
	if (this->CheckConquerConditions(buildingTg) == false)
		return;

	this->behavior.status = Status::E_GOING_TO_CONQUER;
	this->GetMyTarget()->SetMinDistanceRequiredFromTarget(MAX_DISTANCE_TO_CONQUER_SETTLEMENT);

	this->InitializeMovement(buildingTg);
}

void Unit::GoToEnter(const std::shared_ptr<Playable>& buildingTg)
{
	//Target must exist and must be a building.
	if (!buildingTg || buildingTg->IsBuilding() == false)
		return;

	this->behavior.status = Status::E_ENTERING;
	this->GetMyTarget()->SetMinDistanceRequiredFromTarget(0);

	this->InitializeMovement(buildingTg, buildingTg->GetPosition());
}

bool Unit::IsDead(void) const
{
	return (this->behavior.status == Unit::Status::E_DYING || this->behavior.status == Unit::Status::E_TO_ERASE);
}

bool Unit::IsEntering(void) const
{
	return (this->behavior.status == Unit::Status::E_ENTERING);
}

bool Unit::IsInHolder(void) const
{
	return (this->buildingInWhichIAm.expired() == false);
}

bool Unit::IsRanged(void) const
{
	return this->IsHeirOf("ranged");
}

bool Unit::SetCommandWithoutTarget(const std::string& command)
{
	// Check if the unit has the command
	if (this->commandsMapId.contains(command) == false)
		return false; //Cannot execute command.
	// Get command, method and then script
	const std::shared_ptr<Command> cmdSP = this->commandsMapId.at(command);
	const auto& methodToExecute = this->methodsMap.at(cmdSP->GetMethodCRef());
	std::shared_ptr<centurion::assets::xml_script> script = methodToExecute->get_associated_script();
	// Check if the script associated to the method associated to the command doesn't require a target
	if (!script || script->is_target_required() == true)
		return false;  // Cannot execute command
	// Execute method associated to the command
	this->ExecuteMethod(cmdSP->GetMethodCRef());
	return true;
}

bool Unit::SetCommandWithTarget(const std::string& command, const std::shared_ptr<Target>& targetObj)
{
	/// WARNING: If you modify this method, give a look even to Building::SetCommandWithTarget

	// Check if the target is valid and if the unit has the command
	if (!targetObj || this->commandsMapId.contains(command) == false)
		return false;  // Cannot execute command.

	// Get command, method and then script
	const std::shared_ptr<Command> cmdSP = this->commandsMapId.at(command);
	const auto& methodToExecute = this->methodsMap.at(cmdSP->GetMethodCRef());
	std::shared_ptr<centurion::assets::xml_script> script = methodToExecute->get_associated_script();

	// Check if the script associated to the method associated to the command requires a target
	if (!script || script->is_target_required() == false)
		return false;  // Cannot execute command.

	// Reset old target and set a new one
	this->SetTarget(Target::CreateTarget());
	this->SetMyTarget(targetObj->GetObject(), targetObj->GetPoint());

	// Execute method associated to the command.
	this->ExecuteMethod(cmdSP->GetMethodCRef());

	return true;
}
#pragma endregion

void Unit::Stop(void)
{
	this->behavior.status = Status::E_IDLE;
	this->ChangeAnim("idle");
	// Unit now doesn't have any path to follow.
	this->behavior.path.clear();
	this->behavior.nextPathPoint.x = -1;
	this->behavior.nextPathPoint.y = -1;
}

std::shared_ptr<Building> Unit::GetHostBuilding(void) const
{
	return this->buildingInWhichIAm.lock();
}

Unit* Unit::LookForEnemy(void)
{
	// TODO
	if (this->IsDead() == true)
		return nullptr;
	if (this->chunkInWhichIAm == -1)
		return nullptr;

	Unit* enemy = nullptr;
	double d = (double)visibleMapSize.x;  // distance

	// units in same chunk
	auto units = terrain.lock()->GetUnitsInsideChunk(this->chunkInWhichIAm);
	if (units == nullptr)
		return nullptr;
	for (uint8_t iPlayer = 1; iPlayer <= 8; iPlayer++)
	{
		if (iPlayer == this->GetPlayer())
			continue;
		for (auto const& u : (*units)[iPlayer - 1])
		{
			if (u->IsDead() == true)
				continue;
			double ud = Math::euclidean_distance(this->GetPositionX(), this->GetPositionY(), u->GetPositionX(), u->GetPositionY());
			if (ud >= this->GetSight())
				continue;
			if (ud >= d)
				continue;

			d = ud;
			enemy = u;
		}
	}

	// units in adjacent chunks
	auto chunks = terrain.lock()->Get8AdjacentChunksIndicesByIndex(this->chunkInWhichIAm);
	for (auto const& idx : chunks)
	{
		auto units = terrain.lock()->GetUnitsInsideChunk(idx);
		if (units == nullptr)
			return enemy;  // Return enemy in the same chunk if it exists).
		for (uint8_t iPlayer = 1; iPlayer <= 8; iPlayer++)
		{
			if (iPlayer == this->GetPlayer())
				continue;
			for (auto const& u : (*units)[iPlayer - 1])
			{
				if (u->IsDead() == true)
					continue;
				double ud = Math::euclidean_distance(this->GetPositionX(), this->GetPositionY(), u->GetPositionX(), u->GetPositionY());
				if (ud >= this->GetSight())
					continue;
				if (ud >= d)
					continue;

				d = ud;
				enemy = u;
			}
		}
	}

	return enemy;
}

#pragma region Inherited methods
bool Unit::SetPlayer(const uint8_t _playerID)
{
	if (this->GetPlayer() == _playerID || _playerID == PlayersArray::UNDEFINED_PLAYER_INDEX || _playerID > PlayersArray::NUMBER_OF_PLAYERS)
		return false;

	if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match)
	{
		// TODO
		terrain.lock()->RemoveUnitFromChunk(this->chunkInWhichIAm, this, this->chunkInWhichIAmListIt);
		this->chunkInWhichIAmListIt = terrain.lock()->AddUnitToChunk(this->chunkInWhichIAm, this);

		// The unit is trying to conquering a settlement
		if (this->behavior.status == Status::E_CONQUERING)
		{
			const std::shared_ptr<Settlement> targetSettlement = this->behavior.targetSettlement.lock();
			targetSettlement->RemoveBesieger((*this));
			this->Playable::SetPlayer(_playerID);
			targetSettlement->AddBesieger((*this));
			return true;  // In order to avoid a second call to GObject::SetPlayer.
		}
	}
	this->Playable::SetPlayer(_playerID);
	return true;
}

void Unit::CheckPlaceability(const bool bAlwaysPlaceable)
{
	this->bIsPlaceable = true;
	if (bAlwaysPlaceable == true)
		return;
	//if (rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(340) || rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(344)) // left&right shift 
	//	return;

	glm::vec3 var_position = this->GetPosition()->ToVec3();
	GObject::PlacementErrors errorCode = PlacementErrors::e_no_error;
	if (var_position.x <= GRID_CELL_SIZE || var_position.x >= visibleMapSize.x - GRID_CELL_SIZE ||
		var_position.y <= GRID_CELL_SIZE || var_position.y >= visibleMapSize.y - GRID_CELL_SIZE)
	{
		this->bIsPlaceable = false;
		errorCode = GObject::PlacementErrors::e_map_border_limit;
	}

	// ABILITARE QUESTA SEZIONE DI CODICE QUANDO E' STATO IMPLEMENTATO IL CONTROLLO AL POSIZIONAMENTO DELLE UNITA' SU STRUTTURE O TERRENI INACCESSIBILI
	//if (Grid::CheckObjectGridAvailability(this->grid, var_position) == false)
	//{
	//	this->bIsPlaceable = false;
	//	errorCode = 0;
	//}

	if (this->bIsPlaceable == false)
		this->SetPlacementError(errorCode);
	if (this->bisTemporaryMultiple == false) 
		this->SendInfoText();
}

void Unit::LookForFriendlySettlements(void)
{
	// TODO - valutare le prestazioni in gioco per tante unità e tanti settlement. In caso farla svolgere a un secondo thread.
	const auto& settlements = GObject::playersWeakRef.lock()->GetSettlementsCollectionRef().GetSettlementsMapCRef();
	for (auto const& [settID, sett] : settlements)
	{
		const std::shared_ptr<Settlement> setSP = sett.lock();
		assert(setSP);
		assert(settID == setSP->GetId());
		bool bNear = false;
		if (setSP->GetPlayer() == this->GetPlayer())  // Check that unit and the settlement are friends each other.
		{
			std::shared_ptr<ObjsList> buildings = setSP->GetBuildings();
			ObjsList::objs_const_iterator endIt = buildings->cend();
			for (auto buildingIt = buildings->cbegin(); buildingIt != endIt; buildingIt++)
			{
				if (this->DistTo((*buildingIt).lock()) <= MAX_DISTANCE_TO_DEFEND_SETTLEMENT)
				{
					bNear = true;
					if (this->friendlySettlementNear.contains(settID) == false)
					{
						// Unit is enough near to a friendly settlement so the unit can defend it.
						this->friendlySettlementNear.insert(settID);
						setSP->AddDefender((*this));
					}
					break;
				}
			}
		}
		if (bNear == false && this->friendlySettlementNear.contains(settID) == true)
		{
			// Unit is not close to the current settlement but it was previously one of its defenders. 
			// So the unit can't defend the settlement anymore.
			// Reasons for this are: 1) settlement isn't a friend anymore; unit is more distant from it.
			this->friendlySettlementNear.erase(settID);
			setSP->RemoveDefender((*this));
		}
	}
}

void Unit::ApplyGameLogics(void)
{
	// Update the Map Area
	this->FindChunkInWhichIAm();

	auto iGame = Engine::GetInstance().GetEnvironment()->AsIGame();
	// Check if the unit is selected by selection rectangle.
	this->CheckSelectionArea((*iGame));

	if (iGame->GetType() == IEnvironment::Environments::e_match)
	{
		if (this->IsDead() == false)
		{
			// TODO ottenere l'elenco di giocatori che hanno selezionato l'eroe.
			// Unit will be deselect if its hero is selected.
			if (this->commander.expired() == false && this->commander.lock()->IsSelected() == true)
				this->Deselect(1);

			// For each command in the queue, try to run corresponding script.
			this->commandsQueue->ExecuteReadyCommands();

			// Status management
			if (this->behavior.targetSettlement.expired() == false && this->behavior.status != Status::E_CONQUERING)
			{
				// This happens when a unit started to execute a new command when it was trying to conquer a settlement.
				this->behavior.targetSettlement.lock()->RemoveBesieger((*this));
				this->behavior.targetSettlement = std::weak_ptr<Settlement>();
			}
			if (this->behavior.status >= Status::E_MOVING && this->behavior.status <= Status::E_GOING_TO_CONQUER)  // Moving behavior
			{
				if (this->behavior.path.empty() == false || this->behavior.nextPathPoint.x > -1)
				{	// Unit has already a path and so can follow it
					this->FollowPath();
				}
				else 
				{	// Unit need a path: compute it
					this->ComputePath();
				}
			}
			else if (this->behavior.status == Status::E_FIGHTING)  // Fighting behavior
			{
				this->Fighting();
			}
			else if (this->behavior.status == Status::E_CONQUERING)  // Conquering behavior
			{
				this->Conquering();
			}
			//else if (this->bIsGoingToAttack == false && bToAttack == false && (this->bStandPos == false || this->IsRanged() == true))
			{
				//TODO
				//* enemy = this->LookForEnemy();
				//if (enemy != nullptr)
				//{
					//this->SetCommandWithTarget("attack", enemy);
				//}
			}

			this->LookForFriendlySettlements();
		}

		// Verify if it's time to change frame
		auto currentGameFrame = Engine::GetGameTime().GetCurrentFrame();

#pragma region TEMPORARY -- REMOVE THIS PART TO CATCH THE EXCEPTION
		if (this->currentFrame >= this->xml_entity->get_anim_number_of_frames(this->currentState, this->currentAnim))
		{
			this->currentFrame = 0;  // Restart animation.
		}
#pragma endregion

		if (currentGameFrame - this->framesAnimVariation >= this->xml_entity->get_anim_frame_duration(this->currentState, this->currentAnim))
		{
			this->framesAnimVariation = currentGameFrame;
			
			if (this->behavior.status == Status::E_IDLE && this->currentFrame == 0)
			{
				// When is IDLE the animation starts randomly
				int seed = Engine::GetInstance().GetGameTime().GetCurrentFrame() + (int)this->GetUniqueID();
				bool bStartIdleAnimation = Math::runif_discrete(1, 30, seed) == 1;
				if (bStartIdleAnimation)
				{
					this->currentFrame++;
				}
			}
			else
				this->currentFrame++;
			
			if (this->currentFrame >= this->xml_entity->get_anim_number_of_frames(this->currentState, this->currentAnim))
			{
				this->currentFrame = 0;  // Restart animation.
				if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match)
				{
					// A cycle of the "Attack" animation it'over.
					if (this->behavior.status == Status::E_FIGHTING)
						this->bAttackCycleCompleted = true;

					//Switch from "ToAttack" animation to "Attack" animation. Real fighting will be soon start.
					if (this->behavior.status == Status::E_FIGHT_PREPARATION)
					{
						this->ChangeAnim("fight");
						this->behavior.status = Status::E_FIGHTING;
						this->bAttackCycleCompleted = false;
					}

					// Unit is exipired. Destroy it and return
					if (this->behavior.status == Unit::Status::E_DYING)
					{
						this->behavior.status = Unit::Status::E_TO_ERASE;
						return;
					}
				}
			}
		}
	}

	// Update values of each multiple selection
	{
		assert(GObject::playersWeakRef.expired() == false);
		auto players = GObject::playersWeakRef.lock();
		auto& attrValues = this->GetAttributesValues();
		for (auto const playerID : this->playersThatSelectedMe)
		{
			const auto player = players->GetPlayerRef(playerID);
			assert(player.expired() == false && this->me.expired() == false);
			player.lock()->GetSelection()->UpdateAttributes(attrValues, this->GetSight(), this->IsRanged());
		}
	}

	// Right click management
	this->ManageRightClick();
}

bool Unit::IsSelected(void) const noexcept
{
	// A unit is selected if it was directly selected or its hero was selected (in this second case, it's an indirect selection)
	const bool bDirectlySelected = GObject::IsSelected();
	const bool bHasHeroSelected = (this->commander.expired() == false && this->commander.lock()->IsSelected());
	return bDirectlySelected || bHasHeroSelected;
}

bool Unit::Select(const uint8_t _playerID)
{
	if (this->IsDead() == true)
		return false;  // Do NOT select a dead unit
	if (this->commander.expired() == false && commander.lock()->IsSelected() == true)
		return false;  // Return since unit is already selected indirectly beacuse its hero is selected

	this->GObject::Select(_playerID);
	return true;
}

void Unit::ExecuteMethod(const std::string& method)
{
	if (this->HasMethod(method) == false)
		return;
	this->Playable::ExecuteMethod(method);
}

void Unit::GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const
{
	assert(this->behavior.status != Status::E_TO_ERASE);
	// Firstly, calls always GObject::GetBinSignature
	if (calledByChild == false)
		this->GObject::GetBinSignature(data);

	const bool bIsDying = (this->behavior.status == Status::E_DYING);
	BinaryDataInterpreter::PushBoolean(data, bIsDying);
	if (bIsDying == true)
		return;  // If unit is dying, it is useless to save the rest since unit is no longer controllable (it has just to play its animation "die")
	const bool bHasActiveStatus = (this->behavior.status != Status::E_IDLE && this->behavior.status != Status::E_STAND_POSITION);
	BinaryDataInterpreter::PushBoolean(data, bHasActiveStatus);
	if (bHasActiveStatus == true)
		GObject::playersWeakRef.lock()->AddUnitHavingAnActiveBehavior((*this));

	this->UnitAttributes::GetXmlAttributesAsBinaryData(data);

	///If you change saving order here, please go to Unit::SetAttrs and impose the same loading order!
	// Unsigned 32
	BinaryDataInterpreter::PushUInt32(data, this->kills);
	BinaryDataInterpreter::PushString(data, this->currentState);
	BinaryDataInterpreter::PushString(data, this->currentAnim);
	BinaryDataInterpreter::PushUInt32(data, this->currentFrame);
	// Unsigned 8
	BinaryDataInterpreter::PushUInt8(data, this->currentDirection);
	
	// As a last thing, calls always method GObject::GetBinRepresentation
	if (calledByChild == false)
		this->Playable::GetBinRepresentation(data, calledByChild);
}

void Unit::GetTargetAndBehaviorBinRepresentation(std::vector<byte_t>& data)
{
	assert(this->IsDead() == false);

	// Firstly, save target
	this->GetMyTarget()->GetBinRepresentation(data);

	// Then save behavior/status and (if there are) some info about
	const Status& status = this->behavior.status;
	BinaryDataInterpreter::PushUInt32(data, static_cast<uint32_t>(status));
	switch (status)
	{
	case Status::E_CONQUERING:
		assert(this->behavior.targetSettlement.expired() == false);
		BinaryDataInterpreter::PushUInt8(data, this->behavior.targetSettlementPlayer);
		break;
	case Status::E_MOVING:
	case Status::E_ENTERING:
	case Status::E_APPROACHING:
	case Status::E_GOING_TO_ATTACK:
	case Status::E_GOING_TO_CONQUER:
		BinaryDataInterpreter::PushIVec2(data, this->behavior.nextPathPoint);
		BinaryDataInterpreter::PushIVec2List(data, this->behavior.path);
		break;
	default:
		// Nothing else to save for other status
		break;
	}
}

void Unit::SetTargetAndBehaviorByBinData(std::vector<byte_t>&& data, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap)
{
	if (this->IsDead() == true)
		throw BinaryDeserializerException("Trying to load target and status for dead unit");

	uint32_t offset = 0;
	// Firstly, load target
	this->GetMyTarget()->InitByBinData(data, offset, objsMap);

	std::shared_ptr<GObject> target = this->GetMyTarget()->GetObject();
	// Then load status
	this->behavior.status = static_cast<Unit::Status>(BinaryDataInterpreter::ExtractUInt32(data, offset));
	switch (this->behavior.status)
	{
	case Status::E_CONQUERING:
		this->behavior.targetSettlementPlayer = BinaryDataInterpreter::ExtractUInt8(data, offset);
		this->StartConquering();
		break;
	case Status::E_MOVING:
	case Status::E_ENTERING:
	case Status::E_APPROACHING:
	case Status::E_GOING_TO_ATTACK:
	case Status::E_GOING_TO_CONQUER:
		this->behavior.nextPathPoint = BinaryDataInterpreter::ExtractIVec2(data, offset);
		this->behavior.path = BinaryDataInterpreter::ExtractIVec2List(data, offset);
		break;
	default:
		// Nothing else to load for other status
		break;
	}
}
#pragma endregion

#pragma region Public members:
void Unit::SetBuilding(std::weak_ptr<Building> _building)
{
	this->buildingInWhichIAm = std::move(_building);
}

void Unit::SetHero(std::weak_ptr<Hero> hero)
{
	// Heroes cannot be assigned to an other hero.
	if (this->IsHero() == true)
		return;
	this->commander = std::move(hero);
}

bool Unit::IsStandPositionActive(void) const
{
	return (this->behavior.status == Status::E_STAND_POSITION);
}

bool Unit::IsToBeErased(void) const
{
	return (this->behavior.status == Unit::Status::E_TO_ERASE);
}
#pragma endregion

#pragma region Protected members:
#pragma region Inherited methods:
std::ostream& Unit::Serialize(std::ostream& out, const bool calledByChild) const
{
	const std::string tabs(GObject::nTabs, '\t');
	if (calledByChild == false)
		out << tabs 
			<< "<unit class=\"" << this->GetClassName() << "\" "
			<< "id=\"" << std::to_string(this->GetUniqueID()) << "\" " 
			<< "xPos=\"" << std::to_string(this->GetPositionX()) << "\" "
			<< "yPos=\"" << std::to_string(this->GetPositionY()) << "\""
			<< ">\n";

	this->GObject::Serialize(out, true);
	this->UnitAttributes::Serialize(out, tabs);
	out << tabs << "\t<direction>" << std::to_string(this->currentDirection) << "</direction>\n";

	if (calledByChild == false)
		out << tabs << "</unit>";
	return out;
}

void Unit::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	if (dataSource == nullptr)
	{
		this->UnitAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);
		this->kills = 0; 
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		this->UnitAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->SetCurrentDirection(tinyxml2::TryParseFirstChildIntContent(xml, "direction"));
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		// If you change loading order here, please go to Unit::GetBinRepresentation and impose the same saving order!
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		const bool bIsDying = BinaryDataInterpreter::ExtractBoolean((*binData.first), binData.second);
		if (bIsDying == true)
		{
			// If unit was dying when saved, so there is nothing else to load since unit is no longer controllable (it has just to play its animation "die").
			// So simply lets it die and return
			this->SetHealth(0);
			return; 
		}

		// Load if the unit has an active status. If so, notify it to PlayersArray
		const bool bHasActiveStatus = BinaryDataInterpreter::ExtractBoolean((*binData.first), binData.second);
		if (bHasActiveStatus == true)
			GObject::playersWeakRef.lock()->AddUnitHavingAnActiveBehavior((*this));

		// Load XML attributes
		this->UnitAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);

		// Unsigned 32
		this->kills = BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second);
		this->SetCurrentState(BinaryDataInterpreter::ExtractString((*binData.first), binData.second));
		this->SetCurrentAnim(BinaryDataInterpreter::ExtractString((*binData.first), binData.second));
		this->SetCurrentFrame(BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second));

		// Unsigned 8
		this->SetCurrentDirection(BinaryDataInterpreter::ExtractUInt8((*binData.first), binData.second));
	}

	this->SetHealthByPercHealth(this->GetPercentHealth());  // percent_health value is setted in UnitAttributes::SetAttributes

	// As last thing, call GObject::SetAttrs
	Playable::SetAttrs(objData, dataSource);
}

void Unit::SetObjectCommands(const classData_t& objData)
{
	const auto& cmdsVec = objData->get_commands_list_cref();

	for (size_t i = 0; i < cmdsVec.size(); i++)
	{
		// ASSERTION: A command ID must be unique: unit must not have repeated commands
		assert(this->commandsMapId.contains(cmdsVec[i]->GetIdCRef()) == false);
		// ASSERTION: Only buildings can have training commands or technology commands
		assert(cmdsVec[i]->GetType() != assets::xml_command_type::Training && cmdsVec[i]->GetType() != assets::xml_command_type::Technology);

		// Create a new command
		std::shared_ptr<Command> cmd = Command::CreateCommand(*cmdsVec.at(i), false);

		// Save the command
		this->commandsMapId.insert({ cmd->GetIdCRef(), cmd });
		this->commandsVec.push_back(std::move(cmd));
	}
}
#pragma endregion

void Unit::CheckSelectionArea(const IGame& igame)
{
	auto selectionArea = igame.GetCurrentScenario()->GetSelectionAreaPtr().lock();
	if (selectionArea->IsActive() == false)
		return;

	// Get current player ID.
	const auto playersArray = GObject::playersWeakRef.lock();
	const uint8_t currentPlayerID = playersArray->GetCurrentPlayerID();

	// Into a match game, a player can select an unit with the rectangle selection only if the unit belongs to him.
	// Otherwise, in the editor, selection rectangle can select every unit (if there isn't any decoration already selected!!!).
	const IEnvironment::Environments envType = igame.GetType();
	if (envType == IEnvironment::Environments::e_match && igame.IsCurrentPlayer(this->GetPlayer()) == false)
		return;
	else if (envType == IEnvironment::Environments::e_editor)
	{
		auto objSO = playersArray->GetPlayerRef(currentPlayerID).lock()->GetSelection()->Get(0);
		if (objSO && objSO->IsDecoration() == true)
			return;  // At least a decoration was previously selected by rectangle area, so ignore the unit.
	}

	// Don't try to select units into a building.
	if (this->IsInHolder() == true)
		return;

	// Check if the unit is taken by selection rectangle.
	const bool bSelectedFromRect = selectionArea->Intersect(this->HitBox);
	if (bSelectedFromRect == true)
		this->Select(currentPlayerID);
	else if (this->IsSelected() == true)
		this->Deselect(currentPlayerID);

}

void Unit::InitializeMovement(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Point>& destPt)
{
	// Current unit watches its target.
	this->ChangeDirection(this->GetMyTarget()->GetPoint()->x, this->GetMyTarget()->GetPoint()->y);

	// Eventually, reset old path so a new one can be computed.
	this->behavior.path.clear();
	this->behavior.nextPathPoint = { -1, -1 };

	// Set a target.
	this->SetMyTarget(objSP, destPt);

	// If the unit is into a building, it will exit.
	if (this->buildingInWhichIAm.expired() == false)
	{
		bool bIsNotConcretelyInTheGarrison = false;
		std::shared_ptr<Hero> unitCommander = this->commander.lock();
		// If the unit has an hero and if they are in the same building remove the unit from the units in the same building of its hero.
		if (unitCommander && unitCommander->IsInHolder() == true && unitCommander->GetHostBuilding() == this->buildingInWhichIAm.lock())
		{
			bIsNotConcretelyInTheGarrison = this->commander.lock()->GetArmy()->UnmarkUnitAsInTheSameBuilding(std::static_pointer_cast<Unit>(this->me.lock()));
		}

		// The unit will be removed from the garrison of the building in which it is.
		if (bIsNotConcretelyInTheGarrison == false)
		{
			// If Im' here, garrison contains CONCRETELY the unit. It means that the unit doesn't have an hero or that they there aren't in the same building.
			const bool bRemoved = this->buildingInWhichIAm.lock()->GetGarrison()->RemoveUnit(std::static_pointer_cast<Unit>(this->me.lock()));
			assert(bRemoved == true);
		}
		else
		{
			// If I'm here, garrison doesn't contain CONCRETELY the unit, since unit and its hero were in the same building.
			this->buildingInWhichIAm.lock()->GetGarrison()->DecreaseNumberOfTroops(1);
			this->buildingInWhichIAm = std::weak_ptr<Building>();
		}
	}

	// Preparing to movement
	this->ChangeAnim("walk");
}
#pragma endregion

#pragma region Private members:
void Unit::ManageRightClick(void)
{
	if (rattlesmake::peripherals::mouse::get_instance().RightClick == false)  // Of course, right click must be clicked
		return;
	if (this->GObject::IsSelected() == false)  // Check if unit is directly selected
		return;
	auto environment = Engine::GetInstance().GetEnvironment()->AsIGame();
	auto ui = environment->GetUIRef();
	if (ui->AnyIframeIsOpened() == true || gui::IsAnyDialogWindowActive() == true)
		return;
	// Selection area must not be visible (maybe this should be only in editor)
	auto selectionArea = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSelectionAreaPtr().lock();
	if (selectionArea->IsActive() == true)
		return;

	std::list<std::string> const* cmds = nullptr;
	const IEnvironment::Environments environmentId = environment->GetType();
	if (environmentId == IEnvironment::Environments::e_editor)
	{
		if (static_cast<Editor*>(environment)->IsShiftingObject() == true || static_cast<Editor*>(environment)->IsChangingTerrain() == true)
			return;

		this->ChangeDirection();
		std::weak_ptr<Adventure> adventure = environment->GetCurrentAdventureW();
		adventure.lock()->MarkAsEdited();
	}
	else if (environmentId == IEnvironment::Environments::e_match)
	{
		if (this->defaultCommands == nullptr || this->defaultCommands->empty() == true)
			return;
		const bool bIsCurrentPlayer = environment->IsCurrentPlayer(this->GetPlayer());
		if (bIsCurrentPlayer == false)
			return;

		const std::shared_ptr<Point> pt = Point::GetClickedPoint();
		const std::shared_ptr<Target> tg = this->GetPlayerRef()->GetObjsCollection()->TryToGetCapturedTarget(true); 
		// ASSERTION: it must have been possible to obtain a target (previously captured with the right mouse button).
		assert(tg);

		// Search for default commands that can be applied to the target.
		for (auto const& defaultCmd_it : (*this->defaultCommands))
		{
			if (!tg->GetObject())
			{
				if (defaultCmd_it->GetTargetCRef() == "null")
				{
					cmds = defaultCmd_it->GetCommandListCPtr();
					break;
				}
				continue;
			}
			if (tg->GetObject()->IsHeirOf(defaultCmd_it->GetTargetCRef()) == true)
			{
				cmds = defaultCmd_it->GetCommandListCPtr();
				break;
			}
		}

		// Try to execute a default command
		if (cmds != nullptr)
		{
			this->SetTarget(tg);
			for (auto const& cmd : (*cmds))
			{
				if (this->CheckCommandCondition(cmd) == true)
				{
					this->ExecuteMethod(this->commandsMapId.at(cmd)->GetMethodCRef());
					break;  // Stop because a default command can be executed
				}
			}
		}
	}
}

void Unit::ChangeDirection(void)
{
	auto pos = glm::vec2(this->GetPositionX(), this->GetPositionY());
	const float angle = Math::GetViewAngle(pos.x, pos.y, rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate(), rattlesmake::peripherals::mouse::get_instance().GetY2DMapCoordinate());
	this->SetCurrentDirection(static_cast<uint8_t>(round(angle / 360 * xml_entity->get_number_of_directions())));
}

void Unit::ChangeDirection(const int xView, const int yView)
{
	auto pos = this->GetPosition();
	const float angle = Math::GetViewAngle(pos->x, pos->y, (float)xView, (float)yView);
	this->SetCurrentDirection(static_cast<uint8_t>(round(angle / 360 * xml_entity->get_number_of_directions())));
}

void Unit::GotToDestination(void)
{
	// Manage status
	if (this->behavior.status == Status::E_ENTERING)
	{
		// Add the unit to the target building.
		auto target = this->GetMyTarget()->GetObject();
		assert(target && target->IsBuilding() == true);
		std::static_pointer_cast<Building>(target)->GetGarrison()->AddUnit(std::static_pointer_cast<Unit>(this->me.lock()), true, std::nullopt);
	}
	else if (this->behavior.status == Status::E_GOING_TO_ATTACK)
	{
		this->StartFight();
		return;  // In order to don't stop unit calling this->Stop() at the end of this function.
	}
	else if (this->behavior.status == Status::E_GOING_TO_CONQUER)
	{
		this->StartConquering();
		return;  // In order to don't stop unit calling this->Stop() at the end of this function.
	}
	else if (this->behavior.status == Status::E_APPROACHING)
	{
		auto tg = this->GetMyTarget()->GetObject();
		if (tg->IsHero() == true)
		{
			std::static_pointer_cast<Hero>(tg)->GetArmy()->AssignUnit(std::static_pointer_cast<Unit>(this->me.lock()), std::nullopt);
		}
	}

	// Now unit is idle.
	this->Stop();
}

void Unit::FollowPath(void)
{
	const static float elapsedTime = 1.0f / 60.0f;
	// Parameter for movement:
	const std::shared_ptr<Point> pos = this->GetPosition();
	const float startXPos = pos->x;
	const float startYPos = pos->y;
	if (this->behavior.nextPathPoint.x == -1 && this->behavior.nextPathPoint.y == -1)
	{
		const static int terrainHeight = visibleMapSize.y;
		// Get the next point of the path starting from the cell of the grid in which the point is.
		if (this->behavior.path.size() > 1)
		{
			this->behavior.nextPathPoint.x = this->behavior.path.front().x * GRID_CELL_SIZE;
			this->behavior.nextPathPoint.y = (-this->behavior.path.front().y) * GRID_CELL_SIZE + terrainHeight;
		}
		else
		{
			auto& myTarget = this->GetMyTarget();
			// In order to have exactly the destination point (since the last cell contains the center pixel of the cell).
			this->behavior.nextPathPoint.x = myTarget->GetPoint()->x;
			this->behavior.nextPathPoint.y = myTarget->GetPoint()->y;
		}
		this->behavior.path.pop_front();
	}
	//const float endXPos = (this->target.GetObject() != nullptr) ? this->target.GetObject()->GetPositionX() : this->target.GetPoint().x;
	//const float endYPos = (this->target.GetObject() != nullptr) ? this->target.GetObject()->GetPositionY() : this->target.GetPoint().y;
	const float endXPos = (float)this->behavior.nextPathPoint.x;
	const float endYPos = (float)this->behavior.nextPathPoint.y;
	const float startDistance = float(sqrt(pow(startXPos - endXPos, 2) + pow(startYPos - endYPos, 2)));
	const float directionX = (endXPos - startXPos) / startDistance;
	const float directionY = (endYPos - startYPos) / startDistance;

	// New position for the current frame:
	const float newXPos = startXPos + directionX * this->GetSpeed() * 4 * elapsedTime;
	const float newYPos = startYPos + directionY * this->GetSpeed() * 4 * elapsedTime;
	this->SetPosition(newXPos, newYPos);
	const float newDistance = float(sqrt(pow(pos->x - startXPos, 2) + pow(pos->y - startYPos, 2)));
	
	const uint16_t minDistanceRequiredFromTarget = this->GetMyTarget()->GetMinDistanceRequiredFromTarget();
	// Check if unit has come to the current point of its path
	if (newDistance + minDistanceRequiredFromTarget >= startDistance)
	{
		if (this->behavior.path.empty() == true)  // Check if unit has come to destination point:
		{
			// Get to the point. Unit now is idle.
			if (minDistanceRequiredFromTarget == 0)
				this->SetPosition(endXPos, endYPos);

			this->GotToDestination();
		}
		else
		{
			// Unit now will start to go to the next point of the path.
			this->behavior.nextPathPoint.x = -1;
			this->behavior.nextPathPoint.y = -1;
		}
	}
	else
	{
		// Current unit watches its target.
		this->ChangeDirection((int)endXPos, (int)endYPos);
	}
}

void Unit::ComputePath(void)
{
	const std::shared_ptr<Point> pos = this->GetPosition();
	// Get "playable" height of the terrain.
	const static int terrainHeight = visibleMapSize.y;
	// Get starting cell and destination cell starting from the position of the current unit.
	// Y is shifted by a value equal to height of the map in order to have a grid with the y = 0 in the upper left edge of the map
	const int startCellX = (int)pos->x / GRID_CELL_SIZE;
	const int startCellY = (int)((terrainHeight - pos->y) / GRID_CELL_SIZE);
	auto& myTarget = this->GetMyTarget();
	//std::cout << "\n" << myTarget << "(" << myTarget->GetPoint()->x << ", " << myTarget->GetPoint()->y << ")";
	const int endCellX = (int)myTarget->GetPoint()->x / GRID_CELL_SIZE;
	const int endCellY = (int)((terrainHeight - myTarget->GetPoint()->y) / GRID_CELL_SIZE);

	this->behavior.path = path_finding::Get()->GetPath(glm::ivec2{ startCellX, startCellY }, glm::ivec2{ endCellX, endCellY });
	if (this->behavior.path.empty() == false)
	{
		// The cell in which the unit is will be removed from the path.
		this->behavior.path.pop_front();
	}

	if (this->behavior.path.empty() == true)
	{
		// Invalid destination: stop the unit.
		this->Stop();
	}
}

bool Unit::CheckConquerConditions(const std::shared_ptr<Playable>& targetObj) const
{
	// Target must exist a must be a building
	if (!targetObj || targetObj->IsBuilding() == false)
		return false;
	// The target building must be a central building:
	if (std::static_pointer_cast<Building>(targetObj)->IsCentralBuilding() == false)
		return false;
	// The target can't be the building itself (I know: it's really strange that a building can conquer another building, but Pongo wants to make data.zip open)
	if (targetObj->GetUniqueID() == this->GetUniqueID())
		return false;
	// It's possible to conquer only an enemy building
	if (this->GetPlayer() == targetObj->GetPlayer())
		return false;
	// Settlement must have the same player ID it had at the time the unit started the conquest. Important to avoid multiple conquests if more players with the same number of units are conquering at the same time
	if (this->behavior.status == Status::E_CONQUERING && targetObj->GetPlayer() != this->behavior.targetSettlementPlayer)
		return false;
	// Current unit must be alive
	if (this->IsDead() == true)
		return false;

	return true; //Ok
}

void Unit::StartConquering(void)
{
	// Check if conquering is already possible.
	if (this->CheckConquerConditions(this->GetMyTarget()->GetObject()) == false)
	{
		this->Stop();
		return;
	}

	// Preparation for capture...
	this->ChangeAnim("capture");
	this->behavior.targetSettlement = std::static_pointer_cast<Building>(this->GetMyTarget()->GetObject())->GetSettlement();
	this->behavior.targetSettlementPlayer = this->behavior.targetSettlement.lock()->GetPlayer();
	// Notify the settlement that the unit is trying to conquer it.
	this->behavior.targetSettlement.lock()->AddBesieger((*this));

	this->behavior.status = Status::E_CONQUERING;
}

void Unit::Conquering(void)
{
	assert(this->behavior.status == Status::E_CONQUERING);
	// Check if fight is already possible.
	if (this->CheckConquerConditions(this->GetMyTarget()->GetObject()) == false)
	{
		// Notify the settlement that the unit isn't trying to conquer it anymore.
		this->behavior.targetSettlement.lock()->RemoveBesieger((*this));
		this->behavior.targetSettlement = std::weak_ptr<Settlement>();
		// Stop the unit
		this->Stop();
		return;
	}

	// Nothing else to do. Unit is trying to conquer the settlement and will try until the settlement is conquered, unit is died.
}

bool Unit::CheckFightConditions(const std::shared_ptr<Playable>& targetObj) const
{
	// Target must exist
	if (!targetObj)
		return false;
	// ASSERTION: Target must not be a decoration
	assert(targetObj->IsDecoration() == false);
	#if CENTURION_DEBUG_MODE
	// Temporary condition. Allows to attack also allies buildings. Just for test.
	if (targetObj->IsBuilding() == true && std::static_pointer_cast<Building>(targetObj)->GetHealth() != 0)
		return true;
	#endif
	// The target can't be the unit itself
	if (targetObj->GetUniqueID() == this->GetUniqueID())
		return false;
	// It's possible to attacks only enemies.
	if (this->GetPlayer() == targetObj->GetPlayer())
		return false;
	// Only ranged units can attack buildings and only if the building is not totally damaged (health == 0).
	if (targetObj->IsBuilding() == true && (this->IsRanged() == false || std::static_pointer_cast<Building>(targetObj)->GetHealth() == 0))
		return false;
	// Current unit must be alive.
	if (this->IsDead() == true)
		return false;
	// Target, if it's a unit, must be alive.
	if (targetObj->IsUnit() && std::static_pointer_cast<Unit>(targetObj)->IsDead() == true)
		return false;

	return true;  // Ok
}

void Unit::StartFight(void)
{
	// Check if fight is already possible.
	if (this->CheckFightConditions(this->GetMyTarget()->GetObject()) == false)
	{
		this->Stop();
		return;
	}

	// Preparation for attack
	this->ChangeAnim("engage");
	this->behavior.status = Status::E_FIGHT_PREPARATION;  // Unit::ApplyGameLogic will manage the transaction from "toAttack" animation to "Attack" animation when "toAttack" will be completed.

	/*
	//The target will fire back
	if (this->target.GetObject()->IsUnit() == true && this->target.GetObject()->AsUnit()->target.IsEmptyTarget() == true)
	{
		this->target.GetObject()->AsUnit()->SetCommandWithTarget("attack", this);
	}
	*/
}

void Unit::Fighting(void)
{
	assert(this->behavior.status == Status::E_FIGHTING);
	// Check if fight is already possible.
	if (this->CheckFightConditions(this->GetMyTarget()->GetObject()) == false)
	{
		this->Stop();
		return;
	}

	std::shared_ptr<Playable> enemy = this->GetMyTarget()->GetObject();
	assert(enemy);

	const float distanceFromTarget = this->DistTo(enemy);
	// The unit can attack a target only if it can see the target.
	if (distanceFromTarget > this->GetSight())
	{
		this->Stop();
		return;
	}

	// Look your enemy in the face 
	this->ChangeDirection((int)enemy->GetPositionX(), (int)enemy->GetPositionY());

	if (this->bAttackCycleCompleted == false)
		return;

	this->bAttackCycleCompleted = false;

	// If the enemy is moving, follow him to continue the fighting.
	if (distanceFromTarget > this->GetMyTarget()->GetMinDistanceRequiredFromTarget())
	{
		this->GoToAttack(enemy);
		return;
	}

	// Attacking
	if (this->IsRanged() == true)
		Parabola::CreateParabola(std::static_pointer_cast<Unit>(this->me.lock()), enemy, "arrow");
	else
		Unit::ComputeAndApplyDamage(this->GetAttackerDamageParamsCRef(), std::static_pointer_cast<Unit>(this->me.lock()), (*enemy));

}

void Unit::IncKills(const uint32_t newKills)
{
	this->kills += newKills;
}

void Unit::NotifyKill(void)
{
	// ASSERTION: I must not notify a kill if the unit is dead.
	assert(this->IsDead() == false);

	// A new kill
	this->IncKills(1);

	// Stop the unit.
	this->Stop();

	// Finally, execute "on_kill" method.
	this->ExecuteMethod("on_kill");
}

void Unit::MarkAsDead(void)
{
	assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match);

	// Delete every command from the queue without retrieve any eventual resources
	if (this->commandsQueue)
		this->commandsQueue->Reset(0);

	// Remove unit from all selections that includes it
	this->GlobalDeselect();

	// Leave hero army (if attached to some hero)
	this->Detach();

	// Leave garrison
	if (this->buildingInWhichIAm.expired() == false)
		this->buildingInWhichIAm.lock()->GetGarrison()->RemoveUnit(std::static_pointer_cast<Unit>(this->me.lock()));

	// If the unit die while it was conquering, notify to its target settlement that the units is conquering anymore
	if (this->behavior.targetSettlement.expired() == false)
	{
		this->behavior.targetSettlement.lock()->RemoveBesieger((*this));
		this->behavior.targetSettlement = std::weak_ptr<Settlement>();
	}

	// Unit can no longer defend the settlements he was close to
	while (this->friendlySettlementNear.empty() == false)
	{
		const uint32_t settlementID = (*this->friendlySettlementNear.begin());
		assert(GObject::playersWeakRef.expired() == false);
		GObject::playersWeakRef.lock()->GetSettlementsCollectionRef().GetSettlementByID(settlementID)->RemoveDefender((*this));
		this->friendlySettlementNear.erase(settlementID);
	}

	// Stop the unit.
	this->Stop();

	// After stopping, unit now is dead.
	this->behavior.status = Unit::Status::E_DYING;
	this->ChangeAnim("die");

	// Finally, execute "on_die" method.
	this->ExecuteMethod("on_die");
}

void Unit::FindChunkInWhichIAm(void)
{
	// TODO (see also destructor)
	return;

	if (Engine::GetInstance().GetEnvironmentId() != IEnvironment::Environments::e_match || this->IsDead() == true)
		return;

	auto terrain_ptr = this->terrain.lock();

	const std::shared_ptr<Point> pos = this->GetPosition();
	int chunkID = terrain_ptr->GetChunkIndexByMapCoords((int)pos->x, (int)pos->y);
	if (chunkID == this->chunkInWhichIAm)
		return;

	terrain_ptr->RemoveUnitFromChunk(this->chunkInWhichIAm, this, this->chunkInWhichIAmListIt);
	this->chunkInWhichIAm = chunkID;
	this->chunkInWhichIAmListIt = terrain_ptr->AddUnitToChunk(this->chunkInWhichIAm, this);
}
#pragma endregion
