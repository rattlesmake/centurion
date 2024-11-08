#include "selected_objects.h"
#include "objects_list.h"
#include <engine.h>
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/unit.h>

#include <bin_data_interpreter.h>

// Assets
#include <xml_class_enums.h>

// Services
#include <services/pyservice.h>


#pragma region Constructors and destructor:
SelectedObjects::SelectedObjects(const uint8_t _playerID) : 
	playerID(_playerID), environment(Engine::GetInstance().GetEnvironmentId())
{
}

SelectedObjects::~SelectedObjects(void)
{
	bool bReset = true;

	// If match is being loaded from match environment, do not reset selo family
	auto env = Engine::GetInstance().GetEnvironment();
	if (env && env->GetType() == IEnvironment::Environments::e_match && std::static_pointer_cast<Match>(env)->IsLoading() == true)
		bReset = false;

	if (bReset == true)
	{
		auto& py = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
		py.BindSharedPtr("selo", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
		py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selos", PyInterpreter::PyBindedSPTypes::ObjsList, nullptr);
		py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, nullptr);
	}
}
#pragma endregion


#pragma region Factory methods:
std::shared_ptr<SelectedObjects> SelectedObjects::Create(const uint8_t _playerID)
{
	std::shared_ptr<SelectedObjects> selected_objects_sp{ new SelectedObjects{_playerID} };
	selected_objects_sp->me = selected_objects_sp;
	return selected_objects_sp;
}
#pragma endregion


#pragma region To scripts methods:
uint64_t SelectedObjects::GetArmorPierce(void) const
{
	return (this->armorPierce / this->Count());
}

uint64_t SelectedObjects::GetArmorSlash(void) const
{
	return (this->armorSlash / this->Count());
}

uint64_t SelectedObjects::GetFood(void) const
{
	return this->food;
}

uint64_t SelectedObjects::GetHealth(void) const
{
	return this->health;
}

uint64_t SelectedObjects::GetLevel(void) const
{
	return (this->level / this->Count());
}

uint64_t SelectedObjects::GetMinAttack(void) const
{
	return (this->minAttack / this->Count());
}

uint64_t SelectedObjects::GetMaxAttack(void) const
{
	return (this->maxAttack / this->Count());
}

uint64_t SelectedObjects::GetMaxFood(void) const
{
	return this->maxFood;
}

uint64_t SelectedObjects::GetMaxStamina(void) const
{
	return static_cast<uint8_t>(MAX_STAMINA_VALUE) * static_cast<uint64_t>(this->Count());
}

uint64_t SelectedObjects::GetMaxHealth(void) const
{
	return this->maxHealth;
}

uint8_t SelectedObjects::GetPercHealth(void) const
{
	const auto max_health = this->GetMaxHealth();
	return (max_health > 0) ? static_cast<uint8_t>(static_cast<double>(this->GetHealth()) / max_health * 100.0f) : 0;
}

uint8_t SelectedObjects::GetPercStamina(void) const
{
	const auto max_stamina = this->GetMaxStamina();
	return (max_stamina > 0) ? static_cast<uint8_t>(static_cast<double>(this->GetStamina()) / max_stamina * 100.0f) : 0;
}

uint64_t SelectedObjects::GetRange(void) const
{
	return (this->range / this->Count());
}

uint64_t SelectedObjects::GetStamina(void) const
{
	return this->stamina;
}

uint64_t SelectedObjects::GetSight(void) const
{
	return (this->sight / this->Count());
}

uint64_t SelectedObjects::GetSpeed(void) const
{
	return (this->speed / this->Count());
}

bool SelectedObjects::IsRanged(void) const
{
	return (this->rangedUnitsCounter == this->Count());
}
#pragma endregion


void SelectedObjects::UpdateAttributes(const UnitAttributes::UnitValues& attr, const uint32_t _sight, const bool bIsRanged)
{
	this->armorPierce += attr.armorPierce;
	this->armorSlash += attr.armorSlash;
	this->food += attr.food;
	this->health += attr.health;
	this->level += attr.level;
	this->minAttack += attr.minAttack;
	this->maxAttack += attr.maxAttack;
	this->maxFood += attr.maxFood;
	this->maxHealth += attr.maxHealth;
	this->range += attr.range;
	this->stamina += attr.stamina;
	this->speed += attr.speed;

	this->sight += _sight;
	this->rangedUnitsCounter += bIsRanged;  // bIsRanged == true => += 1 else += 0
	// Damage type.
	if (this->damageType != DamageTypes::e_undefined && this->damageType != attr.damageType)
		this->damageType = DamageTypes::e_both;
	else
		this->damageType = attr.damageType;
}

