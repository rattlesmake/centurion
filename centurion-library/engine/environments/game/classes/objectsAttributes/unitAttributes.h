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

#ifndef MAX_UNIT_HEALTH_VALUE
#define MAX_UNIT_HEALTH_VALUE 300000
#endif
#ifndef MAX_LEVEL_VALUE
#define MAX_LEVEL_VALUE 1000
#endif
#ifndef MAX_ATTACK_VALUE
#define MAX_ATTACK_VALUE 9999
#endif
#ifndef MAX_ARMOR_VALUE
#define MAX_ARMOR_VALUE 300
#endif 
#ifndef MAX_FOOD_VALUE
#define MAX_FOOD_VALUE 5000
#endif 
#ifndef MAX_FORMATION_PRIORITY_VALUE    
#define MAX_FORMATION_PRIORITY_VALUE 50
#endif   
#ifndef MIN_RANGE_VALUE
#define MIN_RANGE_VALUE 50
#endif
#ifndef MAX_RANGE_VALUE
#define MAX_RANGE_VALUE 2000
#endif
#ifndef MAX_SPEED_VALUE
#define MAX_SPEED_VALUE 300
#endif 
#ifndef MAX_STAMINA_VALUE
#define MAX_STAMINA_VALUE 10
#endif
#ifndef MAX_MANA_VALUE
#define MAX_MANA_VALUE 100
#endif 
#ifndef MAX_ZOFFSET_VALUE
#define MAX_ZOFFSET_VALUE 50
#endif 


enum class DamageTypes
{
	e_undefined = -1,
	e_slash = 0,
	e_pierce,
	e_both,
};


struct AttackerDamageParams_s
{
	AttackerDamageParams_s(void) = default;
	AttackerDamageParams_s(const uint16_t _levelAttacker, const uint16_t _minAttackAttacker, const uint16_t _maxAttackAttacker, const DamageTypes _typeAttackAttacker) :
		level(_levelAttacker), minAttack(_minAttackAttacker), maxAttack(_maxAttackAttacker), damageType(_typeAttackAttacker) { }
	DamageTypes damageType = DamageTypes::e_undefined;
	uint16_t level = 0;
	uint16_t maxAttack = 0;
	uint16_t minAttack = 0;
};

class UnitAttributes
{
public:
	UnitAttributes(const UnitAttributes& other) = delete;
	UnitAttributes& operator=(const UnitAttributes& other) = delete;

	struct UnitValues
	{
	public:
		uint32_t& maxHealth;
		uint32_t& health;
		uint16_t& level;
		uint16_t& minAttack;
		uint16_t& maxAttack;
		uint16_t& armorPierce;
		uint16_t& armorSlash;
		uint16_t& food;
		uint16_t& maxFood;
		uint16_t& speed;
		uint16_t& range;
		uint8_t& stamina;
		DamageTypes& damageType;
	private:
		UnitValues(void) = default;
		UnitValues(uint32_t& _health, uint32_t& _maxHealth, uint16_t& _level, uint16_t& _minAttack, uint16_t& _maxAttack, uint16_t& _armorPierce, uint16_t& _armorSlash, uint16_t& _food, uint16_t& _maxFood, uint16_t& _speed, uint16_t& _range, uint8_t& _stamina, DamageTypes& _damageType);
		friend UnitAttributes;
	};

	[[nodiscard]] const AttackerDamageParams_s& GetAttackerDamageParamsCRef(void) const noexcept;
	[[nodiscard]] const UnitAttributes::UnitValues& GetAttributesValues(void) const noexcept;

	[[nodiscard]] bool CanBeDamagedByGhost(void) const noexcept;
	[[nodiscard]] bool CanBeInvisible(void) const noexcept;
	[[nodiscard]] bool IsDualWeilding(void) const noexcept;

	[[nodiscard]] std::string GetPluralName(void) const noexcept;
	void SetPluralName(std::string _pluralName);

	[[nodiscard]] uint32_t GetHealth(void) const noexcept;
	virtual void SetHealth(uint32_t _health) = 0;

	[[nodiscard]] uint32_t GetMaxHealth(void) const noexcept;
	void SetMaxHealth(const uint32_t _maxHealth);

	[[nodiscard]] uint16_t GetLevel(void) const noexcept;
	void SetLevel(uint16_t _level);

	[[nodiscard]] uint16_t GetMinAttack(void) const noexcept;
	void SetMinAttack(const uint16_t _minAttack);

	[[nodiscard]] uint16_t GetMaxAttack(void) const noexcept;
	void SetMaxAttack(const uint16_t _maxAttack);

	[[nodiscard]] uint16_t GetArmorPierce(void) const noexcept;
	void SetArmorPierce(const uint16_t _armorPierce);

	[[nodiscard]] uint16_t GetArmorSlash(void) const noexcept;
	void SetArmorSlash(const uint16_t _armorSlash);

	[[nodiscard]] uint16_t GetFood(void) const noexcept;
	void SetFood(const uint16_t _food);

	[[nodiscard]] uint16_t GetMaxFood(void) const noexcept;
	void SetMaxFood(const uint16_t _maxFood);

	[[nodiscard]] uint16_t GetRange(void) const noexcept;
	void SetRange(const uint16_t _range);

	[[nodiscard]] uint16_t GetSpeed(void) const noexcept;
	void SetSpeed(const uint16_t _speed);

	[[nodiscard]] uint8_t GetPercentHealth(void) const noexcept;
	void SetPercentHealth(const uint8_t _percentHealth);

	[[nodiscard]] uint8_t GetStamina(void) const noexcept;
	void SetStamina(const uint8_t _stamina);

	[[nodiscard]] uint8_t GetMaxStamina(void) const noexcept;

	[[nodiscard]] uint8_t GetFormationPriority(void) const noexcept;
	void SetFormationPriority(const uint8_t _formationPriority);

	[[nodiscard]] uint8_t GetZOffeset(void) const noexcept;
	void SetZOffset(const uint8_t _zOffset);

	[[nodiscard]] DamageTypes GetDamageType(void) const noexcept;
	[[nodiscard]] std::string GetDamageTypeStr(void) const noexcept;

protected:
	UnitAttributes(void);

	[[nodiscard]] const std::string& GetPluralNameCRef(void) const noexcept;
	void SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool temporary);
	void GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const;
	std::ostream& Serialize(std::ostream& out, const std::string& tabs) const;

	uint32_t health = 0;
private:
	// This flag indicates which attributes are being set during a loading (both from XML and from binary file).
	// If true, an exception will be thrown, when a set method is invoked, if the value to be set is not valid.
	// This indicates an error in the file.
	bool bLoading = false;

	AttackerDamageParams_s damageParams;
	bool bCanBeDamagedByGhost = false;
	bool bCanBeInvisible = false;
	bool bIsDualWeilding = false;
	bool bHasFreedom = false;
	
	std::string pluralName;

	uint32_t maxHealth = 0;
	uint16_t armorPierce = 0;
	uint16_t armorSlash = 0;
	uint16_t food = 0;
	uint16_t maxFood = 0;
	uint16_t range = 0;
	uint16_t speed = 0;
	uint8_t percentHealth = 100;
	uint8_t stamina = 0;
	uint8_t formationPriority = 0;
	uint8_t healingRate = 0;
	uint8_t healingSpeedRate = 0;
	uint8_t zOffset = 0;

	UnitValues values;
};
