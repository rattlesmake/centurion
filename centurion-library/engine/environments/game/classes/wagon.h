/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <environments/game/classes/unit.h>

#ifndef MAX_GOLD_CAPACITY
#define MAX_GOLD_CAPACITY 10000
#endif

#ifndef MAX_FOOD_CAPACITY
#define MAX_FOOD_CAPACITY 10000
#endif

#ifndef MAX_LOYALTY_VALUE
#define MAX_LOYALTY_VALUE 10
#endif

//TODO
class Wagon : public Unit
{
public:
	#pragma region Constructors and destructor and operators:
	Wagon(const uint8_t _player, glm::vec3 _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	Wagon(const Wagon& other) = delete;
	virtual ~Wagon(void);
	Wagon& operator=(const Wagon& other) = delete;
	#pragma endregion

	#pragma region To scripts Members
	[[nodiscard]] uint16_t GetCarriedGold(void) const noexcept;
	[[nodiscard]] uint16_t GetCarriedFood(void) const noexcept;
	#pragma region

	#pragma region Inherited methods:
	virtual void GetBinRepresentation(std::vector<byte_t>& data, const bool calledByChild) const override;
	#pragma endregion
protected:
	#pragma region Inherited methods:
	std::ostream& Serialize(std::ostream& out, const bool calledByChild) const override;
	void SetAttrs(const classData_t& objData, gobjData_t* dataSource) override;
	#pragma endregion
private:
	#pragma region Attributes:
	uint16_t goldCarried = 0;
	uint16_t foodCarried = 0;
	#pragma endregion
	uint16_t loyalty = MAX_LOYALTY_VALUE;

	#pragma region Private methods:
	/// <summary>
	/// This functions checks if the values of the attributes read are valid.
	/// </summary>
	void CheckAttributesValues(void);
	#pragma endregion
};
