#include "settlement.h"

#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/unit.h>
#include <dialogWindows.h>

#include <random>
#include <bin_data_interpreter.h>

#include <math_utils.h>

// Assets
#include <xml_class.h>


#pragma region Constructors and destructor
Settlement::Settlement(std::weak_ptr<Building> _firstBuilding, std::weak_ptr<PlayersArray> _playersArrayWP, classData_t _settlementData, gobjData_t* dataSource)
	: buildingsOfSettlement(new ObjsList(false)), xmlData(std::move(_settlementData)),
	firstBuilding(std::move(_firstBuilding)), playersArrayWP(std::move(_playersArrayWP))
{
	assert(this->firstBuilding.expired() == false && this->firstBuilding.lock()->IsCentralBuilding() == true);

	this->id = this->firstBuilding.lock()->GetUniqueID();
	this->SetSettlementAttributes(dataSource);
	this->UpdateHitbox();
}

Settlement::~Settlement(void)
{
}
#pragma endregion

#pragma region Operators:
std::ostream& operator<<(std::ostream& out, const Settlement& settlement)
{
	const string nTabs(SettlementsCollection::GetTabs(), '\t');
	out << nTabs << "<settlement>\n";
	out << nTabs << "\t<idName>" << settlement.idName << "</idName>\n";
	settlement.SettlementAttributes::Serialize(out, nTabs);
	out << nTabs << "</settlement>";
	return out;
}
#pragma endregion


#pragma region Static members:
#pragma endregion


#pragma region To Script methods:
std::string Settlement::GetSettlementName(void) const
{
	return this->idName;
}

bool Settlement::SetSettlementName(std::string _settlementName)
{
	std::trim(_settlementName);
	if (this->idName == _settlementName)
		return true;

	SettlementsCollection& settlementsCollection = this->playersArrayWP.lock()->GetSettlementsCollectionRef();
	// Checks if a name belongs to an other existing settlement
	if (settlementsCollection.GetSettlementByName(_settlementName))
	{
		if (this->bLoading == true)
			throw BinaryDeserializerException("Settlement::SetSettlementName -> duplicated name " + std::move(_settlementName)); //Settlement name READ FROM FILE is not valid!
		gui::NewInfoWindow("e_text_sett_name_already_used", IEnvironment::Environments::e_editor);
		return false;
	}

	// Current ID name is now available
	if (this->idName.empty() == false)
		settlementsCollection.RemoveName(this->idName);
	// New ID name isn't available anymore.
	if (_settlementName.empty() == false)
		settlementsCollection.InsertName(_settlementName, this->id);
	// Assigning name 
	this->idName = std::move(_settlementName);
	return true;
}

std::shared_ptr<Building> Settlement::GetFirstBuilding(void) const
{
	return this->firstBuilding.lock();
}

std::shared_ptr<ObjsList> Settlement::GetBuildings(void) const
{
	return this->buildingsOfSettlement;
}

uint8_t Settlement::GetPlayer(void) const
{
	return (this->firstBuilding.expired() == false) ? this->firstBuilding.lock()->GetPlayer() : PlayersArray::UNDEFINED_PLAYER_INDEX;
}

bool Settlement::SetPlayer(const uint8_t _player)
{
	if (_player == this->player || _player == PlayersArray::UNDEFINED_PLAYER_INDEX || _player > PlayersArray::NUMBER_OF_PLAYERS)
		return false;

	this->player = _player;
	ObjsList::objs_const_iterator endIt = this->buildingsOfSettlement->cend();
	for (auto buildingIt = this->buildingsOfSettlement->cbegin(); buildingIt != endIt; buildingIt++)
	{
		const std::shared_ptr<Playable> building = std::static_pointer_cast<Playable>((*buildingIt).lock());
		// N.B.: Will be invoked Playable::SetPlayer and not Building::SetPlayer :)
		building->Playable::SetPlayer(_player);
	}
	return true;
}

uint32_t Settlement::GetNumberOfBuildings(void) const
{
	return this->buildingsOfSettlement->Count();
}
#pragma endregion

