#include "heroAttributes.h"

// Assets
#include <xml_class.h>

uint8_t HeroAttributes::GetTroupExpPercGain(void) const noexcept
{
	return this->troupExpPercGain;
}

void HeroAttributes::SetTroupExpPercGain(const uint8_t _troupExpPercGain)
{
	this->troupExpPercGain = (_troupExpPercGain <= MAX_TROUP_EXP_PERC_GAIN_VALUE) ? _troupExpPercGain : MAX_TROUP_EXP_PERC_GAIN_VALUE;
}

void HeroAttributes::SetAttributes(const classData_t& objData, const bool _temporary)
{
	if (_temporary == true)
		return;

	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;

	const auto& attributesMap = objData->get_attributes();

	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "troupExpPercGain");
	this->troupExpPercGain = iAttribute.value();
}
