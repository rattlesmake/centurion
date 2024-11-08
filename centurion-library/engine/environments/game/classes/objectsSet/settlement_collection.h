/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <tuple>

#include <environments/game/classes/objectsSet/building_settlement_bridge.h>

class Building;
class Settlement;

class SettlementsCollection
{
public:
	SettlementsCollection(void);
	SettlementsCollection(const std::shared_ptr<SettlementsCollection> other) = delete;
	std::shared_ptr<SettlementsCollection> operator=(const std::shared_ptr<SettlementsCollection> other) = delete;
	~SettlementsCollection(void);

	[[nodiscard]] static uint8_t GetTabs(void);
	static void SetTabs(const uint8_t tabs);

	[[nodiscard]] bool Contains(const uint32_t settlementId) const;
	void Insert(const std::shared_ptr<Settlement>& settlement);
	void Remove(const Settlement& settlement);
	void InsertName(std::string settlementName, const uint32_t settlementId);
	void RemoveName(const std::string& settlementName);

	[[nodiscard]] const std::unordered_map<uint32_t, std::weak_ptr<Settlement>>& GetSettlementsMapCRef(void) const;
	[[nodiscard]] std::shared_ptr<Settlement> GetSettlementByID(const uint32_t id) const;
	[[nodiscard]] std::shared_ptr<Settlement> GetSettlementByName(const std::string& name) const;
private:
	std::unordered_map<std::string, uint32_t> settlementsName;
	std::unordered_map<uint32_t, std::weak_ptr<Settlement>> settlementsMap;
	static uint8_t nTabs;
};
