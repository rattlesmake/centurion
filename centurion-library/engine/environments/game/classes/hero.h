/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/game/classes/objectsSet/army.h>
#include <environments/game/classes/unit.h>
#include <environments/game/classes/objectsAttributes/heroAttributes.h>


class Hero : public Unit, public HeroAttributes
{
public:
	#pragma region Constructors and destructor and operators::
	Hero(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	Hero(const Hero& other) = delete;
	virtual ~Hero(void);
	Hero& operator=(const Hero& other) = delete;
	#pragma endregion


	#pragma region To scripts members:
	[[nodiscard]] std::shared_ptr<Army> GetArmy(void) const;

	/// <summary>
	/// This funtion checks if the curent hero leads a specific unit.
	/// </summary>
	/// <param name="unit">The unit.</param>
	/// <returns>True if the hero leads the unit; false otherwise</returns>
	[[nodiscard]] bool LeadsUnit(const std::shared_ptr<Unit>& uunit) const;

	/// <summary>
	/// Placing this function before a generic GoTo method, the units assigned to the hero will move keeping the formation.
	/// Conseguently, unit will assume a correct position than their hero and other units of the hero.
	/// </summary>
	void KeepFormationMoving(void);
	#pragma endregion


	#pragma region Inherited methods:
	virtual void GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const override;
	#pragma endregion
protected:
	#pragma region Inherited methods:
	std::ostream& Serialize(std::ostream& out, const bool calledByChild) const override;

	/// <summary>
	/// This function prepares the current hero and its army to move.
	/// </summary>
	void InitializeMovement(const std::shared_ptr<Playable>& objSP, const std::shared_ptr<Point>& destHeroPt = nullptr) override;

	void ExecuteMethod(const std::string& method) override;
	#pragma endregion
private:
	std::shared_ptr<Army> army;

	std::string currentMethod;

	#pragma region Private methods:
	void CommonHeroConstructor(const classData_t& _objData);

	/// <summary>
	/// This functions sets the attributes of the current hero when it is being created. 
	/// The values of these attributes were previously read 
	/// from an XML file and they are now stored into a ClassesData::XMLClassData object.
	/// </summary>
	/// <param name="objData">The object in which are stored the attributes of the current hero.</param>
	virtual void SetAttrs(const classData_t& objData, gobjData_t* dataSource) override;
	#pragma endregion

	#pragma region Formation:
	struct FormationData
	{
		std::string name;
		std::string description;
		uint16_t unitPerRow = 0;
		uint16_t distBetweenUnits = 0;
		uint16_t heroOffsetX = 0;
		uint16_t heroOffsetY = 0;
		uint16_t rowOffsetX = 0;
		uint16_t rowOffsetY = 0;
		uint16_t bonusLevel = 0;
		uint16_t bonusAttack = 0;
		uint16_t bonusDefence = 0;
	};
	static std::unordered_map<std::string, FormationData> formations;
	static std::string firstFormationRead;
	static bool bFormationReadFromXml;

	bool bKeepFormation = false;
	std::string activeFormation;

	static void ReadFormationsFromXml(void);
	#pragma endregion
};
