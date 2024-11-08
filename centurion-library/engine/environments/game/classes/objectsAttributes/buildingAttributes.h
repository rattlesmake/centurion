/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cctype>
#include <memory>
#include <string>

#include <centurion_typedef.hpp>

#ifndef BUILDING_MAX_HEALTH_VALUE
#define BUILDING_MAX_HEALTH_VALUE       100000
#endif 


class BuildingAttributes
{
public:
	BuildingAttributes(const BuildingAttributes& other) = delete;
	BuildingAttributes& operator=(const BuildingAttributes& other) = delete;

	[[nodiscard]] bool CanAutoRepair(void) const noexcept;

	[[nodiscard]] bool CanProduceFood(void) const noexcept;

	[[nodiscard]] bool CanProduceGold(void) const noexcept;

	[[nodiscard]] bool IsCentralBuilding(void) const noexcept;

	[[nodiscard]] uint8_t GetLoyaltyFearHealthPerc(void) const noexcept;
	void SetLoyaltyFearHealthPerc(const uint8_t _loyaltyFearHealthPercent);

	[[nodiscard]] uint32_t GetHealth(void) const noexcept;
	virtual void SetHealth(uint32_t _health) = 0;

	[[nodiscard]] uint8_t GetPercentHealth(void) const noexcept;
	void SetPercentHealth(const uint8_t _percentHealth);

	[[nodiscard]] uint8_t GetRepairRate(void) const noexcept;
	void SetRepairRate(const uint8_t _repairRate);

	[[nodiscard]] uint32_t GetMaxHealth(void) const noexcept;
	void SetMaxHealth(const uint32_t _maxHealth);
protected:
	BuildingAttributes(void) = default;

	void SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool temporary);

	void GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const;
	std::ostream& Serialize(std::ostream& out, const std::string& tabs) const;

	uint32_t health = 0;
private:
	bool bCanAutoRepair = false;
	bool bCanProduceFood = false;
	bool bCanProduceGold = false;
	bool bIsCentralBuilding = false;
	uint8_t loyaltyFearHealthPercent = 0;
	uint8_t repairRate = 0;
	uint8_t percentHealth = 100;
	uint32_t maxHealth = 0;

	// This flag indicates which attributes are being set during a loading (both from XML and from binary file).
	// If true, an exception will be thrown, when a set method is invoked, if the value to be set is not valid.
	// This indicates an error in the file.
	bool bLoading = false;
};
