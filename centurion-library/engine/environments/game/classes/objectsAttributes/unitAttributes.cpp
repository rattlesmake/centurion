#include "unitAttributes.h"

#include <ostream>

#include <bin_data_interpreter.h>

#include <custom_exceptions.hpp>

#include <tinyxml2_utils.h>

// Assets
#include <xml_class.h>

// Services
#include <services/sqlservice.h>


const AttackerDamageParams_s& UnitAttributes::GetAttackerDamageParamsCRef(void) const noexcept
{
	return this->damageParams;
}

const UnitAttributes::UnitValues& UnitAttributes::GetAttributesValues(void) const noexcept
{
	return this->values;
}

DamageTypes UnitAttributes::GetDamageType(void) const noexcept
{
	return this->damageParams.damageType;
}

std::string UnitAttributes::GetDamageTypeStr(void) const noexcept
{
	switch (this->damageParams.damageType)
	{
	case DamageTypes::e_pierce:
		return "pierce";
	case DamageTypes::e_slash:
		return "slash";
	default:
		return "";
	}
}

bool UnitAttributes::CanBeDamagedByGhost(void) const noexcept
{
	return this->bCanBeDamagedByGhost;
}

bool UnitAttributes::CanBeInvisible(void) const noexcept
{
	return this->bCanBeInvisible;
}

bool UnitAttributes::IsDualWeilding(void) const noexcept
{
	return this->bIsDualWeilding;
}

std::string UnitAttributes::GetPluralName(void) const noexcept
{
	if (this->pluralName.empty() == true)
		return SqlService::GetInstance().GetTranslation("units", true);
	return SqlService::GetInstance().GetTranslation(this->pluralName, true);
}

void UnitAttributes::SetPluralName(std::string _pluralName)
{
	this->pluralName = std::move(_pluralName);
}

uint32_t UnitAttributes::GetHealth(void) const noexcept
{
	return this->health;
}

uint32_t UnitAttributes::GetMaxHealth(void) const noexcept
{
	return this->maxHealth;
}

void UnitAttributes::SetMaxHealth(const uint32_t _maxHealth)
{
	if (this->bLoading == true && (_maxHealth > MAX_UNIT_HEALTH_VALUE || _maxHealth < 1))
		throw BinaryDeserializerException("Unit::SetMaxHealth --> maxHealth = " + std::to_string(_maxHealth));  // MaxHealth READ FROM FILE is not valid!
	this->maxHealth = (_maxHealth <= MAX_UNIT_HEALTH_VALUE) ? _maxHealth : MAX_UNIT_HEALTH_VALUE;
}

uint16_t UnitAttributes::GetLevel(void) const noexcept
{
	return this->damageParams.level;
}

void UnitAttributes::SetLevel(uint16_t _level)
{
	if (this->bLoading == true && (_level > MAX_LEVEL_VALUE || _level < 1))
		throw BinaryDeserializerException("Unit::SetLevel --> level = " + std::to_string(_level));  // Level READ FROM FILE is not valid!
	if (_level == 0)
		_level = 1;  // 1 is the minimum allowed level
	this->damageParams.level = (_level <= MAX_LEVEL_VALUE) ? _level : MAX_LEVEL_VALUE;
}

uint16_t UnitAttributes::GetMinAttack(void) const noexcept
{
	return this->damageParams.minAttack;
}

void UnitAttributes::SetMinAttack(const uint16_t _minAttack)
{
	if (this->bLoading == true && (_minAttack > MAX_ATTACK_VALUE || _minAttack < 0))
		throw BinaryDeserializerException("Unit::SetMinAttack --> minAttack = " + std::to_string(_minAttack));  // MinAttack READ FROM FILE is not valid!
	this->damageParams.minAttack = (this->damageParams.minAttack > MAX_ATTACK_VALUE) ? MAX_ATTACK_VALUE : _minAttack;
	this->damageParams.maxAttack = (this->damageParams.minAttack > this->damageParams.maxAttack) ? this->damageParams.minAttack : this->damageParams.maxAttack;
}

uint16_t UnitAttributes::GetMaxAttack(void) const noexcept
{
	return this->damageParams.maxAttack;
}

