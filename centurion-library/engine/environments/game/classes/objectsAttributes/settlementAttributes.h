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

#ifndef POPULATION_LIMIT
#define POPULATION_LIMIT		10000
#endif 

#ifndef GOLD_LIMIT
#define GOLD_LIMIT				1000000000
#endif 

#ifndef FOOD_LIMIT
#define FOOD_LIMIT				1000000000
#endif

#ifndef MIN_SETTL_RADIUS
#define MIN_SETTL_RADIUS		1500
#endif 

#ifndef SETTL_HITBOX_SIZE
#define SETTL_HITBOX_SIZE		2500
#endif 

#ifndef MAX_LOYALTY_VALUE 
#define MAX_LOYALTY_VALUE		100
#endif

#ifndef LOYALTY_LOSS_RATE
#define LOYALTY_LOSS_RATE		2  //seconds
#endif 

#ifndef LOYALTY_GAIN_RATE
#define LOYALTY_GAIN_RATE		(LOYALTY_LOSS_RATE << 1)  //i.e. LOYALTY_LOSS_RATE * 2 seconds
#endif 

#ifndef MAX_LOYALTY_VARIATION
#define MAX_LOYALTY_VARIATION		10  
#endif 


class SettlementAttributes
{
public:
	SettlementAttributes(const SettlementAttributes& other) = delete;
	SettlementAttributes& operator=(const SettlementAttributes& other) = delete;

	[[nodiscard]] uint32_t GetFood(void) const noexcept;
	void SetFood(const uint32_t _food);

	[[nodiscard]] int GetGold(void) const noexcept;
	void SetGold(const int _gold);

	[[nodiscard]] uint32_t GetPopulation(void) const noexcept;
	void SetPopulation(const uint32_t _population);

	[[nodiscard]] uint32_t GetMaxPopulation(void) const noexcept;
	void SetMaxPopulation(const uint32_t _maxPopulation);

	[[nodiscard]] uint8_t GetLoyalty(void) const noexcept;
	void SetLoyalty(const uint8_t _loyalty);
protected:
	SettlementAttributes(void) = default;

	void SetAttributes(const classData_t& xmlData, gobjData_t* dataSource);
	void GetXmlAttributesAsBinaryData(std::vector<uint8_t>& data) const;
	std::ostream& Serialize(std::ostream& out, const std::string& tabs) const;

	double nextLoyaltyChangementInstant = 0;
private:
	int gold = 0;
	uint32_t food = 0;
	uint32_t population = 0;
	uint32_t maxPopulation = 0;

	// The loyalty of the settlement.
	uint8_t loyalty = 0;

	// This flag indicates which attributes are being set during a loading (both from XML and from binary file).
	// If true, an exception will be thrown, when a set method is invoked, if the value to be set is not valid.
	// This indicates an error in the file.
	bool bLoading = false;
};
