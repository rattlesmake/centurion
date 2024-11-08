#include "ordered_units_list.h"
#include <environments/game/classes/unit.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/objectsSet/army.h>

#include <bin_data_interpreter.h>

#pragma region Constructors:
OrderedUnitsList::OrderedUnitsList(void) :
	engine(Engine::GetInstance()), troops(troopsInfo.units)
{
	troopsInfo.units = std::shared_ptr<ObjsList>(new ObjsList(false));
}

OrderedUnitsList::OrderedUnitsListInfo::OrderedUnitsListInfo(const std::string& _iconName, const std::weak_ptr<Hero>& _hero, std::string& _id) :
	iconName(_iconName), heroWRef(_hero), counter(1), id(std::move(_id))
{
}

OrderedUnitsList::OrderedUnitsListInfo::OrderedUnitsListInfo(OrderedUnitsListInfo&& other) noexcept :
	iconName(std::move(other.iconName)), counter(other.counter), heroWRef(std::move(other.heroWRef)), id(other.id)
{
}
#pragma endregion


#pragma region To scripts members:
bool OrderedUnitsList::Contains(const std::shared_ptr<GObject>& obj)
{
	return this->troops->Contains(obj);
}

float OrderedUnitsList::GetPercHealth(const std::string& className) const
{
	return OrderedUnitsList::GetPercHealth(this->troopsInfo, className);
}

std::shared_ptr<ObjsList> OrderedUnitsList::GetTroops(void) const
{
	return this->troops;
}
#pragma endregion


#pragma region Public members:
const OrderedUnitsList::egressTimeByClassMap_t& OrderedUnitsList::GetEgressTimeByClassMapCRef(void) const
{
	return this->egressTimeByClass;
}

const OrderedUnitsList::counterByEgressTimeMap_t& OrderedUnitsList::GetCounterByEgressTimeMapCRef(void) const
{
	return this->counterByEgressTime;
}

void OrderedUnitsList::HandleHealthChangement(const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new)
{
	OrderedUnitsList::HandleHealthChangement(this->troopsInfo.healthPerClass, this->troopsInfo.totHealthPerc, _unitClassName, _unitID, _old, _new);
}
#pragma endregion

#pragma region Protected members:
void OrderedUnitsList::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	const auto& objsByClass = this->troops->GetObjsByClassMapCRef();
	for (auto const& [egress_time, info] : this->counterByEgressTime)
	{
		std::shared_ptr<Hero> hero = info.heroWRef.lock();
		if (hero)
		{
			//Save unique ID and egress time of the hero
			BinaryDataInterpreter::PushUInt32(data, hero->GetUniqueID());
			BinaryDataInterpreter::PushDouble(data, egress_time);
		}
		else
		{
			//Save unique ID and egress time of each object belonging to the current class
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

void OrderedUnitsList::Update(const std::shared_ptr<Unit>& unit, const Operations operationToDo, const std::optional<egressTimeInstant_t>&& egressTime)
{
	//The following computation is required only in a match environment
	if (this->engine.GetEnvironmentId() != IEnvironment::Environments::e_match)
		return;

	std::string className = unit->GetClassName();
	std::weak_ptr<Hero> heroWP;
	const bool bIsHero = unit->IsHero() == true;
	if (bIsHero == true)
	{
		className += std::to_string(unit->GetUniqueID());
		heroWP = std::static_pointer_cast<Hero>(unit);
	}

	if (operationToDo == Operations::e_insert)
	{
		if (this->egressTimeByClass.contains(className) == true)
		{
			auto& egressTime = this->egressTimeByClass.at(className);
			auto& currentElement = this->counterByEgressTime.at(egressTime);
			currentElement.counter += 1;
		}
		else
		{
			auto _egressTime = (egressTime.has_value() == false) ? engine.GetGameTime().GetElapsedSecs() : egressTime.value();
			this->egressTimeByClass.insert({ className, _egressTime });
			this->counterByEgressTime.insert({ _egressTime, OrderedUnitsListInfo(unit->GetIconName(), heroWP,  className) });
		}
	}
	else if (operationToDo == Operations::e_remove)
	{
		assert(this->egressTimeByClass.contains(className) == true);
		auto& egressTime = this->egressTimeByClass.at(className);
		auto& currentElement = this->counterByEgressTime.at(egressTime);
		if (currentElement.counter > 1)
		{
			currentElement.counter -= 1;
		}
		else
		{
			this->counterByEgressTime.erase(egressTime);
			this->egressTimeByClass.erase(className);
		}
	}
	else
	{
		//Assertion: if I'm here, there was an error when parameters was provided to function.
		assert(true == false);
	}
	this->troopsInfo.healthPerClass.UpdateHealthMap(this->troopsInfo.totHealthPerc, unit, operationToDo);
}

float OrderedUnitsList::GetPercHealth(const troops_s& troopsStruct, const std::string& className)
{
	float percentage = -1.0f;
	const bool bEmpty = (className.empty() == false);
	const uint32_t size = (bEmpty) ? troopsStruct.units->CountByClass(className) : troopsStruct.units->Count();
	if (size >= 1)
	{
		const float value = (bEmpty) ? static_cast<float>(troopsStruct.healthPerClass.At(className).first) : static_cast<float>(troopsStruct.totHealthPerc);
		percentage = value / size;
	}
	return percentage;
}

void OrderedUnitsList::HandleHealthChangement(HealthPerClassMap& _hMap, HealthPerClassMap::perHealth_t& _tot, const std::string& _unitClassName, const uniqueID_t _unitID, const HealthPerClassMap::perHealth_t _old, const HealthPerClassMap::perHealth_t _new)
{
	if (_hMap.hMap.contains(_unitClassName) == false)
		return;
	HealthPerClassMap::hmapValue_t& value = _hMap.hMap.at(_unitClassName);
	_tot -= _old;
	_tot += _new;
	value.first -= _old;
	value.first += _new;
	if (value.first == 0)
	{
		_hMap.hMap.erase(_unitClassName);
	}
}
#pragma endregion

void OrderedUnitsList::HealthPerClassMap::UpdateHealthMap(perHealth_t& tot, const std::shared_ptr<Unit>& unit, const OrderedUnitsList::Operations operationToDo)
{
	const std::string& unitClassName = unit->GetClassNameCRef();
	const uint8_t unitPercHealth = unit->GetPercentHealth();
	if (operationToDo == Operations::e_insert)
	{
		if (this->hMap.contains(unitClassName) == true)
		{
			auto& info = this->hMap.at(unitClassName);
			info.first += unitPercHealth;
			assert(info.second.contains(unit->GetUniqueID()) == false);
			info.second.insert(unit->GetUniqueID());
		}
		else
		{
			std::unordered_set<uniqueID_t>set{ unit->GetUniqueID() };
			auto p = std::make_pair(unitPercHealth, std::move(set));
			this->hMap.insert({ unitClassName, std::move(p) });
		}
		tot += unitPercHealth;
	}
	else //Remove operation.
	{
		assert(this->hMap.contains(unitClassName) == true);
		auto& info = this->hMap.at(unitClassName);
		info.first -= unitPercHealth;
		info.second.erase(unit->GetUniqueID());
		if (info.second.empty() == true)
		{
			assert(info.first == 0);
			this->hMap.erase(unitClassName);
		}
		tot -= unitPercHealth;
	}
}

const OrderedUnitsList::HealthPerClassMap::hmapValue_t& OrderedUnitsList::HealthPerClassMap::At(const std::string& key) const
{
	return this->hMap.at(key);
}