void UnitAttributes::SetMaxAttack(const uint16_t _maxAttack)
{
	if (this->bLoading == true && (_maxAttack > MAX_ATTACK_VALUE || _maxAttack < 0))
		throw BinaryDeserializerException("Unit::SetMaxAttack --> maxAttack = " + std::to_string(_maxAttack));  // MaxAttack READ FROM FILE is not valid!
	this->damageParams.maxAttack = (_maxAttack > MAX_ATTACK_VALUE) ? MAX_ATTACK_VALUE : _maxAttack;
	this->damageParams.minAttack = (this->damageParams.minAttack > this->damageParams.maxAttack) ? this->damageParams.maxAttack : this->damageParams.minAttack;
}

uint16_t UnitAttributes::GetArmorPierce(void) const noexcept
{
	return this->armorPierce;
}

void UnitAttributes::SetArmorPierce(const uint16_t _armorPierce)
{
	if (this->bLoading == true && (_armorPierce > MAX_ARMOR_VALUE || _armorPierce < 0))
		throw BinaryDeserializerException("Unit::SetArmorPierce --> armorPierce = " + std::to_string(_armorPierce));  // ArmorPierce READ FROM FILE is not valid!
	this->armorPierce = (_armorPierce > MAX_ARMOR_VALUE) ? MAX_ARMOR_VALUE : _armorPierce;
}

uint16_t UnitAttributes::GetArmorSlash(void) const noexcept
{
	return this->armorSlash;
}

void UnitAttributes::SetArmorSlash(const uint16_t _armorSlash)
{
	if (this->bLoading == true && (_armorSlash > MAX_ARMOR_VALUE || _armorSlash < 0))
		throw BinaryDeserializerException("Unit::SetArmorSlash --> armorSlash = " + std::to_string(_armorSlash));  // ArmorSlash READ FROM FILE is not valid!
	this->armorSlash = (_armorSlash > MAX_ARMOR_VALUE) ? MAX_ARMOR_VALUE : _armorSlash;
}

uint16_t UnitAttributes::GetFood(void) const noexcept
{
	return this->food;
}

void UnitAttributes::SetFood(const uint16_t _food)
{
	if (this->bLoading == true && (_food > MAX_FOOD_VALUE || _food < 0))
		throw BinaryDeserializerException("Unit::SetFood --> food = " + std::to_string(_food));  // Food READ FROM FILE is not valid!
	this->food = (_food <= MAX_FOOD_VALUE) ? _food : MAX_FOOD_VALUE;
	if (this->food > this->maxFood)
		this->food = this->maxFood;
}

uint16_t UnitAttributes::GetMaxFood(void) const noexcept
{
	return this->maxFood;
}

void UnitAttributes::SetMaxFood(const uint16_t _maxFood)
{
	if (this->bLoading == true && (_maxFood > MAX_FOOD_VALUE || _maxFood < 0))
		throw BinaryDeserializerException("Unit::SetMaxFood --> maxFood = " + std::to_string(_maxFood));  // MaxFood READ FROM FILE is not valid!
	this->maxFood = (_maxFood <= MAX_FOOD_VALUE) ? _maxFood : MAX_FOOD_VALUE;
	if (this->food > this->maxFood)
		this->food = this->maxFood;
}

uint16_t UnitAttributes::GetRange(void) const noexcept
{
	return this->range;
}

void UnitAttributes::SetRange(const uint16_t _range)
{
	this->range = (_range <= MAX_RANGE_VALUE) ? _range : MAX_RANGE_VALUE;
	if (this->range < MIN_RANGE_VALUE)
		this->range = MIN_RANGE_VALUE;
}

uint16_t UnitAttributes::GetSpeed(void) const noexcept
{
	return this->speed;
}

void UnitAttributes::SetSpeed(const uint16_t _speed)
{
	if (this->bLoading == true && (_speed > MAX_SPEED_VALUE || _speed < 1))
		throw BinaryDeserializerException("Unit::SetSpeed --> speed = " + std::to_string(_speed));  // Speed READ FROM FILE is not valid!
	this->speed = (_speed <= MAX_SPEED_VALUE) ? _speed : MAX_SPEED_VALUE;
}

uint8_t UnitAttributes::GetPercentHealth(void) const noexcept
{
	return this->percentHealth;
}

void UnitAttributes::SetPercentHealth(const uint8_t _percentHealth)
{
	if (this->bLoading == true && (_percentHealth > 100 || _percentHealth < 1))
		throw BinaryDeserializerException("Unit::SetPercentHealth --> percentHealth = " + std::to_string(_percentHealth)); //PercentHealth READ FROM FILE is not valid!
	this->percentHealth = (_percentHealth <= 100) ? _percentHealth : 100;
}

uint8_t UnitAttributes::GetStamina(void) const noexcept
{
	return this->stamina;
}

