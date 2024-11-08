/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include "vfx.h"
#include <environments/game/classes/unit.h>

class Parabola : public VFX
{
public:
	Parabola(const Parabola& other) = delete;
	Parabola& operator=(const Parabola& other) = delete;
	virtual ~Parabola(void);

	static void CreateParabola(const std::shared_ptr<Unit>& thrower, const std::shared_ptr<Playable>& target, std::string vfxClassName);

	void ApplyGameLogics(void) override;
private:
	Parabola(std::string& vfxClassName, std::shared_ptr<Unit> _thrower, std::shared_ptr<Playable> _target);

	[[nodiscard]] Point GetNextPoint(void);
	[[nodiscard]] Point ComputeNextPoint(const double time);

	AttackerDamageParams_s damageParams;

	std::weak_ptr<Unit> thrower;
	std::weak_ptr<Playable> target;
	Point startPoint;
	Point destPoint;

	double destTime = 0.f;
	double startMovementTime = -1.0f; //Movement isn't started yet.
	double height = 0.f;
	bool bArrived = false;
};