void SelectedObjects::ResetAttributes(void)
{
	this->armorPierce = 0;
	this->armorSlash = 0;
	this->food = 0;
	this->health = 0;
	this->level = 0;
	this->minAttack = 0;
	this->maxAttack = 0;
	this->maxFood = 0;
	this->maxHealth = 0;
	this->range = 0;
	this->stamina = 0;
	this->speed = 0;

	this->sight = 0;
	this->rangedUnitsCounter = 0;
	this->damageType = DamageTypes::e_undefined;
}

void SelectedObjects::Reset(void)
{
	auto& objsCRef = this->troops->GetObjsCRef();
	while (objsCRef.empty() == false)
	{
		assert(objsCRef.front().expired() == false);
		objsCRef.front().lock()->Deselect(this->playerID);
		// GObject::Deselect will remove an element from objsCRef.
	}
	// I don't need to clear collections (e.g. maps, list, etc), since GObject::Deselect calls SelectedObjects::Remove and SelectedObjects::Remove updates collection.
}

void SelectedObjects::Add(const std::shared_ptr<GObject>& object)
{
	if (this->CheckIfResetIsNeeded(*object) == true)
		this->Reset();

	// Add to selection and update selo family bindings.
	this->troops->Insert(object);
	const uint32_t troopsNumber = this->Count();
	if (troopsNumber <= 1)  // Only a GObject is selected
	{
		// After addition, selection has only a GObject. So set it as selo.
		this->selectedObject = object;
		this->BindSelection(troopsNumber);
	}
	else if (this->selectedObject.expired() == false)  // If more than a GObject is selected and if before this addition only a GObject was selected.
	{
		// After addition, selection has more than a GObject. So reset selo.
		this->selectedObject = std::weak_ptr<GObject>();
		this->BindSelection(troopsNumber);
	}

	if (object->IsUnit() == true)  // Update info for topbar.
		this->Update(std::static_pointer_cast<Unit>(object), OrderedUnitsList::Operations::e_insert, std::nullopt);

	if (this->environment == IEnvironment::Environments::e_match)  // In editor we don't have commands, since they aren't useful.
	{
		//ASSERTION: In match, only PlayableGobjects can be selected
		assert(object->IsPlayableGObject() == true);

		/// Updating common commands
		// Updating all commands
		auto& objCommands = std::static_pointer_cast<Playable>(object)->GetCommandsVecCRef();
		for (auto const& cmd : objCommands)
		{
			const std::string& cmdID = cmd->GetIdCRef();
			if (this->ownersPerCommand.contains(cmdID) == true)
				this->ownersPerCommand.at(cmdID).first += 1;
			else
				this->ownersPerCommand.insert( { cmdID, { 1, cmd } } );
		}	

		// Chosing common commands
		this->SetCommonCommands();
	}

	// Update number of GObjects per player:
	const uint8_t playerID = (object->IsPlayableGObject() == true) ? std::static_pointer_cast<Playable>(object)->GetPlayer() : PlayersArray::UNDEFINED_PLAYER_INDEX;
	this->selectedObjsPerClass[playerID] += 1;
}

void SelectedObjects::Remove(const std::shared_ptr<GObject>& object)
{
	// Remove from selection and update selo family bindings.
	this->troops->GetOut(object);
	const uint32_t troopsNumber = this->Count();
	if (troopsNumber <= 1)
	{
		// After removal, if selection contains only a Gobject, then get it and set it as selo. Otherwise reset selo.
		this->selectedObject = (troopsNumber == 1) ? this->troops->Get(0) : std::weak_ptr<GObject>();
		this->BindSelection(troopsNumber);
	}

	if (object->IsUnit() == true) 	//Update info for topbar.
		this->Update(std::static_pointer_cast<Unit>(object), OrderedUnitsList::Operations::e_remove, std::nullopt);

	if (this->environment == IEnvironment::Environments::e_match)  // In editor we don't have commands, since they aren't useful.
	{
		// ASSERTION: In match, only PlayableGobjects can be selected
		assert(object->IsPlayableGObject() == true);

		/// Updating common commands
		// Updating all commands
		auto& objCommands = std::static_pointer_cast<Playable>(object)->GetCommandsVecCRef();
		for (auto const& cmd : objCommands)
		{
			const std::string& cmdID = cmd->GetIdCRef();
			assert(this->ownersPerCommand.contains(cmdID) == true);
			if (this->ownersPerCommand.at(cmdID).first >= 2)
				this->ownersPerCommand.at(cmdID).first -= 1;
			else  // ==1
				this->ownersPerCommand.erase(cmdID);
		}

		//Chosing common commands
		this->SetCommonCommands();
	}

	// Update number of GObjects per player:
	const uint8_t playerID = (object->IsPlayableGObject() == true) ? std::static_pointer_cast<Playable>(object)->GetPlayer() : PlayersArray::UNDEFINED_PLAYER_INDEX;
	this->selectedObjsPerClass[playerID] -= 1;
}

