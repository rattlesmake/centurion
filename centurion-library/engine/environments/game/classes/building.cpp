#include "building.h"
#include "unit.h"
#include "hero.h"
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>

#include <bin_data_interpreter.h>
#include <keyboard.h>

#include <math_utils.h>

// Assets
#include <xml_entity.h>
#include <xml_class_method.h>
#include <xml_class.h>
#include <xml_script.h>
#include <xml_entity_structs.h>


#pragma region Constructors and destructor:
Building::Building(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource)
	: Playable(_player, std::move(_pos), _bTemporary, _objData, surface, dataSource)
{
	this->SetPosition(_pos.x, _pos.y);
}

Building::~Building(void)
{
}
#pragma endregion


#pragma region To scripts methods:
void Building::Damage(const uint32_t damage)
{
	if (damage >= this->health)
		this->SetHealth(0);
	else
		this->SetHealth(this->health - damage);
}

std::shared_ptr<Garrison> Building::GetGarrison(void) const
{
	return this->garrison;
}

std::shared_ptr<Settlement> Building::GetSettlement(void) const
{
	return this->settlement;
}

bool Building::HasGarrison(void) const
{
	return (this->garrison) ? true : false;
}

bool Building::IsBroken(void) const
{
	return this->bIsBroken;
}

bool Building::IsCentralBuilding(void) const
{
	return this->bIsCentralBuilding;
}

bool Building::IsFirstBuilding(void) const
{
	return this->bIsFirstBuilding;
}

void Building::Repair(void)
{
	if (this->bIsBroken == false)
		return;

	if (this->GetPercentHealth() <= 10)
	{
		this->SetPercentHealth(11);
		this->health = 11 * this->GetMaxHealth() / 100;
	}

	//The building isn't broken anymore.
	this->SetBrokeness(false);
}

bool Building::SetCommandWithTarget(const std::string& command, const std::shared_ptr<Target>& targetObj)
{
	/// WARNING: If you modify this method, give a look even to Unit::SetCommandWithTarget

	// Check if the target is valid and if the unit has the command
	if (!targetObj || this->commandsMapId.contains(command) == false)
		return false;  // Cannot execute command.

	// Get command, method and then script
	const std::shared_ptr<Command> cmdSP = this->commandsMapId.at(command);
	const auto& methodToExecute = this->methodsMap.at(cmdSP->GetMethodCRef());
	const std::shared_ptr<centurion::assets::xml_script> script = methodToExecute->get_associated_script();

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

std::shared_ptr<Point> Building::GetExitPoint(void) const
{
	try
	{
		auto& pts = xml_entity->get_points_by_type_cref("exit");
		return std::shared_ptr<Point>(new Point(this->GetPositionX() + pts[0].x, this->GetPositionY() + pts[0].y));
	}
	catch (std::runtime_error&)  // TODO exception - sostituire con tipo restituito da get_points_by_type_cref
	{
		return std::shared_ptr<Point>(new Point(this->GetPositionX(), this->GetPositionY()));
	}	
}

bool Building::CanTrainUnits(void) const
{
	return (this->trainingsCommandsNumber != 0);
}

bool Building::CanReseachTechnologies(void) const
{
	return (this->technologiesCommandsNumber != 0);
}

#pragma region Inherited methods:
void Building::SetHealth(uint32_t _health)
{
	// Building is already totally damaged
	if (this->health == 0)
		return;

	const uint32_t maxHealth = this->GetMaxHealth();
	if (_health > maxHealth)
		_health = maxHealth;

	// Update health.
	this->health = _health;
	this->SetPercentHealth(static_cast<uint8_t>((static_cast<float>(_health) / static_cast<float>(maxHealth) * 100.0)));

	if (this->bIsBroken == false && this->GetPercentHealth() <= 10)
	{
		//The building is extremely damaged (i.e. it's broken).
		this->SetBrokeness(true);

		//Delete every command from the queue retrieving only the 50% of the resources eventually paid when the command was inserted in the queue.
		this->commandsQueue->Reset(50);
	}
}

bool Building::SetPlayer(const uint8_t _playerID)
{
	// _playerID parameter is checked by Settlement::SetPlayer
	return this->settlement->SetPlayer(_playerID);
}

void Building::CheckPlaceability(const bool bAlwaysPlaceable)
{
	std::weak_ptr<SurfaceGrid> surfaceGrid = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSurface()->GetSurfaceGrid();

	this->bIsPlaceable = true;
	if (bAlwaysPlaceable == true)
		return;

	if (rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(340) || rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(344)) // left&right shift 
		return;

	PlacementErrors placementError = GObject::PlacementErrors::e_no_error;
	this->bIsPlaceable = surfaceGrid.lock()->CheckObjectGridAvailability(this->grid, (*this->GetPosition()));
	if (this->bIsPlaceable == true)
	{
		const std::pair<bool, uint8_t> result = BuildingSettlementBridge::IsBuildingCloseToFriendlySettlement(std::static_pointer_cast<Building>(this->me.lock()), GObject::playersWeakRef.lock());
		this->bIsPlaceable = result.first;
		if (this->bIsPlaceable == true)
		{
			if (this->GetSettlement() && this->GetSettlement()->GetNumberOfBuildings() >= 2 && this->CheckIfSettlementChainIsNotBroken() == false)
			{
				this->bIsPlaceable = false;
				placementError = GObject::PlacementErrors::e_settlement_chain_broken;
			}
		}
		else
			placementError = static_cast<PlacementErrors>(result.second);
	}
	else
		placementError = GObject::PlacementErrors::e_impassable_point;

	this->SetPlacementError(placementError);
	this->SendInfoText();
}

void Building::ApplyGameLogics(void)
{
	if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match)
	{
		this->commandsQueue->ExecuteReadyCommands();

		if (this->bIsFirstBuilding == true)
		{
			this->settlement->HandleLoyalty();
		}
	}
}

