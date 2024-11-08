#include "army.h"

#include <environments/game/classes/building.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/objectsSet/objects_list.h>

#include <bin_data_interpreter.h>

#pragma region Costructors:
Army::Army(const std::weak_ptr<Hero>& _hero, const uint16_t _maxNumberOfTroops) :
	hero(_hero), maxNumberOfTroops(_maxNumberOfTroops)
{
	troopsInTheSameBuilding.units = std::shared_ptr<ObjsList>(new ObjsList(false));
}
#pragma endregion


#pragma region Static members:
std::shared_ptr<Army> Army::CreateArmy(const std::shared_ptr<Hero>& _hero, uint32_t _maxNumberOfTroops)
{
	assert(_hero);
	if (_maxNumberOfTroops > MAX_MAXARMY_VALUE)
		_maxNumberOfTroops = MAX_MAXARMY_VALUE;
	return std::shared_ptr<Army>(new Army(_hero, _maxNumberOfTroops));
}
#pragma endregion


#pragma region To scripts members:
uint32_t Army::GetNumberOfUnits(void) const
{
	return this->troops->Count();
}

uint32_t Army::GetMaxNumberOfUnits(void) const
{
	return this->maxNumberOfTroops;
}

std::shared_ptr<ObjsList> Army::GetTroopsInTheSameBuilding(void) const
{
	return this->troopsInTheSameBuilding.units;
}

bool Army::IsFull(void) const
{
	return (this->troops->Count() == this->maxNumberOfTroops);
}

float Army::GetPercHealthUnitsInTheSameGarrison(const std::string& className) const
{
	return OrderedUnitsList::GetPercHealth(this->troopsInTheSameBuilding, className);
}
#pragma endregion


#pragma region Inherited methods:
void Army::HandleHealthChangement(const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new)
{
	OrderedUnitsList::HandleHealthChangement(_unitClassName, _unitID, _old, _new);
	OrderedUnitsList::HandleHealthChangement(this->troopsInTheSameBuilding.healthPerClass, this->troopsInTheSameBuilding.totHealthPerc, _unitClassName, _unitID, _old, _new);
}

void Army::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	//Firstly, save the ID of the hero owning this army
	//N.B.: This info will be the load by PlayersArray::DeserializeFromBinFile, so Army::InitByBinData must not consider it.
	assert(this->hero.expired() == false);
	BinaryDataInterpreter::PushUInt32(data, this->hero.lock()->GetUniqueID());

	//Secondly, save number of units inside the army
	BinaryDataInterpreter::PushUInt32(data, this->GetNumberOfUnits());

	//Finally, call parent's method to save OrderUnitsList info
	this->OrderedUnitsList::GetBinRepresentation(data);
}

void Army::InitByBinData(std::vector<uint8_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap)
{
	//Load the number of units attached to the hero
	const uint32_t nTroops = BinaryDataInterpreter::ExtractUInt32(binData, offset);

	if (this->maxNumberOfTroops > nTroops)
	{
		bool bValidAdd = true;
		for (uint32_t n = 1; n <= nTroops && bValidAdd == true; n += 1)
		{
			//Load next next unique ID and egress time.
			const uniqueID_t ID = BinaryDataInterpreter::ExtractUInt32(binData, offset);
			const egressTimeInstant_t egressTime = BinaryDataInterpreter::ExtractDouble(binData, offset);

			//Try to get a unit to add to this garrison
			std::shared_ptr<GObject> obj;
			if (objsMap.contains(ID) == false || !(obj = objsMap.at(ID).lock()) || obj->IsUnit() == false)
				break; //Invalid ID --> stop (it will trigger an exception as you can see)

			//Assign unit
			bValidAdd = this->AssignUnit(std::static_pointer_cast<Unit>(obj), egressTime);
		}
	}

	if (this->GetNumberOfUnits() != nTroops)
		throw BinaryDeserializerException("Invalid army for hero having ID " + std::to_string(this->hero.lock()->GetUniqueID()));
}
#pragma endregion