std::shared_ptr<GObject> SelectedObjects::Get(const uint32_t index) const
{
	return this->troops->Get(index);
}

void SelectedObjects::Filter(const std::string& className)
{
	// The complexity of this filtering functions is O(numbers of GObjects belonging to a class != className)

	// Get a reference to the classes map of the ObjsList inside the Selection
	auto& objsByClass = this->troops->GetObjsByClassMapCRef();

	/*
		This double nested loop can appear strange. However, you should keep in mind that GObject::Deselect 
		will be automatically update Selection and it's ObjsList.
		In other words, the reference objsByClass will change during the loop dynamically.
	*/
	auto classesIterator = objsByClass.cbegin();
	while (classesIterator != objsByClass.cend())
	{
		if (classesIterator->first != className)
		{
			auto& objsSet = classesIterator->second;
			const size_t objsToRemove = objsSet.size();
			size_t removedObjs = 0;
			while (removedObjs < objsToRemove)
			{
				objsSet.begin()->lock()->Deselect(this->playerID);
				removedObjs += 1;  // A GObjects was removed by the invokation of GObject::Deselect.
			}
			classesIterator = objsByClass.cbegin();
		}
		else
			classesIterator++;
	}
}

uint32_t SelectedObjects::Count(void) const
{
	return this->troops->Count();
}

std::shared_ptr<GObject> SelectedObjects::GetSelectedObject(void) const
{
	return (this->troops->Count() == 1) ? this->selectedObject.lock() : std::shared_ptr<GObject>();
}

uint32_t SelectedObjects::GetNumberOfCommands(void) const
{
	return this->nCommands;
}

std::shared_ptr<Command> SelectedObjects::GetCommand(const uint8_t index) const
{
	return (index < this->nCommands) ? this->commonCommandsVec.at(index) : Command::GetInvalidCommand();
}

DamageTypes SelectedObjects::GetDamageType(void) const
{
	return this->damageType;
}

bool SelectedObjects::ContainsOnlyItsOwnGObjects(void) const
{
	return (this->selectedObjsPerClass.at(this->playerID) == this->troops->Count());
}

bool SelectedObjects::ContainsOnlyUnits(void) const
{
	return (this->troops->CountByClass("unit") == this->troops->Count());
}

void SelectedObjects::UpdateObjsByPlayerCounter(const uint8_t playerIdToDecrease, const uint8_t playerIdToIncrement)
{
	this->selectedObjsPerClass.at(playerIdToDecrease) -= 1;
	this->selectedObjsPerClass.at(playerIdToIncrement) += 1;
}

#pragma region Inherited methods:
void SelectedObjects::GetBinRepresentation(std::vector<byte_t>& data) const
{
	// Firstly, save number of selected GObjects
	BinaryDataInterpreter::PushUInt32(data, this->Count());

	// Save selection
	auto selo = selectedObject.lock();
	if (selo && selo->IsBuilding() == true)
	{
		// Only a single building is selected. Save it here, since OrderedUnitsList is just for units and not for buildings
		assert(this->Count() == 1);
		BinaryDataInterpreter::PushUInt32(data, selo->GetUniqueID());
		BinaryDataInterpreter::PushDouble(data, selo->GetClickedTime());
	}
	else
		this->OrderedUnitsList::GetBinRepresentation(data);  // Call parent's method to save OrderUnitsList info
}

void SelectedObjects::InitByBinData(std::vector<byte_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap)
{
	//Load the number of selected GObjects
	const uint32_t nCount = BinaryDataInterpreter::ExtractUInt32(binData, offset);

	if (nCount >= 1 && this->playerID == PlayersArray::UNDEFINED_PLAYER_INDEX)
		throw BinaryDeserializerException("Inavlid selection! Found selection for undefined player");

	for (uint32_t n = 1; n <= nCount; n += 1)
	{
		// Load next next unique ID and egress time (we do not need egress time for selection, but we have saved it in order to be compatible with other OrderedUnitsList).
		const uniqueID_t ID = BinaryDataInterpreter::ExtractUInt32(binData, offset);
		const egressTimeInstant_t egressTime = BinaryDataInterpreter::ExtractDouble(binData, offset);

		// Try to get a GObject and add it to selection
		std::shared_ptr<GObject> obj;
		if (objsMap.contains(ID) == false || !(obj = objsMap.at(ID).lock()) || obj->IsDecoration() == true || (obj->IsBuilding() && nCount > 1))
			break;  // Invalid ID --> stop (it will trigger an exception as you can see)

		// Select GObject
		obj->Select(this->playerID);
	}


	if (this->Count() != nCount)
		throw BinaryDeserializerException("An error occurs while loading selection beloging to player " + std::to_string(playerID));
}
#pragma endregion


