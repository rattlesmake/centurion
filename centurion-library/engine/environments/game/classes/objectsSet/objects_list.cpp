#include "objects_list.h"
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/building.h>
#include <encode_utils.h>


// Assets 
#include <xml_class_enums.h>


#pragma region Static attributes initialization
std::weak_ptr<PlayersArray> ObjsList::playersArrayWP;
#pragma endregion


#pragma region Constructors:
ObjsList::ObjsList(const bool _bScriptsCanEdit) : bScriptsCanEdit(_bScriptsCanEdit)
{
}
#pragma endregion


#pragma region Operators:
#pragma endregion


#pragma region Iterators:
ObjsList::objs_iterator ObjsList::begin(void) noexcept
{
	return this->objs.begin();
}

ObjsList::objs_const_iterator ObjsList::cbegin(void) const noexcept
{
	return this->objs.cbegin();
}

ObjsList::objs_iterator ObjsList::end(void) noexcept
{
	return this->objs.end();
}

ObjsList::objs_const_iterator ObjsList::cend(void) const noexcept
{
	return this->objs.cend();
}
#pragma endregion


#pragma region Static members:
void ObjsList::SetPlayersArrayWeakRef(const std::weak_ptr<PlayersArray>& weakRef)
{
	ObjsList::playersArrayWP = weakRef;
}
#pragma endregion


#pragma region Functions that MUST BE used only by external scripts (CPP CODE MUST NOT USE THEM!!!)
bool ObjsList::Add(const std::shared_ptr<GObject>& object)
{
	bool bAdded = false;
	if (this->bScriptsCanEdit == true && this->Contains(object) == false)
	{
		this->Insert(object);
		bAdded = true;
	}
	return bAdded;
}

bool ObjsList::Remove(const std::shared_ptr<GObject>& object)
{
	bool bRemoved = false;
	if (this->bScriptsCanEdit == true && this->Contains(object) == true)
	{
		this->GetOut(object);
		bRemoved = true;
	}
	return bRemoved;
}

bool ObjsList::Clear(void)
{
	if (this->bScriptsCanEdit == true)
	{
		this->ClearAll();
		return true;
	}
	return false;
}
#pragma endregion


#pragma region Function used by external scripts (even CPP code can use them)
bool ObjsList::Contains(const std::shared_ptr<GObject>& object) const
{
	return this->iteratorsMap.contains(object->GetUniqueID());
}

uint32_t ObjsList::Count(void) const
{
	return static_cast<uint32_t>(this->objs.size());
}

uint32_t ObjsList::CountByClass(std::string className) const
{
	Encode::ToLowerCase(className);
	if (className == "unit")
		return this->units_number;
	if (className == "hero")
		return this->heroes_number;
	if (className == "druid")
		return this->druids_numbers;
	if (className == "wagon")
		return this->wagons_number;
	if (className == "decoration")
		return this->decorations_number;
	if (className == "building")
		return this->buildings_number;
	if (this->objsByClass.contains(className) == true)
		return static_cast<uint32_t>(this->objsByClass.at(className).size());
	return 0;
}

std::shared_ptr<GObject> ObjsList::Get(const uint32_t index) const
{
	if (this->objs.size() <= index)
		return std::shared_ptr<GObject>();

	auto it = this->objs.begin();
	std::advance(it, index); //It's O(n) !!!!
	assert((*it).expired() == false);
	return (*it).lock();
}

uint32_t ObjsList::GetNumberOfDifferentClasses(void) const
{
	return static_cast<uint32_t>(this->objsByClass.size());
}

bool ObjsList::IsEmpty(void) const
{
	return this->objs.empty();
}

void ObjsList::Select(const uint8_t playerID)
{
	assert(ObjsList::playersArrayWP.expired() == false);
	{
		auto players = ObjsList::playersArrayWP.lock();
		if (playerID >= players->GetArraySize())
			return;

		auto selection = (*players)[playerID]->GetSelection();
		if (selection->GetTroops().get() == this)  // If this ObjsList is the ObjsList inside the Selection then
			return;  // I don't need to select again all the selection, since obviously it's already selected :) 
		// Else, reset selection before select other GObjects.
		selection->Reset();
	}

	for (auto const& gobj : this->objs)
	{
		auto gobjSP = gobj.lock();
		assert(gobjSP);
		gobjSP->Select(playerID);
	}
}

