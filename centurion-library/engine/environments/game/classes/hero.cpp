#include "hero.h"
#include "building.h"

#include <algorithm>
#include <math.h>
#include <engine.h>
#include <services/logservice.h>
#include <zipservice.h>
#include <fileservice.h>

#include <bin_data_interpreter.h>
#include <math_utils.h>


// Assets
#include <xml_class.h>


#pragma region Static variable initialization
std::string Hero::firstFormationRead{ "" };
bool Hero::bFormationReadFromXml = false;
std::unordered_map<std::string, Hero::FormationData> Hero::formations;
#pragma endregion


#pragma region Constructors and destructor:
Hero::Hero(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource)
	: Unit(_player, std::move(_pos), _bTemporary, _objData, surface, dataSource)
{
	this->CommonHeroConstructor(_objData);	
}

Hero::~Hero(void)
{
}
#pragma endregion


#pragma region To scripts members:
std::shared_ptr<Army> Hero::GetArmy(void) const
{
	return this->army;
}

bool Hero::LeadsUnit(const std::shared_ptr<Unit>& unit) const
{
	if (!unit)
		return false;
	return this->army->Contains(unit);
}

void Hero::KeepFormationMoving(void)
{
	this->bKeepFormation = true;
}
#pragma endregion


#pragma region Inherited methods:
void Hero::GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const
{
	// Firstly, calls always GObject::GetBinSignature
	if (calledByChild == false)
		this->GObject::GetBinSignature(data);

	// If the units has some unit attached, notify it to PlayersArray in order to save its army later.
	// In any case, save this info
	const bool bHasNotEmptyArmy = (this->army->GetTroops()->Count() >= 1);
	if (bHasNotEmptyArmy == true)
		GObject::playersWeakRef.lock()->AddHeroHavingNotEmptyArmy((*this));
	BinaryDataInterpreter::PushBoolean(data, bHasNotEmptyArmy);

	BinaryDataInterpreter::PushBoolean(data, this->bKeepFormation);
	BinaryDataInterpreter::PushString(data, this->activeFormation);

	// Call parent's method to get binary representation
	this->Unit::GetBinRepresentation(data, true);


	// As a last thing, calls always method Playable::GetBinRepresentation
	if (calledByChild == false)
		this->Playable::GetBinRepresentation(data, calledByChild);
}

std::ostream& Hero::Serialize(std::ostream& out, const bool calledByChild) const
{
	const std::string tabs(GObject::nTabs, '\t');
	if (calledByChild == false)
		out << tabs 
			<< "<hero class=\"" << this->GetClassName() << "\" "
			<< "id=\"" << std::to_string(this->GetUniqueID()) << "\" "
			<< "xPos=\"" << std::to_string(this->GetPositionX()) << "\" "
			<< "yPos=\"" << std::to_string(this->GetPositionY()) << "\""
			<< ">\n";
	this->Unit::Serialize(out, true);
	if (calledByChild == false)
		out << tabs << "</hero>";
	return out;
}

void Hero::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	const auto& attributesMap = objData->get_attributes();

	if (dataSource != nullptr && std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		// Read if building saved into binary file had a not empty army
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		const bool bHasNotEmptyArmy = BinaryDataInterpreter::ExtractBoolean((*binData.first), binData.second);
		if (bHasNotEmptyArmy)
			this->GObject::playersWeakRef.lock()->AddHeroHavingNotEmptyArmy((*this));
		this->bKeepFormation = BinaryDataInterpreter::ExtractBoolean((*binData.first), binData.second);
		this->activeFormation = BinaryDataInterpreter::ExtractString((*binData.first), binData.second);
	}

	this->HeroAttributes::SetAttributes(objData, this->bIsTemporary);
	this->Unit::SetAttrs(objData, dataSource);

	if (this->bIsTemporary == true)
		return;

	// TryParseFloat, TryParseInteger, TryParseString, TryParseBool
	std::optional<int> iAttribute = 0;

	// Integer attributes:
	iAttribute = assets::xml_class::try_parse_integer(attributesMap, "maxArmySize");

	// ASSERTION: hero must be able to have an army (otherwise, it isn't a real hero asd)
	assert(iAttribute > 0);
	this->army = Army::CreateArmy(std::static_pointer_cast<Hero>(this->me.lock()), iAttribute.value());
}

