#include "playableAttributes.h"

#include <engine.h>
#include <encode_utils.h>
#include <environments/game/igame.h>
#include <environments/game/igameUi.h>
#include <services/sqlservice.h>

// Assets
#include <xml_class.h>


#ifndef INVALID_ICON_PATH
#define INVALID_ICON_PATH  "game/match/topbar/icons/classes/no_icon"
#endif


uint16_t PlayableAttributes::GetSight(void) const noexcept
{
	return this->sight;
}

void PlayableAttributes::SetSight(const uint16_t _sight)
{
	this->sight = (_sight <= MAX_SIGHT_VALUE) ? _sight : MAX_SIGHT_VALUE;
}

void PlayableAttributes::SetSingularName(std::string _singularName)
{
	this->singularName = std::move(_singularName);
}

std::string PlayableAttributes::GetSingularName(void) const noexcept
{
	if (this->singularName.empty() == true)
		return SqlService::GetInstance().GetTranslation("object", true);
	return SqlService::GetInstance().GetTranslation(this->singularName, true);
}

std::string PlayableAttributes::GetIconName(void)
{
	// todo
	// todo
	// todo
	// controllare questa parte
	if (this->iconName.empty() == true)
	//if (this->iconName.empty() == true || !ImageService::GetInstance().GetEnvironmentImage(this->iconName, false).lock())
		return INVALID_ICON_PATH;
	return this->iconName;
}

void PlayableAttributes::SetIconName(std::string _iconName)
{
	this->iconName = Encode::FixImageName(std::move(_iconName));
}

const std::string& PlayableAttributes::GetSingularNameCRef(void) const noexcept
{
	return this->singularName;
}

void PlayableAttributes::SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool _temporary)
{
	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;
	std::optional<std::string> strAttribute;
	std::optional<bool> boolAttribute;

	auto& objAttrs = objData->get_attributes();

	if (_temporary == true)
		return;

	strAttribute = assets::xml_class::try_parse_string(objAttrs, "iconName");
	this->iconName = Encode::FixImageName(std::move(strAttribute.value()));

	iAttribute = assets::xml_class::try_parse_integer(objAttrs, "sight");
	this->SetSight(iAttribute.value());

	strAttribute = assets::xml_class::try_parse_string(objAttrs, "singularName");
	this->singularName = std::move(strAttribute.value());

	if (dataSource == nullptr)
	{
		;  // For now, any attributes is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		;  // For now, any attributes is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		;  // For now, any attributes is saved neither in XML save file nor in binary save file, so we must load everithing from xml file of the class.
	}
}

void PlayableAttributes::GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const
{
	// If you change saving order here, please go to GObjectAttributes::SetAttributes and impose the same loading order!

	return;  // Nothing have to be saved, for now.
}