void UnitAttributes::SetStamina(const uint8_t _stamina)
{
	if (this->bLoading == true && _stamina > MAX_STAMINA_VALUE)
		throw BinaryDeserializerException("Unit::SetStamina --> stamina = " + std::to_string(_stamina));  // Stamina READ FROM FILE is not valid!
	this->stamina = (_stamina <= MAX_STAMINA_VALUE) ? _stamina : MAX_STAMINA_VALUE;
}

uint8_t UnitAttributes::GetMaxStamina(void) const noexcept
{
	return MAX_STAMINA_VALUE;
}

uint8_t UnitAttributes::GetFormationPriority(void) const noexcept
{
	return this->formationPriority;
}

void UnitAttributes::SetFormationPriority(const uint8_t _formationPriority)
{
	if (this->bLoading == true && (_formationPriority > MAX_FORMATION_PRIORITY_VALUE || _formationPriority < 1))
		throw BinaryDeserializerException("Unit::SetFormationPriority --> formationPriority = " + std::to_string(_formationPriority));  // FormationPriority READ FROM FILE is not valid!
	this->formationPriority = (_formationPriority <= MAX_FORMATION_PRIORITY_VALUE) ? _formationPriority : MAX_FORMATION_PRIORITY_VALUE;
}

uint8_t UnitAttributes::GetZOffeset(void) const noexcept
{
	return this->zOffset;
}

void UnitAttributes::SetZOffset(const uint8_t _zOffset)
{
	this->zOffset = (_zOffset <= MAX_ZOFFSET_VALUE) ? _zOffset : MAX_ZOFFSET_VALUE;
}

UnitAttributes::UnitAttributes(void) :
	values(this->health, this->maxHealth, this->damageParams.level, this->damageParams.minAttack, this->damageParams.maxAttack, this->armorPierce, this->armorSlash, this->food, this->maxFood, this->speed, this->range, this->stamina, this->damageParams.damageType)
{
}

UnitAttributes::UnitValues::UnitValues(uint32_t& _health, uint32_t& _maxHealth, uint16_t& _level, uint16_t& _minAttack, uint16_t& _maxAttack, uint16_t& _armorPierce, uint16_t& _armorSlash, uint16_t& _food, uint16_t& _maxFood, uint16_t& _speed, uint16_t& _range, uint8_t& _stamina, DamageTypes& _damageType) :
	health(_health), maxHealth(_maxHealth), level(_level), minAttack(_minAttack), maxAttack(_maxAttack),
	armorPierce(_armorPierce), armorSlash(_armorSlash), food(_food), maxFood(_maxFood), speed(_speed), range(_range),
	stamina(_stamina), damageType(_damageType)
{
}

const std::string& UnitAttributes::GetPluralNameCRef(void) const noexcept
{
	return this->pluralName;
}

