#include "gobjectAttributes.h"

// Assets
#include <xml_class.h>


bool GObjectAttributes::IsWaterObject(void) const noexcept
{
	return this->bIsWaterObject;
}

bool GObjectAttributes::IsAlwaysVisibleInGameMinimap(void) const noexcept
{
	return this->bAlwaysVisibleInGameMinimap;
}

std::string GObjectAttributes::GetRaceName(void) const noexcept
{
	return this->raceName;
}

void GObjectAttributes::SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool _temporary)
{
	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;
	std::optional<std::string> strAttribute;
	std::optional<bool> boolAttribute;

	auto& objAttrs = objData->get_attributes();

	strAttribute = assets::xml_class::try_parse_string(objAttrs, "race");
	this->raceName = std::move(strAttribute.value());

	if (_temporary == true)
		return;

	boolAttribute = assets::xml_class::try_parse_bool(objAttrs, "alwaysVisibleInGameMinimap");
	this->bAlwaysVisibleInGameMinimap = boolAttribute.value();

	boolAttribute = assets::xml_class::try_parse_bool(objAttrs, "isWaterObject");
	this->bIsWaterObject = boolAttribute.value();

	if (dataSource == nullptr)
	{
		;  // For now, any attrbitues is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		;  // For now, any attrbitues is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		;  // For now, any attrbitues is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
}

void GObjectAttributes::GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const
{
	// If you change saving order here, please go to GObjectAttributes::SetAttributes and impose the same loading order!
	
	return;  // Nothing have to be saved, for now.
}
