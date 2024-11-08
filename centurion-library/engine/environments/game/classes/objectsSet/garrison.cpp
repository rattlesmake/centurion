#include "garrison.h"
#include "objects_list.h"
#include <environments/game/classes/building.h>
#include <environments/game/classes/unit.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/objectsSet/army.h>

#include <bin_data_interpreter.h>

#pragma region Constructors and destructor:
Garrison::Garrison(const std::weak_ptr<Building>& _building, const uint32_t _maxNumberOfTroops):
	building(_building), maxNumberOfTroops(_maxNumberOfTroops)
{
}

Garrison::~Garrison(void)
{
}
#pragma endregion

#pragma region Static members:
std::shared_ptr<Garrison> Garrison::CreateGarrison(const std::shared_ptr<Building>& _building, const uint32_t _maxNumberOfTroops)
{
	assert(_building);
	return std::shared_ptr<Garrison>(new Garrison(_building, _maxNumberOfTroops));
}
#pragma endregion

#pragma region To scripts members:
uint32_t Garrison::GetNumberOfUnits(void) const
{
	return this->numberOfTroops;
}

uint32_t Garrison::GetMaxNumberOfUnits(void) const
{
	return this->maxNumberOfTroops;
}
#pragma endregion

bool Garrison::AddUnit(const std::shared_ptr<Unit>& unit, const bool bChangeCounter, std::optional<egressTimeInstant_t>&& egressTime)
{	
	//These checks are useful even if this function cannot directly be called by an external script.
	//In fact, this function can indirectly be called by other API functions (e.g. Unit::GoToEnter).
	if (!unit)
		return false;
	if (this->troops->Contains(unit) == true)
		return false;
	if (this->numberOfTroops == this->maxNumberOfTroops)
		return false;

	this->numberOfTroops += bChangeCounter;
	{
		std::shared_ptr<Hero> heroOfTheUnit = unit->GetHero();
		//[1] If the unit is attached to an hero and its hero is in the same building, the hero will receive the notify that the unit is in the same building and after that then only [2] is necessary.
		//Else, first the unit will be added to the ObjsList containing the units inside of the current building.
		if (heroOfTheUnit && heroOfTheUnit->GetHostBuilding() && heroOfTheUnit->GetHostBuilding()->GetUniqueID() == this->building.lock()->GetUniqueID())
			heroOfTheUnit->GetArmy()->MarkUnitAsInTheSameBuilding(unit);
		else
		{
			this->troops->Insert(unit);
			//Update info for topbar.
			this->Update(unit, OrderedUnitsList::Operations::e_insert, std::move(egressTime));
		}
	}
	//[2] Unit will know that it is in this building:
	unit->SetBuilding(this->building);

	//If the unit just added to the current building is an hero,
	//for each unit attached to the hero check if the unit is in the current building too.
	//If so, the unit will removed from the ObjsList of the units inside this building.
	if (unit->IsHero() == true)
		this->RemoveUnits(std::static_pointer_cast<Hero>(unit)->GetArmy()->GetTroops(), std::static_pointer_cast<Hero>(unit)->GetArmy());

	//this->matchUI.lock()->NotifyTopbarChangement(this);
	return true;	
}

bool Garrison::RemoveUnit(const std::shared_ptr<Unit>& unit, const bool bChangeCounter)
{
	//These checks are usefull even if this function cannot directly be called by an external script.
	//In fact, this function can indirectly be called by other API functions (e.g. Unit::GoToEnter).
	if (!unit)
		return false;
	if (this->troops->Contains(unit) == false)
		return false;

	this->numberOfTroops -= bChangeCounter;
	this->troops->GetOut(unit);
	//Removed unit will be made aware that it isn't in a building anymore.
	unit->SetBuilding(std::weak_ptr<Building>());
	//Update info for topbar.
	this->Update(unit, OrderedUnitsList::Operations::e_remove, std::nullopt);
	//this->matchUI.lock()->NotifyTopbarChangement(this);
	return true;
}

void Garrison::DecreaseNumberOfTroops(const uint32_t n)
{
	this->numberOfTroops -= n;
}