void UnitAttributes::SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool temporary)
{
	this->bLoading = (dataSource != nullptr);  // If true, unit is being loaded from a file otherwise it's being created manually (e.g. from a barracks or placing it in the editor).

	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;
	std::optional<std::string> strAttribute;
	std::optional<bool> booleanAttribute;

	const auto& attributesMap = objData->get_attributes();

	//String attributes:
	strAttribute = assets::xml_class::try_parse_string(attributesMap, "damageType");
	if (strAttribute == "slash")
		this->damageParams.damageType = DamageTypes::e_slash;
	else if (strAttribute == "pierce")
		this->damageParams.damageType = DamageTypes::e_pierce;
	assert(this->damageParams.damageType != DamageTypes::e_undefined);
	strAttribute = assets::xml_class::try_parse_string(attributesMap, "pluralName");
	this->pluralName = std::move(strAttribute.value());

	// Boolean attributes:
	booleanAttribute = assets::xml_class::try_parse_bool(attributesMap, "dualWeilding");
	this->bIsDualWeilding = booleanAttribute.value();
	booleanAttribute = assets::xml_class::try_parse_bool(attributesMap, "canBeDamagedByGhost");
	this->bCanBeDamagedByGhost = booleanAttribute.value();
	booleanAttribute = assets::xml_class::try_parse_bool(attributesMap, "canBeInvisible");
	this->bCanBeDamagedByGhost = booleanAttribute.value();

	// Integer attributes:
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxHealth");
	this->SetMaxHealth(iAttribute.value());
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxFood");
	this->SetMaxFood(iAttribute.value());
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "range");
	this->SetRange(iAttribute.value());
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "formationPriority");
	this->SetFormationPriority(iAttribute.value());
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "healingRate");
	this->healingRate = iAttribute.value();
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "healingSpeedRate");
	this->healingSpeedRate = iAttribute.value();
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "zOffset");
	this->SetZOffset(iAttribute.value());

	if (dataSource == nullptr)
	{
		this->damageParams.level = 1;
		this->food = this->maxFood;
		this->percentHealth = 100;
		this->stamina = MAX_STAMINA_VALUE;

		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "armorPierce");
		this->SetArmorPierce(iAttribute.value());

		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "armorSlash");
		this->SetArmorSlash(iAttribute.value());

		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "minAttack");
		this->SetMinAttack(iAttribute.value());

		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxAttack");
		this->SetMaxAttack(iAttribute.value());

		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "speed");
		this->SetSpeed(iAttribute.value());
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->SetArmorPierce(tinyxml2::TryParseFirstChildIntContent(xml, "armorPierce"));
		this->SetArmorSlash(tinyxml2::TryParseFirstChildIntContent(xml, "armorSlash"));
		this->SetFood(tinyxml2::TryParseFirstChildIntContent(xml, "food"));
		this->SetLevel(tinyxml2::TryParseFirstChildIntContent(xml, "level"));
		this->SetMaxAttack(tinyxml2::TryParseFirstChildIntContent(xml, "maxAttack"));
		this->SetMinAttack(tinyxml2::TryParseFirstChildIntContent(xml, "minAttack"));
		this->SetPercentHealth(tinyxml2::TryParseFirstChildIntContent(xml, "percentHealth"));
		this->SetStamina(tinyxml2::TryParseFirstChildIntContent(xml, "stamina"));
		this->SetSpeed(tinyxml2::TryParseFirstChildIntContent(xml, "speed"));
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		// If you change loading order here, please go to UnitAttributes::GetXmlAttributesAsBinaryData and impose the same saving order!
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);

		// Unsigned 16
		this->SetLevel(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetMaxAttack(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetMinAttack(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetArmorPierce(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetArmorSlash(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetFood(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));
		this->SetSpeed(BinaryDataInterpreter::ExtractUInt16((*binData.first), binData.second));

		// Unsigned 8
		this->SetPercentHealth(BinaryDataInterpreter::ExtractUInt8((*binData.first), binData.second));
		this->SetStamina(BinaryDataInterpreter::ExtractUInt8((*binData.first), binData.second));
	}

	// Always equals to false, at the end of this method
	this->bLoading = false;
}

void UnitAttributes::GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const
{
	// If you change saving order here, please go to Unit::SetAttributes and impose the same loading order!

	// Unsigned 16
	BinaryDataInterpreter::PushUInt16(data, this->damageParams.level);
	BinaryDataInterpreter::PushUInt16(data, this->damageParams.maxAttack);  // Always save before maxAttack than minAttack
	BinaryDataInterpreter::PushUInt16(data, this->damageParams.minAttack);
	BinaryDataInterpreter::PushUInt16(data, this->armorPierce);
	BinaryDataInterpreter::PushUInt16(data, this->armorSlash);
	BinaryDataInterpreter::PushUInt16(data, this->food);
	BinaryDataInterpreter::PushUInt16(data, this->speed);

	// Unsigned 8
	BinaryDataInterpreter::PushUInt8(data, this->percentHealth);
	BinaryDataInterpreter::PushUInt8(data, this->stamina);
}

std::ostream& UnitAttributes::Serialize(std::ostream& out, const std::string& tabs) const
{
	out << tabs << "\t<armorPierce>" << std::to_string(this->armorPierce) << "</armorPierce>\n";
	out << tabs << "\t<armorSlash>" << std::to_string(this->armorSlash) << "</armorSlash>\n";
	out << tabs << "\t<food>" << std::to_string(this->food) << "</food>\n";
	out << tabs << "\t<level>" << std::to_string(this->damageParams.level) << "</level>\n";
	out << tabs << "\t<minAttack>" << std::to_string(this->damageParams.minAttack) << "</minAttack>\n";
	out << tabs << "\t<maxAttack>" << std::to_string(this->damageParams.maxAttack) << "</maxAttack>\n";
	out << tabs << "\t<percentHealth>" << std::to_string(this->percentHealth) << "</percentHealth>\n";
	out << tabs << "\t<stamina>" << std::to_string(this->stamina) << "</stamina>\n";
	out << tabs << "\t<speed>" << std::to_string(this->speed) << "</speed>\n";

	return out;
}