void Hero::InitializeMovement(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Point>& destHeroPt)
{
	if (this->bKeepFormation == true && this->activeFormation != "" && this->army->GetTroops()->Count() >= 1)
	{
		// Change destination point of each units assigned to the hero in order to keep the formation during the movement.
		// Remember that this->bKeepFormation = true should be setted calling a proper method inside python scripts.
		if (this->IsEntering() == false)
		{
			// WARNING: In this scope is important to don't use directly objects having class Point since they have unsigned coordinates
			// and some of the following calculations may instead produce negative numbers

			// As discussed above, converting hero's points from unit32_t to double 
			const double startHeroX = static_cast<double>(this->GetPositionX());
			const double startHeroY = static_cast<double>(this->GetPositionY());
			const double destHeroX = static_cast<double>(destHeroPt->x);
			const double destHeroY = static_cast<double>(destHeroPt->y);

			// Get the current formation active.
			Hero::FormationData currentFormationData = Hero::formations.at(this->activeFormation);
			/*
			* Default params
			FormationData currentFormationData;
			currentFormationData.heroOffsetX = 0;
			currentFormationData.heroOffsetY = 80;
			currentFormationData.unitPerRow = 8;
			currentFormationData.rowOffsetY = 50;
			currentFormationData.distBetweenUnits = 150
			*/

			// Calculation of the angle of shifting of the hero
			double heroAngleRotation = static_cast<double>(atan2(destHeroY - startHeroY, destHeroX - startHeroX)) * Math::Radiant;
			if (heroAngleRotation < 0)
				heroAngleRotation += 360.0f;
			// Calculation of the unit angle rotation respect to the hero final position.
			// A unit rotates by 270 degree counterclockwise respect the angle of shifiting of the hero.
			const double sinAngleUnit = sin((270.0 + heroAngleRotation) * Math::ReciprocalRadiant);
			const double cosAngleUnit = cos((270.0 + heroAngleRotation) * Math::ReciprocalRadiant);

			// Relative positions between units (N.B.: this part will be modified to have a generic formation)
			uint16_t unitPerCurrentRow = 0;
			double xOffset = currentFormationData.heroOffsetX; // = 0 in this case.
			double yOffset = currentFormationData.heroOffsetY;

			// Where a unit should go if you had a rigid translation respect to the hero.
			double xPosUnit = 0;
			double yPosUnit = 0;
			// Where a unit will be go concretely even considering the direction of movement.
			double xPosUnitFinal = 0;
			double yPosUnitFinal = 0;

			// Scans the units in order of formation priority
			auto& formPriority = this->army->GetArmyByFormationPriorityCRef();
			for (const auto& [priorioty, units] : formPriority)
			{
				// Calculate destination point of each unit with the same formation priority.
				for (auto& it_unit : units)
				{
					std::shared_ptr<Unit> unitSP = std::static_pointer_cast<Unit>(it_unit.lock());
					Unit& unit = (*unitSP);
					assert(unitSP && unitSP->IsDead() == false);
					// Reset offsets if a row is full (N.B.: this part will be modified to have a generic formation)
					if (unitPerCurrentRow == currentFormationData.unitPerRow)
					{
						unitPerCurrentRow = 0;
						xOffset = 0;
						yOffset += currentFormationData.rowOffsetY;
					}

					// Determines the new location of the unit.
					xPosUnit = xOffset + destHeroX;
					yPosUnit = yOffset + destHeroY;
					// Formulas for rotation of a point with respect to a center (N.B.: the center is the destination pos of the hero).
					xPosUnitFinal = (xPosUnit - destHeroX) * cosAngleUnit - (yPosUnit - destHeroY) * sinAngleUnit + destHeroX;
					yPosUnitFinal = (xPosUnit - destHeroX) * sinAngleUnit + (yPosUnit - destHeroY) * cosAngleUnit + destHeroY;
					// std::cout << unitSP->GetUniqueID() << "(" << xPosUnitFinal << "," << yPosUnitFinal << ") ";
					// Assign a target to the unit starting from the target of the hero and changing the position using the previously calculated point.
					std::shared_ptr<Point> pt{ new Point{static_cast<uint32_t>(xPosUnitFinal),static_cast<uint32_t>(yPosUnitFinal) } };

					// The unit is going to move towards is destination points just computed
					const std::shared_ptr<Target> unitTarget = Target::CreateTarget(std::weak_ptr<Playable>(), pt);
					unit.SetCommandWithTarget("move", unitTarget);

					// Calculate offset of the new unit (N.B.: this part will be modified to have a generic formation).
					unitPerCurrentRow += 1;
					xOffset = (unitPerCurrentRow % 2 != 0) ? std::abs(xOffset) + currentFormationData.distBetweenUnits : xOffset * (-1);
				}
			}
		}
		else
		{
			// The army are going to enter in the same building in which the hero is entering
			const std::shared_ptr<Target> unitTarget = Target::CreateTarget(this->GetMyTarget()->GetObject(), this->GetMyTarget()->GetObject()->GetPosition());
			this->army->GetTroops()->SetCommandWithTarget("enter", unitTarget);
		}
	}

	// Finally, call the parent method (hero will be considered as a simple unit) in order to prepare unit to movement and in order to set destination point.
	Unit::InitializeMovement(objSP, destHeroPt);
}

