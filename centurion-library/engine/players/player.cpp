#include "players_array.h"
#include "player.h"

#include <environments/game/tlv_types.h>
#include <environments/game/races/races_interface.h>

// Assets
#include <xml_class_enums.h>

uint8_t Player::nTabs = 0;

#pragma region Constructors and destructor:
Player::Player(const uint8_t _playerdID, const Color& _defaultColor, const PlayersArray& ref) 
	: id(_playerdID), color(_defaultColor), playersRef(ref)
{
	this->selection = SelectedObjects::Create(this->id);
	this->playerObjs = std::unique_ptr<ObjsCollection>(new ObjsCollection(_playerdID));
	this->SetName("player_" + std::to_string(id));
	this->SetRace("rome");
}

Player::~Player(void) 
{
}
#pragma endregion

#pragma region Operators:
std::ostream& operator<<(std::ostream& out, const Player& player) noexcept
{
	const string tabs(Player::nTabs, '\t');
	out << tabs << "<player id=\"" << std::to_string(player.id) << "\">\n";
	ObjsCollection::SetTabs(Player::nTabs + 1);
	out << (*player.playerObjs);
	out << '\n' << tabs << "</player>";
	return out;
}
#pragma endregion

#pragma region Static members:
void Player::SetTabs(const uint8_t tabs)
{
	Player::nTabs = tabs;
}
#pragma endregion

void Player::DeserializeFromXML(const classesData_t& classesDataSP, tinyxml2::XMLElement& playerTag, const std::shared_ptr<Surface>& surface)
{
	auto decorationsTag = playerTag.FirstChildElement("Decorations");
	auto buildingsTag = playerTag.FirstChildElement("Buildings");
	auto unitsTag = playerTag.FirstChildElement("Units");
	assert(decorationsTag != nullptr && buildingsTag != nullptr && unitsTag != nullptr);
	std::map<assets::xml_class_type, tinyxml2::XMLElement*> tags;
	tags[assets::xml_class_type::e_decorationClass] = decorationsTag;
	tags[assets::xml_class_type::e_buildingClass] = buildingsTag;
	tags[assets::xml_class_type::e_unitClass] = unitsTag;
	this->playerObjs->DeserializeFromXML(classesDataSP, tags, surface);
}

bool Player::IsCurrentPlayer(void) const
{
	return (this->playersRef.GetCurrentPlayerID() == this->id);
}

std::shared_ptr<SelectedObjects> Player::GetSelection(void) const
{
	return this->selection;
}

void Player::SetName(std::string name)
{
	this->name = std::move(name);
}

std::string Player::GetName(void) const
{
	return this->name;
}

void Player::SetRace(const std::string raceName)
{
	this->race = RacesInterface::GetRacePtr(raceName);
}

std::string Player::GetRace(void) const
{
	return this->race.lock()->GetRaceName();
}

void Player::SetColor(Color& color)
{
	this->color = color;
}

Color& Player::GetColor(void)
{
	return this->color;
}

void Player::SetBonus(const uint8_t bonus)
{
	this->bonus = bonus;
}

uint8_t Player::GetBonus(void) const
{
	return this->bonus;
}

void Player::SetStartingPoint(const glm::vec2 point)
{
	this->startingPoint = point;
}

glm::vec2 Player::GetStartingPoint(void) const
{
	return this->startingPoint;
}

const std::unique_ptr<ObjsCollection>& Player::GetObjsCollection(void) const
{
	return this->playerObjs;
}
