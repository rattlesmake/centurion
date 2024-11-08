#include "wagon.h"

// Assets
#include <xml_class.h>


#pragma region Constructors and destructor:
Wagon::Wagon(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource)
	: Unit(_player, std::move(_pos), _bTemporary, _objData, surface, dataSource)
{
}

Wagon::~Wagon(void)
{
}
#pragma endregion


#pragma region To scripts members:
uint16_t Wagon::GetCarriedGold(void) const noexcept
{
	return this->goldCarried;
}

uint16_t Wagon::GetCarriedFood(void) const noexcept
{
	return this->foodCarried;
}
#pragma endregion

#pragma region Inherited methods:
void Wagon::GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const
{
	this->Unit::GetBinRepresentation(data, calledByChild);

	//TODO
}

std::ostream& Wagon::Serialize(std::ostream& out, const bool calledByChild) const
{
	std::string tab(GObject::nTabs, '\t');
	std::string xml;
	if (calledByChild == false)
		xml = tab + "<wagon ";
	Unit::Serialize(out, true);
	if (calledByChild == false)
		xml += "/>";
	return out;
}

void Wagon::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	const auto& attributesMap = objData->get_attributes();

	this->Unit::SetAttrs(objData, dataSource);
	if (this->bIsTemporary == true)
		return;

	// TryParseFloat, TryParseInteger, TryParseString
	std::optional<int> iAttribute = 0;

	//Integer attributes:
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "goldCarried");
	this->goldCarried = iAttribute.value();
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "foodCarried");
	this->foodCarried = iAttribute.value();

	this->CheckAttributesValues();
}
#pragma endregion


#pragma region Private methods:
void Wagon::CheckAttributesValues(void)
{
	this->goldCarried = (this->goldCarried > MAX_GOLD_CAPACITY) ? MAX_GOLD_CAPACITY : this->goldCarried;
	this->foodCarried = (this->foodCarried > MAX_FOOD_CAPACITY) ? MAX_FOOD_CAPACITY : this->foodCarried;
}
#pragma endregion