uint32_t Settlement::GetId(void) const
{
	return this->id;
}

void Settlement::AddBuildingToSettlement(const std::shared_ptr<Building>& building)
{
	// Assertion: I shouldn't add a building if this settlement already has it.
	assert(this->buildingsOfSettlement->Contains(building) == false);
	// Assertion: the building MUST have a settlement.
	assert(building->GetSettlement());
	// Assertion: The settlement of the building MUST be this.
	assert(building->GetSettlement()->GetId() == this->id);

	this->buildingsOfSettlement->Insert(building);
	this->UpdateHitbox();
}

void Settlement::RemoveBuildingFromSettlement(const std::shared_ptr<Building>& building)
{
	// Assertion: I mustn't remove a building if this settlement doesn't have it.
	assert(this->buildingsOfSettlement->Contains(building) == true);
	// Assertion: the building MUST have a settlement.
	assert(building->GetSettlement());
	// Assertion: The settlement of the building MUST be this.
	assert(building->GetSettlement()->GetId() == this->id);

	this->buildingsOfSettlement->GetOut(building);
	this->UpdateHitbox();

	// After removal, if settlements has zero building then settlement is going to be destoyed so remove it from settlement collection 
	// (IT IS IMPORTANT TO DO SINCE IN THIS CASE WE ARE ALWAYS IN THE SAME ENVIRONMENT)
	if (this->buildingsOfSettlement->IsEmpty() == true)
		this->playersArrayWP.lock()->GetSettlementsCollectionRef().Remove((*this));
}

void Settlement::RenderHitbox(void)
{
	this->hitbox.Render();
}

void Settlement::UpdateHitbox(void)
{
	if (this->GetNumberOfBuildings() == 0)
		return;

	auto fBuilding = this->firstBuilding.lock();
	auto hbCenter = glm::ivec2();
	auto hbSize = glm::ivec2();

	if (this->GetNumberOfBuildings() == 1)
	{
		hbCenter = glm::ivec2(fBuilding->GetPositionX(), fBuilding->GetPositionY());
		hbSize = glm::ivec2(MIN_SETTL_RADIUS, MIN_SETTL_RADIUS);
	}
	else
	{
		auto minPosX = fBuilding->GetPositionX();
		auto maxPosX = fBuilding->GetPositionX();
		auto minPosY = fBuilding->GetPositionY();
		auto maxPosY = fBuilding->GetPositionY();
		ObjsList::objs_const_iterator endIt = this->buildingsOfSettlement->cend();
		for (auto buildingIt = this->buildingsOfSettlement->begin(); buildingIt != endIt; buildingIt++)
		{
			auto& building = (*buildingIt);
			auto x = building.lock()->GetPositionX();
			auto y = building.lock()->GetPositionY();
			minPosX = std::min(minPosX, x);
			maxPosX = std::max(maxPosX, x);
			minPosY = std::min(minPosY, y);
			maxPosY = std::max(maxPosY, y);
		}
		hbCenter = glm::ivec2((maxPosX + minPosX) / 2, (maxPosY + minPosY) / 2);
		hbSize = glm::ivec2(maxPosX - minPosX + SETTL_HITBOX_SIZE, maxPosY - minPosY + SETTL_HITBOX_SIZE);
	}

	// update hitbox
	this->hitbox = RectangularArea(hbCenter, hbSize.x, hbSize.y, AreaOrigin::Center);
}

const RectangularArea& Settlement::GetHitbox(void) const
{
	return this->hitbox;
}

int Settlement::GetDefaultIntAttributesValue(const std::string propertyName)
{
	// Assertion: class must exist.
	assert(this->xmlData);

	std::optional<int> iProperty = assets::xml_class::try_parse_integer(this->xmlData->get_attributes(), propertyName);
	return (iProperty.has_value() ? iProperty.value() : -1);
}