void Building::SetDisplayedName(std::string _displayedName)
{
	Playable::SetDisplayedName(_displayedName);
	// TODO
	// if (this->IsCentralBuilding()) this->displayedNameText.SetText(this->displayedName);

}

bool Building::SetPosition(const int x, const int y)
{
	const bool bValidPos = GObject::SetPosition(x, y);
	if (bValidPos && this->IsCentralBuilding() == true)
	{
		//float x = this->position.x / MEDIUM_MAP_WIDTH * myWindow::GetInstance().GetWidth();
		//float y = rattlesmake::peripherals::camera::get_instance().GetYMinimapCoordinate(this->position.y / MEDIUM_MAP_HEIGHT * myWindow::GetInstance().GetHeight()) + 15;
		//this->displayedNameText.SetPosition(ImVec2(x, y + 5));
	}
	return bValidPos;
}

void Building::AddCommandToExecute(const std::string& commandID)
{
	if (this->bIsBroken == true && commandID != "building_repair")
		return;
	this->Playable::AddCommandToExecute(commandID);
}

void Building::GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const
{
	// Firstly, calls always GObject::GetBinSignature
	if (calledByChild == false)
		this->GObject::GetBinSignature(data);

	// If you change saving order here, please go to BuildingAttributes::SetAttributes and impose the same loading order!
	this->BuildingAttributes::GetXmlAttributesAsBinaryData(data);

	// If this building has a NOT empty garrison (of course it must have a garrison as necessary condition), make aware PlayersArray about: In any case, save this info.
	const bool bHasNotEmptyGarrison = (this->garrison && this->garrison->GetNumberOfUnits() >= 1);
	if (bHasNotEmptyGarrison == true)
		GObject::playersWeakRef.lock()->AddBuildingHavingNotEmptyGarrison((*this));
	BinaryDataInterpreter::PushBoolean(data, bHasNotEmptyGarrison);

	//Then calls always method Playable::GetBinRepresentation
	if (calledByChild == false)
		this->Playable::GetBinRepresentation(data, calledByChild);

	// Finally save ALWAYS settlement ID, since ObjsCollection::CreateGObject___ as first thing creates a new GObject and only after that, if the GObject is a building, a settlement in assigned
	BinaryDataInterpreter::PushUInt32(data, this->settlement->GetId());
	// After that, if this building is the first building of a settlement, save also settlement info
	if (this->bIsFirstBuilding == true)
		this->settlement->GetBinRepresentation(data);
}
#pragma endregion


void Building::Heal(const uint32_t healthPoints)
{
	this->SetHealth(this->health + healthPoints);
}

bool Building::IsVillageHall(void) const
{
	return this->bIsVillageHall;
}

bool Building::IsTownhall(void) const
{
	return this->bIsTownhall;
}

bool Building::IsOutpost(void) const
{
	return this->bIsOutpost;
}

bool Building::IsShipyard(void) const
{
	return this->bIsShipyard;
}

void Building::SetEntPath(std::string par_ent_path)
{
	this->ent_path = std::move(par_ent_path);
}