void Hero::ExecuteMethod(const std::string& method)
{
	if (this->HasMethod(method) == false)
		return;
	this->bKeepFormation = false;
	this->currentMethod = method;
	// Execute the method for the hero calling its parent method (hero is considered as an unit).
	Unit::ExecuteMethod(method);
}
#pragma endregion

#pragma region Formations:
void Hero::ReadFormationsFromXml(void)
{
	std::string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "formations.xml");

	tinyxml2::XMLDocument xmlFile;
	tinyxml2::XMLError result = xmlFile.Parse(xmlText.c_str());
	assert(result == tinyxml2::XML_SUCCESS);
	if (result != tinyxml2::XML_SUCCESS)
		return;  // (???) TODO - Lanciare eccezione e log.
	tinyxml2::XMLElement* _formationXML = xmlFile.FirstChildElement("formations");
	if (_formationXML != nullptr)
	{
		for (tinyxml2::XMLElement* _it_form = _formationXML->FirstChildElement(); _it_form != nullptr; _it_form = _it_form->NextSiblingElement())
		{
			// Read formation
			FormationData formation;
			formation.name = tinyxml2::TryParseStrAttribute(_it_form, "name");
			if (formation.name.empty() == true)
			{
				// (???) Inserire logger che indica che la formazione non ha un id e quindi è stata saltata.
				continue;
			}
			formation.description = tinyxml2::TryParseFirstChildStrContent(_it_form, "description");
			formation.unitPerRow = tinyxml2::TryParseFirstChildIntContent(_it_form, "unitPerRow");
			formation.distBetweenUnits = tinyxml2::TryParseFirstChildIntContent(_it_form, "distBetweenUnits");
			formation.heroOffsetX = tinyxml2::TryParseFirstChildIntContent(_it_form, "heroOffsetX");
			formation.heroOffsetY = tinyxml2::TryParseFirstChildIntContent(_it_form, "heroOffsetY");
			formation.rowOffsetX = tinyxml2::TryParseFirstChildIntContent(_it_form, "rowOffsetX");
			formation.rowOffsetY = tinyxml2::TryParseFirstChildIntContent(_it_form, "rowOffsetY");
			formation.bonusLevel = tinyxml2::TryParseFirstChildIntContent(_it_form, "bonusLevel");
			formation.name = tinyxml2::TryParseStrAttribute(_it_form, "name");
			formation.bonusAttack = tinyxml2::TryParseFirstChildIntContent(_it_form, "bonusAttack");
			formation.bonusDefence = tinyxml2::TryParseFirstChildIntContent(_it_form, "bonusDefence");
			
			// Store formation
			if (Hero::formations.contains(formation.name) == false)
			{
				Hero::formations.insert({ formation.name, formation });
				if (Hero::firstFormationRead.empty() == true)
				{
					Hero::firstFormationRead = formation.name;
				}
			}
			else
			{
				;
				// (???) Logger che indica che la formazione e' ripetuta.
			}
		}
	}
}
#pragma endregion

#pragma region Private methods:
void Hero::CommonHeroConstructor(const classData_t& _objData)
{
	// If it's the first hero created and if it's a match, read all formations from XML
	if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match && Hero::bFormationReadFromXml == false)
	{
		Hero::ReadFormationsFromXml();
		Hero::bFormationReadFromXml = true;
	}

	// Set default formation:
	this->activeFormation = Hero::firstFormationRead;
	this->bKeepFormation = false;
}
#pragma endregion