bool Settlement::CheckIfAnyBuildingIsNear(const std::shared_ptr<Building>& building) const
{
	bool bAnyBuildingIsNear = false;
	ObjsList::objs_const_iterator endIt = this->buildingsOfSettlement->cend();
	for (auto buildingIt = this->buildingsOfSettlement->begin(); buildingIt != endIt; buildingIt++)
	{
		const std::shared_ptr<Building> bld = std::static_pointer_cast<Building>((*buildingIt).lock());
		if (building->GetUniqueID() == bld->GetUniqueID()) 
			continue;

		const uint32_t dist = static_cast<uint32_t>(Math::euclidean_distance(bld->GetPositionX(), bld->GetPositionY(), building->GetPositionX(), building->GetPositionY()));
		if (dist <= MAX_DISTANCE_BETWEEN_TWO_BUILDINGS) 
		{
			bAnyBuildingIsNear = true;
			break;
		}
	}
	return bAnyBuildingIsNear;
}

void Settlement::AddDefender(const Unit& unit)
{
	this->defenders.insert(unit.GetUniqueID());
}

void Settlement::RemoveDefender(const Unit& unit)
{
	this->defenders.erase(unit.GetUniqueID());
}

void Settlement::AddBesieger(const Unit& unit)
{
	const uniqueID_t unitID = unit.GetUniqueID();
	const uint8_t playerIndex = unit.GetPlayer() - 1;
	if (this->besiegers[playerIndex].contains(unitID) == false)
	{
		this->besiegers[playerIndex].insert(unitID);
		this->numberOfBesiegers += 1;
		if (this->numberOfBesiegers == 1)
		{
			// The besieger is the first. Now siege can start => Settlement --> first reaction: shock!
			this->nextLoyaltyChangementInstant = Engine::GetInstance().GetGameTime().GetTotalSeconds();
		}
	}
}

void Settlement::RemoveBesieger(const Unit& unit)
{
	const uniqueID_t unitID = unit.GetUniqueID();
	const uint8_t playerIndex = unit.GetPlayer() - 1;
	if (this->besiegers[playerIndex].contains(unitID) == true)
	{
		this->numberOfBesiegers -= 1;
		this->besiegers[playerIndex].erase(unitID);
	}
}

void Settlement::HandleLoyalty(void)
{
	const uint8_t currentLoyalty = this->GetLoyalty();
	const size_t defendersNumber = this->defenders.size();

	if (this->numberOfBesiegers >= 1 && this->numberOfBesiegers > defendersNumber)
	{
		// Settlement is under siege (i.e. numberOfBesiegers >= 1) and besegiers are outnumber the defenders (i.e. numberOfBesiegers > defendersNumber). 
		auto currentInstant = Engine::GetInstance().GetGameTime().GetTotalSeconds();
		if (islessequal(this->nextLoyaltyChangementInstant + LOYALTY_LOSS_RATE, currentInstant) == true)
		{
			// Settlement is going to lose loyalty points.
			if (currentLoyalty > 0)
			{
				// Calculate how much loyalty the settlement should lose
				const size_t loyaltyVariation = this->numberOfBesiegers - defendersNumber;
				// Calculate how much loyalty the settlement can lose in order to not go below zero
				const uint8_t maxLoyaltyLoss = (currentLoyalty >= MAX_LOYALTY_VARIATION) ? MAX_LOYALTY_VARIATION : currentLoyalty;
				// Decrease loyalty (and also when will be applied the next loyalty changement).
				this->SetLoyalty(currentLoyalty - ((loyaltyVariation <= maxLoyaltyLoss) ? static_cast<uint8_t>(loyaltyVariation) : maxLoyaltyLoss));
			}
			else  // Settlement has been conquered 
			{
				const uint8_t newPlayer = this->DefineNewPlayer();
				this->SetPlayer(newPlayer);

				// Sets a time that indicates from when loyalty will start to grow again (having been conquered the settlement).
				this->nextLoyaltyChangementInstant = currentInstant;
			}
		}
	}
	else if (currentLoyalty < MAX_LOYALTY_VALUE)
	{
		// Settlement isn't under siege or defenders are outnumber the besegiers
		auto currentInstant = Engine::GetInstance().GetGameTime().GetTotalSeconds();
		if (islessequal(this->nextLoyaltyChangementInstant + static_cast<uint64_t>(LOYALTY_GAIN_RATE), currentInstant) == true)
		{
			// Calculate how much loyalty the settlement should gain
			const size_t loyaltyVariation = (defendersNumber - this->numberOfBesiegers) + 1; //Adding one to the difference allows the settlement to still get a loyalty point even in case of a tie in the number of units
			// Settlement recovers a point of loyalty (and also when will be applied the next loyalty changement).
			this->SetLoyalty(currentLoyalty + ((loyaltyVariation >= MAX_LOYALTY_VARIATION) ? MAX_LOYALTY_VARIATION : static_cast<uint8_t>(loyaltyVariation)));
		}
	}
}