void Building::SetSettlement(const std::shared_ptr<Settlement>& sett, const bool _bIsFirstBuilding)
{
	// Assertion: sett MUST be valid.
	assert(sett);
	// Assertion: the current building doesn't already have a settlement.
	assert(!this->settlement);
	// Assertion: check numbers of buildings:
	if (_bIsFirstBuilding == true)
		assert(sett->GetNumberOfBuildings() == 0);
	else
		assert(sett->GetNumberOfBuildings() >= 1);

	this->bIsFirstBuilding = _bIsFirstBuilding;
	this->settlement = sett;
}

std::list<std::shared_ptr<Building>> Building::GetNeighbours(void) const
{
	std::list<std::shared_ptr<Building>> neighbours;
	const std::shared_ptr<Point> pos = this->GetPosition();
	std::shared_ptr<ObjsList> buildings = this->GetSettlement()->GetBuildings();
	ObjsList::objs_const_iterator endIt = buildings->cend();
	for (auto buildingIt = buildings->cbegin(); buildingIt != endIt; buildingIt++)
	{
		assert((*buildingIt).expired() == false);
		const std::shared_ptr<Building> bSP = std::static_pointer_cast<Building>((*buildingIt).lock());
		if (bSP->GetUniqueID() == this->GetUniqueID())
			continue;  // Do not check nearness with the building itself.
		const bool bIsNeighbour = bSP->IsNeighbour(pos->x, pos->y);
		if (bIsNeighbour)
			neighbours.push_back(bSP);
	}
	return neighbours;
}

bool Building::IsNeighbour(const uint32_t x, const uint32_t y) const
{
	const std::shared_ptr<Point> pos = this->GetPosition();
	const uint32_t dist = static_cast<uint32_t>(Math::euclidean_distance(pos->x, pos->y, x, y));
	return (dist <= MAX_DISTANCE_BETWEEN_TWO_BUILDINGS); 
}

bool Building::CheckIfSettlementChainIsNotBroken(const bool thisIsBeingErased) const
{	
	uint32_t nBuildings = this->GetSettlement()->GetNumberOfBuildings();
	// Assertion: a settlement MUST have at least a building.
	assert(nBuildings >= 1);

	// If I want to remove the FIRST building and if it's the ONLY building of the settlement, a deletion can never broke the settlement chain.
	if (this->bIsFirstBuilding == true && nBuildings == 1)
		return true;

	std::set<std::shared_ptr<Building>> connectedBuildings;
	connectedBuildings.insert(this->GetSettlement()->GetFirstBuilding());

	if (thisIsBeingErased == true)
	{
		nBuildings--;
		assert(this->me.expired() == false);
		auto meSP = std::dynamic_pointer_cast<Building>(this->me.lock());
		this->AddNeighboursToSet(connectedBuildings, this->GetSettlement()->GetFirstBuilding(), meSP);
	}
	else
	{
		this->AddNeighboursToSet(connectedBuildings, this->GetSettlement()->GetFirstBuilding(), nullptr);
	}

	return (connectedBuildings.size() == nBuildings);
}

void Building::AddNeighboursToSet(std::set<std::shared_ptr<Building>>& setRef, const std::shared_ptr<Building>& firstBuilding, const std::shared_ptr<Building>& buildingToBeErased) const
{
	std::list<std::shared_ptr<Building>> listOfNeighbours = firstBuilding->GetNeighbours();
	while (listOfNeighbours.empty() == false)
	{
		std::shared_ptr<Building> b = std::move(listOfNeighbours.front());
		listOfNeighbours.pop_front();
		if (buildingToBeErased && b->GetUniqueID() == buildingToBeErased->GetUniqueID())
			continue;

		const bool is_in = setRef.contains(b);
		if (is_in == false)
		{
			setRef.insert(b);
			b->AddNeighboursToSet(setRef, b, buildingToBeErased);
		}
	}
}

#pragma region Protected members:
#pragma region Inherited methods:
std::ostream& Building::Serialize(std::ostream& out, const bool calledByChild) const
{
	const std::string tabs(GObject::nTabs, '\t');

	if (calledByChild == false)
		out << tabs << "<building class=\"" << this->GetClassName() << "\" "
			<< "id=\"" << std::to_string(this->GetUniqueID()) << "\" "
			<< "settlementID=\"" << std::to_string(this->settlement->GetId()) << "\" " 
			<< "xPos=\"" << std::to_string(this->GetPositionX()) << "\" "
			<< "yPos=\"" << std::to_string(this->GetPositionY()) << "\""
			<< ">\n";

	this->GObject::Serialize(out, true);
	this->Playable::Serialize(out, true);
	if (this->bIsFirstBuilding)
		out << (*this->settlement) << '\n';
	this->BuildingAttributes::Serialize(out, tabs);

	if (calledByChild == false)
		out << tabs << "</building>";

	return out;
}

