#include "settlementAttributes.h"
#include <engine.h>

#include <bin_data_interpreter.h>

// Assets
#include <xml_class.h>


uint32_t SettlementAttributes::GetFood(void) const noexcept
{
	return this->food;
}

void SettlementAttributes::SetFood(const uint32_t _food)
{
	if (this->bLoading == true && _food > FOOD_LIMIT)
		throw BinaryDeserializerException("Settlement::SetFood --> food = " + std::to_string(_food));  // Food READ FROM FILE is not valid!
	this->food = _food <= FOOD_LIMIT ? _food : FOOD_LIMIT;
}

int SettlementAttributes::GetGold(void) const noexcept
{
	return this->gold;
}

void SettlementAttributes::SetGold(int _gold)
{
	if (this->bLoading == true && std::abs(_gold) > GOLD_LIMIT)
		throw BinaryDeserializerException("Settlement::SetGold --> gold = " + std::to_string(_gold));  // Gold READ FROM FILE is not valid!
	if (_gold >= 0)
		this->gold = _gold <= GOLD_LIMIT ? _gold : GOLD_LIMIT;
	else
		this->gold = _gold >= (-GOLD_LIMIT) ? _gold : (-GOLD_LIMIT);
}

uint32_t SettlementAttributes::GetPopulation(void) const noexcept
{
	return this->population;
}

void SettlementAttributes::SetPopulation(const uint32_t _population)
{
	if (this->bLoading == true && (_population > POPULATION_LIMIT || _population > this->maxPopulation))
		throw BinaryDeserializerException("Settlement::SetPopulation --> population = " + std::to_string(_population));  // Population READ FROM FILE is not valid!
	this->population = _population <= POPULATION_LIMIT ? _population : POPULATION_LIMIT;
	if (this->population > this->maxPopulation)
		this->population = this->maxPopulation;
}

uint32_t SettlementAttributes::GetMaxPopulation(void) const noexcept
{
	return this->maxPopulation;
}

void SettlementAttributes::SetMaxPopulation(const uint32_t _maxPopulation)
{
	if (this->bLoading == true && (_maxPopulation > POPULATION_LIMIT || _maxPopulation < this->population))
		throw BinaryDeserializerException("Settlement::SetMaxPopulation --> max_population = " + std::to_string(_maxPopulation));  // MaxPopulation READ FROM FILE is not valid!
	this->maxPopulation = _maxPopulation <= POPULATION_LIMIT ? _maxPopulation : POPULATION_LIMIT;
	if (this->population > this->maxPopulation)
		this->population = this->maxPopulation;
}

uint8_t SettlementAttributes::GetLoyalty(void) const noexcept
{
	return this->loyalty;
}

void SettlementAttributes::SetLoyalty(const uint8_t _loyalty)
{
	if (this->bLoading == true && _loyalty > MAX_LOYALTY_VALUE)
		throw BinaryDeserializerException("Settlement::SetLoyalty --> loyalty = " + std::to_string(_loyalty));  // Loyalty READ FROM FILE is not valid!
	this->loyalty = (_loyalty <= MAX_LOYALTY_VALUE) ? _loyalty : MAX_LOYALTY_VALUE;
	this->nextLoyaltyChangementInstant = Engine::GetInstance().GetGameTime().GetTotalSeconds();
}

void SettlementAttributes::SetAttributes(const classData_t& xmlData, gobjData_t* dataSource)
{
	if (!xmlData)
		throw BinaryDeserializerException("Cannot find a class having name Settlement");

	this->bLoading = (dataSource != nullptr);  // If true, settlement is being loaded from a file otherwise it's being created manually (e.g. from a barracks or placing it in the editor).

	std::optional<int> iAttribute = 0;
	const auto& attributesMap = xmlData->get_attributes();
	if (dataSource == nullptr)
	{
		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "gold");
		this->gold = iAttribute.value();
		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "food");
		this->food = iAttribute.value();
		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "population");
		this->population = iAttribute.value();
		iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxPopulation");
		this->maxPopulation = iAttribute.value();

		this->loyalty = MAX_LOYALTY_VALUE;
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->SetGold(tinyxml2::TryParseFirstChildIntContent(xml, "gold"));
		this->SetFood(tinyxml2::TryParseFirstChildIntContent(xml, "food"));
		this->SetMaxPopulation(tinyxml2::TryParseFirstChildIntContent(xml, "maxPopulation"));  // Set always max population and only then population!
		this->SetPopulation(tinyxml2::TryParseFirstChildIntContent(xml, "population"));
		this->SetLoyalty(tinyxml2::TryParseFirstChildIntContent(xml, "loyalty"));
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		// If you change loading order here, please go to SettlementAttributes::GetXmlAttributesAsBinaryData and impose the same saving order!
		this->SetGold(BinaryDataInterpreter::ExtractInt32((*binData.first), binData.second));
		this->SetFood(BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second));
		this->SetMaxPopulation(BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second));  // Set always max population and only then population!
		this->SetPopulation(BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second));
		this->SetLoyalty(BinaryDataInterpreter::ExtractUInt8((*binData.first), binData.second));
	}

	// Always equals to false, at the end of this method
	this->bLoading = false;
}

void SettlementAttributes::GetXmlAttributesAsBinaryData(std::vector<uint8_t>& data) const
{
	// If you change saving order here, please go to SettlementAttributes::SetAttributes and impose the same loading order!
	BinaryDataInterpreter::PushInt32(data, this->gold);
	BinaryDataInterpreter::PushUInt32(data, this->food);
	BinaryDataInterpreter::PushUInt32(data, this->maxPopulation);  // Save maxPopulation ALWAYS BEFORE population (in order to avoid BinaryDeserializerException during loading)
	BinaryDataInterpreter::PushUInt32(data, this->population);
	BinaryDataInterpreter::PushUInt8(data, this->loyalty);
}

std::ostream& SettlementAttributes::Serialize(std::ostream& out, const std::string& tabs) const
{
	out << tabs << "\t<gold>" << std::to_string(this->gold) << "</gold>\n";
	out << tabs << "\t<food>" << std::to_string(this->food) << "</food>\n";
	out << tabs << "\t<population>" << std::to_string(this->population) << "</population>\n";
	out << tabs << "\t<maxPopulation>" << std::to_string(this->maxPopulation) << "</maxPopulation>\n";
	out << tabs << "\t<loyalty>" << std::to_string(this->loyalty) << "</loyalty>\n";

	return out;
}