void Settlement::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	this->SettlementAttributes::GetXmlAttributesAsBinaryData(data);

	// If you change saving order here, please go to Settlement::SetSettlementProperties and impose the same loading order!
	BinaryDataInterpreter::PushString(data, this->idName);
}

#pragma region Private members:
void Settlement::SetSettlementAttributes(gobjData_t* dataSource)
{
	this->bLoading = (dataSource != nullptr);  // If true, settlement is being loaded from a file otherwise it's being created manually (e.g. from a barracks or placing it in the editor).

	this->SettlementAttributes::SetAttributes(this->xmlData, dataSource);

	std::optional<int> iProperty = 0;
	if (dataSource != nullptr)
	{
		if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
		{
			tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
			this->SetSettlementName(tinyxml2::TryParseFirstChildStrContent(xml, "idName"));
		}
		else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
		{
			gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
			// If you change loading order here, please go to Settlement::GetBinRepresentation and impose the same saving order!
			this->SetSettlementName(BinaryDataInterpreter::ExtractString((*binData.first), binData.second));
		}
	}

	// Always equals to false, at the end of this method
	this->bLoading = false;
}

const uint8_t Settlement::DefineNewPlayer(void) const
{
	/// Choice of player to assign the settlement to.
	/// The player having the highest number of besiegers will be selected.
	/// If there is a tie between some players, one will be chosen at random.

	// Compute the max number of units and, for each different number, associates a list of players with it.
	size_t maxUnitsNumber = 0;
	std::unordered_map<size_t, std::vector<uint8_t>> indexesByUnitsNumber;
	for (uint8_t i = 0; i < static_cast<uint8_t>(this->besiegers.size()); ++i)
	{
		const size_t nUnits = this->besiegers[i].size();
		if (nUnits == 0)
			continue;
		if (nUnits > maxUnitsNumber)
			maxUnitsNumber = nUnits;
		indexesByUnitsNumber[nUnits].push_back(i);
	}
	assert(maxUnitsNumber > 0);

	// Get the index of the player having the max number of units.
	uint8_t indexOfMax = 0;
	if (indexesByUnitsNumber.at(maxUnitsNumber).size() == 1)
	{
		// There is only one player with the highest number of units
		indexOfMax = indexesByUnitsNumber.at(maxUnitsNumber).back();
	}
	else
	{
		// More players (i.e. indexesByUnitsNumber.at(maxUnitsNumber).size()) have the same max number of units. One will be chosen randomly.
		std::random_device rd;  // Will be used to obtain a seed for the random number engine.
		std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd().
		std::uniform_int_distribution<> distrib(0, static_cast<uint32_t>(indexesByUnitsNumber.at(maxUnitsNumber).size()) - 1);
		// Use distrib to transform the random unsigned int generated by gen into an int in [0, indexesByUnitsNumber.at(maxUnitsNumber).size() - 1].
		const int randomChoose = distrib(gen);
		// Get the index using the random number.
		indexOfMax = indexesByUnitsNumber.at(maxUnitsNumber).at(randomChoose);
	}

	// Return the player ID adding one to the computed index.
	return indexOfMax + 1;
}
#pragma endregion