#pragma region Public members:
bool Army::AssignUnit(const std::shared_ptr<Unit>& unit, std::optional<egressTimeInstant_t>&& egressTime)
{
	//These checks are usefull even if this function cannot directly be called by an external script.
	//In fact, this function can indirectly be called by other API functions (e.g. Unit::GoToApproach).
	if (!unit)
		return false;
	if (this->IsFull() == true)
		return false;
	//Check if units is NOT assigned to the hero
	if (this->troops->Contains(unit) == true)
		return false;

	//If the unit is already assigned to another hero, detach it before assigning it to the current hero.
	std::shared_ptr<Hero> oldHero = unit->GetHero();
	if (oldHero)
		oldHero->GetArmy()->DetachUnit(unit);

	//Insertion in the hero army
	this->troops->Insert(unit);
	{
		//Check if the unit have to be added from the units in the same building of the hero.
		auto heroSp = this->hero.lock();
		if (unit->IsInHolder() == true && heroSp->IsInHolder() == true)
		{
			auto heroBuilding = heroSp->GetHostBuilding();
			if (this->troopsInTheSameBuilding.units->Contains(unit) == false && unit->GetHostBuilding()->GetUniqueID() == heroBuilding->GetUniqueID())
			{
				//Unit is already in the building but now it has an hero. 
				//So, add it to the hero's troops in the same building and update the corrisponding health perc map..
				this->troopsInTheSameBuilding.units->Insert(unit);
				this->troopsInTheSameBuilding.healthPerClass.UpdateHealthMap(this->troopsInTheSameBuilding.totHealthPerc, unit, OrderedUnitsList::Operations::e_insert);
				//And remove it from the garrison (the unit was already in the building, so I don't need to update counter because it was previously counted).
				heroBuilding->GetGarrison()->RemoveUnit(unit, false);
			}
		}
	}

	//Add unit to the formation priority map:
	{
		const uint8_t formationPriority = unit->GetFormationPriority();
		if (this->formPriority.contains(formationPriority) == true)
		{
			assert(this->formPriority.at(formationPriority).contains(unit) == false);
			this->formPriority.at(formationPriority).insert(unit);
		}
		else
			this->formPriority.insert({ formationPriority, gobjsSPUSet_t{ unit } });
	}

	//Set the current hero to the unit, so the unit will known its leader.
	unit->SetHero(this->hero);

	//TODO - hero and it's army must have the same velocity (Temporary solution).
	unit->SetSpeed(this->hero.lock()->GetSpeed());

	//Update info for topbar.
	this->Update(unit, OrderedUnitsList::Operations::e_insert, std::move(egressTime));

	return true;
}

bool Army::DetachUnit(const std::shared_ptr<Unit>& unit)
{
	//Check if units is assigned to the hero
	if (this->troops->Contains(unit) == false)
		return false;

	//Get a reference to the hero before setting a weak reference.
	std::shared_ptr<Hero> heroSp = this->hero.lock();
	//Unit now doesn't have an hero.
	unit->SetHero(std::weak_ptr<Hero>());

	//Dismissal from the hero army
	this->troops->GetOut(unit);
	{
		//Check if the unit have to be removed from the units in the same building of the hero.
		if (unit->IsInHolder() == true && heroSp->IsInHolder() == true)
		{
			auto heroBuilding = heroSp->GetHostBuilding();
			if (this->troopsInTheSameBuilding.units->Contains(unit) == true && unit->GetHostBuilding()->GetUniqueID() == heroBuilding->GetUniqueID())
			{
				//Unit is already in the building but without an hero. 
				//So, remove it from the hero's troops in the same building and updates the corrisponding health perc map.
				this->troopsInTheSameBuilding.units->GetOut(unit);
				this->troopsInTheSameBuilding.healthPerClass.UpdateHealthMap(this->troopsInTheSameBuilding.totHealthPerc, unit, OrderedUnitsList::Operations::e_remove);
				//And add it inside the garrison (the unit was already in the building, so I don't need to update counter because it was previously counted).
				heroBuilding->GetGarrison()->AddUnit(unit, false, std::nullopt);
			}
		}
	}
	
	//this->matchUI.lock()->NotifyTopbarChangement(this);

	//Remove unit from the formation priority map:
	{
		const uint8_t formationPriority = unit->GetFormationPriority();
		assert(this->formPriority.contains(formationPriority) == true);
		assert(this->formPriority.at(formationPriority).contains(unit) == true);
		if (this->formPriority.at(formationPriority).size() > 1)
			this->formPriority.at(formationPriority).erase(unit);
		else
			this->formPriority.erase(formationPriority);
	}


	//TODO - trovare un modo per ripristinare la vecchia velocita'
	//unit->SetSpeed(this->hero.lock()->GetSpeed());

	//Update info for topbar.
	this->Update(unit, OrderedUnitsList::Operations::e_remove, std::nullopt);

	return true;
}

bool Army::MarkUnitAsInTheSameBuilding(const std::shared_ptr<Unit>& unit)
{
	if (this->troops->Contains(unit) == true && this->troopsInTheSameBuilding.units->Contains(unit) == false)
	{
		//ASSERTION: if a I'm here, hero should be inside a building.
		assert(this->hero.lock()->IsInHolder() == true);
		this->troopsInTheSameBuilding.units->Insert(unit);
		this->troopsInTheSameBuilding.healthPerClass.UpdateHealthMap(this->troopsInTheSameBuilding.totHealthPerc, unit, OrderedUnitsList::Operations::e_insert);
		return true;
	}
	return false;
}

bool Army::UnmarkUnitAsInTheSameBuilding(const std::shared_ptr<Unit>& unit)
{
	if (this->troops->Contains(unit) == true && this->troopsInTheSameBuilding.units->Contains(unit) == true)
	{
		//ASSERTION: if a I'm here, hero should be inside a building.
		assert(this->hero.lock()->IsInHolder() == true);
		//ASSERTION: if I'm here, unit and hero must be in the same building.
		assert(this->hero.lock()->GetHostBuilding()->GetUniqueID() == unit->GetHostBuilding()->GetUniqueID() == true);

		this->troopsInTheSameBuilding.units->GetOut(unit);
		this->troopsInTheSameBuilding.healthPerClass.UpdateHealthMap(this->troopsInTheSameBuilding.totHealthPerc, unit, OrderedUnitsList::Operations::e_remove);
		
		return true;
	}
	return false;
}

const std::map<uint8_t, gobjsSPUSet_t>& Army::GetArmyByFormationPriorityCRef(void) const
{
	return this->formPriority;
}
#pragma endregion