void ObjsList::SelectByClass(const uint8_t playerID, const std::string& className)
{
	assert(ObjsList::playersArrayWP.expired() == false);

	if (this->objsByClass.contains(className) == false)
		return;

	{
		auto players = ObjsList::playersArrayWP.lock();
		if (playerID >= players->GetArraySize())
			return;

		auto selection = (*players)[playerID]->GetSelection();
		if (selection->GetTroops().get() == this)  // If this ObjsList is the ObjsList inside the Selection then
		{
			// Filter by class.
			selection->Filter(className);
			return; 
		}
		// Else, reset selection before select other GObjects.
		selection->Reset();
	}

	auto& setOfObjs = this->objsByClass.at(className);
	for (auto const& gobj : setOfObjs)
	{
		auto gobjSP = gobj.lock();
		assert(gobjSP);
		gobjSP->Select(playerID);
	}
}

void ObjsList::SelectHero(const uint8_t playerID, const std::shared_ptr<Hero>& hero)
{
	assert(ObjsList::playersArrayWP.expired() == false);

	const uniqueID_t heroID = hero->GetUniqueID();
	if (this->iteratorsMap.contains(heroID) == false)
		return;
	{
		auto players = ObjsList::playersArrayWP.lock();
		if (playerID >= players->GetArraySize())
			return;
		(*players)[playerID]->GetSelection()->Reset();
	}
	hero->Select(playerID);
}

void ObjsList::SetHealthByPercHealth(uint8_t perc_health)
{
	if (perc_health > 100)
		perc_health = 100;

	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		// Change health only for Units or buildings and skip others
		if (objSP->IsUnit() == true)
			std::static_pointer_cast<Unit>(objSP)->SetHealthByPercHealth(perc_health);
		else if (objSP->IsBuilding() == true)
			std::static_pointer_cast<Building>(objSP)->SetHealth(std::static_pointer_cast<Building>(objSP)->GetMaxHealth() * perc_health / 100);
	}
}

void ObjsList::SetLevel(const uint16_t level)
{
	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		if (objSP->IsUnit() == true)  // Change level only for Units and skip others
			std::static_pointer_cast<Unit>(objSP)->SetLevel(level);
	}
}

void ObjsList::SetPlayer(const uint8_t playerID)
{
	if (playerID == PlayersArray::UNDEFINED_PLAYER_INDEX || playerID > PlayersArray::NUMBER_OF_PLAYERS)
		return;
	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		if (objSP->IsPlayableGObject() == true)  // Change player only for PlayableGObjects and skip others
			std::static_pointer_cast<Playable>(objSP)->SetPlayer(playerID);
	}
}

void ObjsList::SetStaminaByPercStamina(uint8_t perc_stamina)
{
	if (perc_stamina > 100)
		perc_stamina = 100;

	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		if (objSP->IsUnit() == true)  // Change level only for Units and skip others
			std::static_pointer_cast<Unit>(objSP)->SetStamina(std::static_pointer_cast<Unit>(objSP)->GetMaxStamina() * perc_stamina / 100);
	}
}

uint32_t ObjsList::SetCommandWithTarget(const std::string& commandID, const std::shared_ptr<Target>& targetObj)
{
	if (!targetObj || commandID.empty() == true)
		return 0;  // Invalid params, so none can execute the command

	// This counter counts the GObjects inside the ObjsList who were able to execute the command 
	uint32_t counter = 0;
	// Try to execute the command for each PlayableGObject inside the ObjsList
	for (auto& obj_it : this->objs)
	{
		std::shared_ptr<GObject> obj = obj_it.lock();
		if (obj && obj->IsPlayableGObject() == true)
			counter += std::static_pointer_cast<Playable>(obj)->SetCommandWithTarget(commandID, targetObj);  // GObject::SetCommandWithTarget returns true or false, so we can have or counter += 1 or counter += 0
	}
	return counter;
}