#pragma region Inherited methods:
void Garrison::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	//Firstly, save the ID of the building owning this garrison
	//N.B.: This info will be the load by PlayersArray::DeserializeFromBinFile, so Garrison::InitByBinData must not consider it.
	assert(this->building.expired() == false);
	BinaryDataInterpreter::PushUInt32(data, this->building.lock()->GetUniqueID());

	//Secondly, save number of units inside the garrison (it also includes the number of units assigned to heroes in the garrison)
	BinaryDataInterpreter::PushUInt32(data, this->numberOfTroops);

	//Then save units inside garrison
	const auto& counterByEgressTimeMap = this->OrderedUnitsList::GetCounterByEgressTimeMapCRef();
	const auto& objsByClass = this->troops->GetObjsByClassMapCRef();
	for (auto const& [egress_time, info] : counterByEgressTimeMap)
	{
		std::shared_ptr<Hero> hero = info.heroWRef.lock();
		if (hero)
		{
			///If the current class is an hero class, save hero <ID, egressTime> and the number of units attached to him that're in the same building.
			///Then save the IDs of all the units attached to the hero that're always in this garrison.

			//Save unique ID and egress time.
			BinaryDataInterpreter::PushUInt32(data, hero->GetUniqueID());
			BinaryDataInterpreter::PushDouble(data, egress_time);

			std::shared_ptr<ObjsList> heroArmy = hero->GetArmy()->GetTroopsInTheSameBuilding();

			//Save number of units attached to the hero that're in the same building.
			BinaryDataInterpreter::PushUInt32(data, heroArmy->Count());

			//Save IDs and egress time of the units attached to the hero that are in the same building. As egress time, we use the same value of the hero.
			ObjsList::objs_const_iterator currentIt = heroArmy->cbegin();
			ObjsList::objs_const_iterator endIt = heroArmy->cend();
			while (currentIt != endIt)
			{
				auto unit = std::static_pointer_cast<Unit>((*currentIt).lock());
				assert(unit);
				BinaryDataInterpreter::PushUInt32(data, unit->GetUniqueID());
				BinaryDataInterpreter::PushDouble(data, egress_time);
				currentIt++; //Go to next unit.
			}
		}
		else
		{
			///If the current class is not an hero class, save the pair <ID, egressTime> for each unit who belong to the current class.
			///Units belonging to the same class have, of course, the same egress time, but we repet this information for an easier loading.

			auto& objs = objsByClass.at(info.id);
			for (auto const& obj : objs)
			{
				auto unit = std::static_pointer_cast<Unit>(obj.lock());
				assert(unit);
				BinaryDataInterpreter::PushUInt32(data, unit->GetUniqueID());
				BinaryDataInterpreter::PushDouble(data, egress_time);
			}
		}
	}
}

void Garrison::InitByBinData(std::vector<uint8_t>&& binData, uint32_t offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap)
{
	std::list<std::pair<egressTimeInstant_t, uint32_t>> tmpHeroesList;

	//Load the number of units inside the garrison (it also includes the number of units assigned to heroes in the garrison)
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

			//Add unit
			bValidAdd = this->AddUnit(std::static_pointer_cast<Unit>(obj), true, egressTime);

			//If the current unit is an hero, get the number of units that he had in the same building and store it inside the temporary list
			if (obj->IsHero() == true)
				tmpHeroesList.push_back({ egressTime, BinaryDataInterpreter::ExtractUInt32(binData, offset) });
		}
	}

	if (this->numberOfTroops == nTroops)
	{
		//Check if heroes has the same number of units in the same building that they had during the previous save.
		const auto& counterByEgressTimeMap = this->OrderedUnitsList::GetCounterByEgressTimeMapCRef();
		while (tmpHeroesList.empty() == false)
		{
			auto& heroInfo = tmpHeroesList.front();
			assert(counterByEgressTimeMap.contains(heroInfo.first));
			if (counterByEgressTimeMap.at(heroInfo.first).heroWRef.lock()->GetArmy()->GetTroopsInTheSameBuilding()->Count() != heroInfo.second)
				break; //Invalid number of units in the same building for this hero.
			tmpHeroesList.pop_front();
		}
	}

	if (this->numberOfTroops != nTroops || tmpHeroesList.empty() == false)
		throw BinaryDeserializerException("Invalid garrison for building having ID " + std::to_string(this->building.lock()->GetUniqueID()));

	assert(offset == static_cast<uint32_t>(binData.size()));
}
#pragma endregion

void Garrison::RemoveUnits(const std::shared_ptr<ObjsList>& units, const std::shared_ptr<Army>& heroArmy)
{
	ObjsList::objs_const_iterator currentIt = units->cbegin();
	ObjsList::objs_const_iterator endIt = units->cend();
	while (currentIt != endIt)
	{
		auto unit = std::static_pointer_cast<Unit>((*currentIt).lock());
		assert(unit);
		if (this->troops->Contains(unit) == true) //The unit is arrived in the building before its hero.
		{
			this->troops->GetOut(unit);
			heroArmy->MarkUnitAsInTheSameBuilding(unit);
			//Update info for topbar.
			this->Update(unit, OrderedUnitsList::Operations::e_remove, std::nullopt);
		}
		currentIt++; //Go to next unit.
	}
}
