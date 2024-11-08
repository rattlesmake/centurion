#include "buildingAttributes.h"

#include <ostream>

#include <bin_data_interpreter.h>
#include <custom_exceptions.hpp>

#include <tinyxml2_utils.h>

// Assets
#include <xml_class.h>


bool BuildingAttributes::CanAutoRepair(void) const noexcept
{
	return this->bCanAutoRepair;
}

bool BuildingAttributes::CanProduceFood(void) const noexcept
{
	return this->bCanProduceFood;
}

bool BuildingAttributes::CanProduceGold(void) const noexcept
{
	return this->bCanProduceGold;
}

bool BuildingAttributes::IsCentralBuilding(void) const noexcept
{
	return this->bIsCentralBuilding;
}

uint8_t BuildingAttributes::GetLoyaltyFearHealthPerc(void) const noexcept
{
	return this->loyaltyFearHealthPercent;
}

void BuildingAttributes::SetLoyaltyFearHealthPerc(const uint8_t _loyaltyFearHealthPercent)
{
	this->loyaltyFearHealthPercent = (_loyaltyFearHealthPercent <= 100) ? _loyaltyFearHealthPercent : 100;
}

uint32_t BuildingAttributes::GetHealth(void) const noexcept
{
	return this->health;
}

uint8_t BuildingAttributes::GetPercentHealth(void) const noexcept
{
	return this->percentHealth;
}

void BuildingAttributes::SetPercentHealth(const uint8_t _percentHealth)
{
	if (this->bLoading == true && _percentHealth > 100)
		throw BinaryDeserializerException("Building::SetPercentHealth --> percentHealth = " + std::to_string(_percentHealth));  // PercentHealth READ FROM FILE is not valid!
	this->percentHealth = (_percentHealth <= 100) ? _percentHealth : 100;
}

uint8_t BuildingAttributes::GetRepairRate(void) const noexcept
{
	return this->repairRate;
}

void BuildingAttributes::SetRepairRate(const uint8_t _repairRate)
{
	this->repairRate = (_repairRate <= 100) ? _repairRate : 100;
}

uint32_t BuildingAttributes::GetMaxHealth(void) const noexcept
{
	return this->maxHealth;
}

void BuildingAttributes::SetMaxHealth(const uint32_t _maxHealth)
{
	this->maxHealth = (_maxHealth <= BUILDING_MAX_HEALTH_VALUE) ? _maxHealth : BUILDING_MAX_HEALTH_VALUE;
}

void BuildingAttributes::SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool temporary)
{
	this->bLoading = (dataSource != nullptr);  // If true, building is being loaded from a file otherwise it's being created manually (e.g. from a barracks or placing it in the editor).

	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;
	std::optional<std::string> strAttribute;
	std::optional<bool> boolAttribute;

	const auto& attributesMap = objData->get_attributes();

	boolAttribute = assets::xml_class::try_parse_bool(attributesMap, "isCentralBuilding");
	this->bIsCentralBuilding = boolAttribute.value();

	if (temporary == true)
		return;

	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxHealth");
	this->maxHealth = iAttribute.value();
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "repairRate");
	this->repairRate = iAttribute.value();
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "loyaltyFearHealthPercent");
	this->loyaltyFearHealthPercent = iAttribute.value();

	boolAttribute = assets::xml_class::try_parse_bool(attributesMap, "canAutoRepair");
	this->bCanAutoRepair = boolAttribute.value();
	boolAttribute = assets::xml_class::try_parse_bool(attributesMap, "canProduceGold");
	this->bCanProduceGold = boolAttribute.value();
	boolAttribute = assets::xml_class::try_parse_bool(attributesMap, "canProduceFood");
	this->bCanProduceFood = boolAttribute.value();

	if (dataSource == nullptr)
	{
		this->SetPercentHealth(100);
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->SetPercentHealth(tinyxml2::TryParseFirstChildIntContent(xml, "percentHealth"));
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		this->SetPercentHealth(BinaryDataInterpreter::ExtractUInt8((*binData.first), binData.second));
	}

	this->health = this->percentHealth / 100 * this->maxHealth;

	// Always equals to false, at the end of this method
	this->bLoading = false;
}

void BuildingAttributes::GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const
{
	// Unsigned Int 8 bit
	BinaryDataInterpreter::PushUInt8(data, this->GetPercentHealth());
}

std::ostream& BuildingAttributes::Serialize(std::ostream& out, const std::string& tabs) const
{
	out << tabs << "\t<percentHealth>" << std::to_string(this->percentHealth) << "</percentHealth>\n";
	return out;
}