#pragma endregion

#pragma region Functions that MUST BE used only by CPP code (EXTERNAL SCRIPTS MUST NOT USE THEM!!!)
const std::list<std::weak_ptr<GObject>>& ObjsList::GetObjsCRef(void) const
{
	return this->objs;
}

void ObjsList::ClearAll(void)
{
	while (this->objs.empty() == false)
	{
		auto obj = this->objs.front().lock();
		assert(obj);
		this->GetOut(obj);
	}
	assert(this->objs.empty() == true && this->iteratorsMap.empty() == true);
}

void ObjsList::Insert(const std::shared_ptr<GObject>& object)
{
	const uniqueID_t ID = object->GetUniqueID();
	assert(this->iteratorsMap.contains(ID) == false);

	// Add the object.
	this->objs.push_back(object);

	// Store a pointer to the list position in which there is the inserted object.
	iteratorToObj_t it = this->objs.end();
	it--;
	this->iteratorsMap[ID] = it;

	// Updating number of objs per class
	const string& className = object->GetClassNameCRef();
	auto& set = this->objsByClass[className];
	set.insert(object);
	const assets::xmlClassTypeInt_t obj_type = object->GetTypeInt();
	switch (obj_type)
	{
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_heroClass):
		this->heroes_number += 1;
		this->units_number += 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_druidClass):
		this->druids_numbers += 1;
		this->units_number += 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_wagonClass):
		this->wagons_number += 1;
		this->units_number += 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass):
		this->units_number += 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass):
		this->buildings_number += 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_decorationClass):
		this->decorations_number += 1;
		break;
	default:
		break;
	}
}

void ObjsList::GetOut(const std::shared_ptr<GObject>& object)
{
	const uniqueID_t ID = object->GetUniqueID();
	assert(this->iteratorsMap.contains(ID) == true);

	// Get a pointer to the list position in which there is the object to removed then remove it from the ObjsList.
	iteratorToObj_t it = this->iteratorsMap.at(ID);
	this->iteratorsMap.erase(ID);
	this->objs.erase(it);

	// Updating number of objs per class
	const string& className = object->GetClassNameCRef();
	assert(this->objsByClass.contains(className) == true);
	auto& el = objsByClass.at(className);
	el.erase(object);
	if (el.empty() == true)
		this->objsByClass.erase(className);
	const assets::xmlClassTypeInt_t obj_type = object->GetTypeInt();
	switch (obj_type)
	{
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_heroClass):
		this->heroes_number -= 1;
		this->units_number -= 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_druidClass):
		this->druids_numbers -= 1;
		this->units_number -= 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_wagonClass):
		this->wagons_number -= 1;
		this->units_number -= 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass):
		this->units_number -= 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass):
		this->buildings_number -= 1;
		break;
	case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_decorationClass):
		this->decorations_number -= 1;
		break;
	default:
		break;
	}
}

void ObjsList::ExecuteCommand(const std::string& commandID, const bool bTestOnlyCondition)
{
	assert(commandID.empty() == false);

	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		if (objSP->IsPlayableGObject() == true)
		{
			if (bTestOnlyCondition == false)
				std::static_pointer_cast<Playable>(objSP)->AddCommandToExecute(commandID);
			else
				std::static_pointer_cast<Playable>(objSP)->CheckCommandCondition(commandID);
		}
	}
}

void ObjsList::ExecuteMethod(const std::string& methodID)
{
	assert(methodID.empty() == false);

	for (auto const& obj : this->objs)
	{
		assert(obj.expired() == false);
		std::shared_ptr<GObject> objSP = obj.lock();
		if (objSP->IsPlayableGObject() == true)
			std::static_pointer_cast<Playable>(objSP)->ExecuteMethod(methodID);
	}
}

const std::unordered_map<std::string, gobjsSPUSet_t>& ObjsList::GetObjsByClassMapCRef(void) const
{
	return this->objsByClass;
}
#pragma endregion
