#include "settlement_collection.h"
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>

#include <bin_data_interpreter.h>

#pragma region Static attributes:
uint8_t SettlementsCollection::nTabs = 1;
#pragma endregion


#pragma region Constructor and destructor:
SettlementsCollection::SettlementsCollection(void)
{
}

SettlementsCollection::~SettlementsCollection(void)
{
}
#pragma endregion


#pragma region Static members:
uint8_t SettlementsCollection::GetTabs(void)
{
	return SettlementsCollection::nTabs;
}

void SettlementsCollection::SetTabs(const uint8_t tabs)
{
	SettlementsCollection::nTabs = tabs;
}
#pragma endregion

bool SettlementsCollection::Contains(const uint32_t settlementId) const
{
	return this->settlementsMap.contains(settlementId);
}

void SettlementsCollection::Insert(const std::shared_ptr<Settlement>& settlement)
{
	this->settlementsMap.insert({ settlement->GetId(), settlement });
}

void SettlementsCollection::Remove(const Settlement& settlement)
{
	this->settlementsMap.erase(settlement.GetId());
	const ::string settlementName = settlement.GetSettlementName();
	if (settlementName.empty() == false)
		this->settlementsName.erase(settlementName);
}

void SettlementsCollection::InsertName(std::string settlementName, const uint32_t settlementId)
{
	this->settlementsName.insert({ std::move(settlementName), settlementId });
}

void SettlementsCollection::RemoveName(const std::string& settlementName)
{
	this->settlementsName.erase(settlementName);
}

const std::unordered_map<uint32_t, std::weak_ptr<Settlement>>& SettlementsCollection::GetSettlementsMapCRef(void) const
{
	return this->settlementsMap;
}

std::shared_ptr<Settlement> SettlementsCollection::GetSettlementByID(const uint32_t id) const
{
	const bool bIdFound = (this->settlementsMap.contains(id) == true);
	if (bIdFound == true)
	{
		//Assertion: if I have an ID then a settlement MUST exist.
		assert(this->settlementsMap.at(id).expired() == false);
		return this->settlementsMap.at(id).lock();
	}
	return std::shared_ptr<Settlement>();
}

std::shared_ptr<Settlement> SettlementsCollection::GetSettlementByName(const std::string& name) const
{
	if (this->settlementsName.contains(name) == true)
	{
		auto sp = SettlementsCollection::GetSettlementByID(this->settlementsName.at(name));
		//Assertion: if the settlementsName map contains a name, the asocciated settlement MUST exist.
		assert(sp);
		return sp;
	}
	return std::shared_ptr<Settlement>();
}
