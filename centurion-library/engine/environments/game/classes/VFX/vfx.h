/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <environments/game/adventure/scenario/area.h>
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/VFX/vfxCollection.h>

namespace centurion
{
	namespace assets
	{
		class xml_entity;
	};
};

class VFX
{
public:
	VFX(const VFX& other) = delete;
	VFX& operator=(const VFX& other) = delete;
	virtual ~VFX(void);

	[[nodiscard]] VFXTypes GetType(void) const;
protected:
	VFX(std::string& _className, const VFXTypes _type, const uint32_t startX, const uint32_t startY);

	virtual void ApplyGameLogics(void);

	[[nodiscard]] const centurion::assets::xml_entity& GetEntity(void) const;
	[[nodiscard]] const std::string& GetCurrentAnim(void) const;
	[[nodiscard]] const std::string& GetCurrentState(void) const;
	void SetPosition(const uint32_t x, const uint32_t y);
	void SetPosition(const Point& pt);
	void SetCurrentDirection(const float angle);
	bool bToErase = false;
private:
	[[nodiscard]] bool IsInCameraViewport(void) const;

	Point position;
	RectangularArea HitBox;
	uint8_t currentFrame = 0;
	uint8_t currentDirection = 0;
	std::string currentAnim = "idle";
	std::string currentState = "default";
	uint32_t framesAnimVariation = 0;
	//std::shared_ptr<Entity> entity;
	std::shared_ptr<centurion::assets::xml_entity> xml_entity;
	VFXTypes type = VFXTypes::e_unknown;
	std::string className;

	friend VFXCollection;
};



//class VFX
//{
//public:
//
//
//	#pragma region Public static methods:
//
//	static void CreateVFX(std::string _className, );
//	#pragma endregion
//
//	~VFX(void);
//private:
//	#pragma region Private members:
//
//	[[nodiscard]] glm::vec3 ComputeNextPosition(void);
//	void SetPosition(const glm::vec3& pos);
//	void SetPlayer(void);
//	void ApplyDamage(void) const;
//	#pragma endregion
//
//	#pragma region Constructor and destructor
//	VFX(const std::string& _className, const glm::vec3& _pos, const Unit* const _creator, const uint64_t& _damage, const Unit* const _target);
//	#pragma endregion
//
//
//	uint8_t playerID = 0;
//	uint32_t damage = 0;
//	uint32_t creatorUniqueID = 0;
//	uint32_t targetUniqueID = 0;
//	glm::vec3 position;
//	glm::vec3 positionDest;
//	string className;
//};