void Building::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	this->BuildingAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);

	const auto& attributes_map = objData->get_attributes();

	// Get if the building is a central building.
	// Important to read even if the building is temporary in order to can place a building in the editor.
	this->bIsCentralBuilding = assets::xml_class::try_parse_bool(attributes_map, "isCentralBuilding").value();

	if (this->bIsTemporary == false)
	{
		// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
		std::optional<std::string> entPathOpt = assets::xml_class::try_parse_string(attributes_map, "ent_path");
		this->ent_path = std::move(entPathOpt.value());
		std::optional<int> maxUnitsAttribute = 0;
		maxUnitsAttribute = assets::xml_class::try_parse_integer(attributes_map, "maxUnits");
		if (maxUnitsAttribute.value() >= 1)
			this->garrison = Garrison::CreateGarrison(std::static_pointer_cast<Building>(this->me.lock()), maxUnitsAttribute.value());
		if (dataSource != nullptr && std::holds_alternative<gobjBinData_t>(*dataSource) == true)
		{
			// Read if building saved into binary file had a not empty garrison
			gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
			const bool bHasNotEmptyGarrison = BinaryDataInterpreter::ExtractBoolean((*binData.first), binData.second);
			if (bHasNotEmptyGarrison == true)
			{
				if (!this->garrison)  // Check that saved data is compatible with read data (i.e. if the building had a not empty garrison it must have a garrison)
					throw BinaryDeserializerException("Incompatible garrison info for building having ID " + std::to_string(this->GetUniqueID()));
				this->GObject::playersWeakRef.lock()->AddBuildingHavingNotEmptyGarrison((*this));
			}
		}

		if (this->IsCentralBuilding() == true)  // TODO pensare se deve essere central o first.
		{
			//TODO - che cazzo e'?
			//this->displayedNameText = gui::Text();
			//float x = this->position.x / MEDIUM_MAP_WIDTH * myWindow::GetInstance().GetWidth();
			//float y = rattlesmake::peripherals::camera::get_instance().GetYMinimapCoordinate(this->position.y / MEDIUM_MAP_HEIGHT * myWindow::GetInstance().GetHeight()) + 15;
			//this->displayedNameText.Create(this->displayedName, "tahoma_15", ImVec2(x, y + 5), ImVec4(1, 1, 1, 1), "center", "middle");
		}
	}

	// As a last thing, calls Playable::SetAttrs
	this->Playable::SetAttrs(objData, dataSource);
}

void Building::SetObjectCommands(const classData_t& objData)
{
	const auto& cmdsVec = objData->get_commands_list_cref();

	for (uint16_t i = 0; i < cmdsVec.size(); i++)
	{
		// ASSERTION: A command ID must be unique: building must not have repeated commands.
		assert(this->commandsMapId.contains(cmdsVec[i]->GetIdCRef()) == false);

		// Create a new command.
		std::shared_ptr<Command> cmd = Command::CreateCommand(*cmdsVec.at(i), false);

		// In order to let the current building know that the commands are particular
		const assets::xml_command_type type = cmd->GetType();
		if (type == assets::xml_command_type::Training)
			this->trainingsCommandsNumber += 1;
		else if (type == assets::xml_command_type::Technology)
			this->technologiesCommandsNumber += 1;

		// Save the command
		this->commandsMapId.insert({ cmd->GetIdCRef(), cmd });
		this->commandsVec.push_back(std::move(cmd));
	}

	// Assertion: This command must be always present. +
	// TODO except - mettere apposita eccezione
	assert(this->commandsMapId.contains("building_repair") == true);

	if (this->bIsBroken == false)
		this->commandsMapId.at("building_repair")->SetEnableness(false);  // building_repair is initially disabled.
}
#pragma endregion
#pragma endregion


#pragma region Private members:
void Building::SetBrokeness(const bool _bIsBroken)
{
	assert(this->bIsBroken != _bIsBroken);

	this->bIsBroken = _bIsBroken;

	// If the building is broken, all the commands will be disabled except "building_repair" that will be enabled.
	// Otherwise, if the building has been repaired (isn't broken anymore) all the commands will be enabled except "building_repair" thats will be disabled.
	// TODO - Quando sara' possibile disabilitare/abilitare i comandi da script (introduzione dei metodi CmdEnable e CmdDisable), i metodi che erano stati disattivati non devono essere abilitati quando la struttura è riparata.
	for (auto& cmd : this->commandsVec)
	{
		cmd->SetEnableness(!_bIsBroken);
	}
	this->commandsMapId.at("building_repair")->SetEnableness(_bIsBroken);  // building_repair is now enabled.
}
#pragma endregion