#pragma region Private members:
void SelectedObjects::BindSelection(const uint32_t numberOfSelectedObjects) const
{
	static PyInterpreter& py = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
	if (numberOfSelectedObjects == 1)  // Selection has only a GObject
	{
		// Bind selo family properly.
		auto object = this->selectedObject.lock();
		const assets::xmlClassTypeInt_t objectType = object->GetTypeInt();
		py.BindSharedPtr("selo", PyInterpreter::PyBindedSPTypes::GObject, object);
		py.BindSharedPtr("selos", PyInterpreter::PyBindedSPTypes::ObjsList, nullptr);
		py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, nullptr);

		switch (objectType)
		{
		case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_heroClass):
			py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
			py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, this->me.lock());
			break;
		case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass):
		case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_wagonClass):
			py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
			py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, this->me.lock());
			break;
		case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass):
			py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, object);
			py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, std::static_pointer_cast<Building>(object)->GetSettlement());
			py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			break;
		case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_decorationClass):
			// ASSERTION: Decorations must be selected only in editor.
			assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor);
			py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
			py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
			break;  // A decoration will be never binded
		default:
			// ASSERTION: If I'm here ==> error!
			assert(true == false);
			break;
		}
	}
	else if (numberOfSelectedObjects > 1)  // Selection has more than a GObject.
	{
		// Unbind selo family
		py.BindSharedPtr("selo", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
		py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		// Bind multiple selection
		py.BindSharedPtr("selos", PyInterpreter::PyBindedSPTypes::ObjsList, this->troops);
		py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, nullptr);
		if (Engine::GetInstance().GetEnvironment()->AsIGame()->IsCurrentPlayer(this->playerID) && this->ContainsOnlyUnits() == true)
		{
			assert(this->me.expired() == false);
			py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, this->me.lock());
		}
	}
	else  // Selection has 0 GObject (it was reset)
	{
		// Unbind everithing
		py.BindSharedPtr("selo", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, nullptr);
		py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, nullptr);
		py.BindSharedPtr("selos", PyInterpreter::PyBindedSPTypes::ObjsList, nullptr);
		py.BindSharedPtr("selectedUnits", PyInterpreter::PyBindedSPTypes::MultiSelection, nullptr);
	}
}

bool SelectedObjects::CheckIfResetIsNeeded(const GObject& objToInsert) const
{
	///Multiple selection can contain more than a GObject only it these GObjects are units.
	///In editor, multiple selection can contains even more than a decoration (decoration can be selected only in editor).

	const assets::xmlClassTypeInt_t objToInsertType = objToInsert.GetTypeInt();
	
	// I have always to reset the selection if the GObject I want to insert is a building.
	if (objToInsertType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass))
		return true;

	if (this->Count() >= 1)
	{
		const std::shared_ptr<GObject> firstEl = this->troops->Get(0);
		assert(firstEl);

		const assets::xmlClassTypeInt_t multiSelType = firstEl->GetTypeInt();
		if (multiSelType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass))
		{
			// I have to reset selection if the selection contains a building.
			assert(this->Count() == 1);
			return true;
		}
		if (multiSelType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_decorationClass) && objToInsertType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass))
		{
			// I have to reset selection if the selelection contains at least a decoration and I want to select a unit.
			// N.B.: This situation has sense only in editor!
			assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor);
			return true;
		}
		if (multiSelType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass) && objToInsertType == static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_decorationClass))
		{
			// I have to reset selection if the selelection contains at least a unit and I want to select a decoration.
			// N.B.: This situation has sense only in editor!
			assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor);
			return true;
		}
	}

	// Selection does not need to be reset.
	return false;
}

void SelectedObjects::SetCommonCommands(void)
{
	// Init to zero the number of commands of the selection.
	this->nCommands = 0;
	// Get the number of selected objects.
	const uint32_t objectsNumber = this->Count();

	// Computing common commands vector.
	for (auto const& cmd : this->ownersPerCommand)
	{
		// cmd.second,fist is the number of owners of cmd (cmd is a command ID).
		// cmd.second.second is the command.
		if (cmd.second.first == objectsNumber)
		{
			if (nCommands < this->commonCommandsVec.size())
				this->commonCommandsVec.at(this->nCommands) = cmd.second.second;
			else
				this->commonCommandsVec.push_back(cmd.second.second);
			this->nCommands++;  // A new common command.
		}
	}
	std::sort(commonCommandsVec.begin(), commonCommandsVec.begin() + this->nCommands, Command::CommandSPComparator());
}
#pragma endregion
