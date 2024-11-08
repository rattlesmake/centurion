#include "Parabola.h"
#include <engine.h>

#include <xml_entity.h>
#include <math_utils.h>

#pragma region Constructors and destructor:
Parabola::Parabola(std::string& vfxClassName, std::shared_ptr<Unit> _thrower, std::shared_ptr<Playable> _target) :
	VFX(vfxClassName, VFXTypes::e_parabola, _thrower->GetPositionX(), _thrower->GetPositionY()),
	damageParams(_thrower->GetLevel(), _thrower->GetMinAttack(), _thrower->GetMaxAttack(), _thrower->GetDamageType()),
	startPoint(_thrower->GetPositionX(), _thrower->GetPositionY()),
	destPoint(_target->GetPositionX(), _target->GetPositionY()),
	target(std::move(_target)), thrower(std::move(_thrower))
{
	this->height = 100; //The height of the parabola. MAYBE this value should be set using startPos and endPos.
	this->destTime = this->GetEntity().get_anim_total_duration(this->GetCurrentState(), this->GetCurrentAnim()) / 60.f; // TODO?
}

Parabola::~Parabola(void)
{
	if (this->bArrived == true && this->target.expired() == false)
	{
		auto tgSP = this->target.lock();
		//The damage applied will use the information that the thrower had at the time of the throw.
		Unit::ComputeAndApplyDamage(this->damageParams, this->thrower, (*tgSP));
	}
}
#pragma endregion

#pragma region Static members:
void Parabola::CreateParabola(const std::shared_ptr<Unit>& thrower, const std::shared_ptr<Playable>& target, std::string vfxClassName)
{
	assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match);
	std::unique_ptr<VFX> vfx{ new Parabola(vfxClassName, thrower, target) };
	Engine::GetInstance().GetEnvironment()->AsMatch()->GetVFXCollection().AddVFX(std::move(vfx));
}
#pragma endregion

#pragma region Inherited methods:
void Parabola::ApplyGameLogics(void)
{
	this->SetPosition(this->GetNextPoint());
	//Set direction
	const float angle = Math::GetViewAngle(this->startPoint.x, this->startPoint.y, this->destPoint.x, this->destPoint.y);
	this->SetCurrentDirection(angle);

	if (this->bArrived == true)
	{
		this->bToErase = true;
	}

	VFX::ApplyGameLogics();
}
#pragma endregion

#pragma region Private members:
Point Parabola::GetNextPoint(void)
{
	Point newPos;

	//Get time:
	double currentTime = 0;
	if (this->startMovementTime != -1.0f)
	{
		currentTime = Engine::GetGameTime().GetTotalSeconds();
	}
	else
	{
		this->startMovementTime = Engine::GetGameTime().GetTotalSeconds();
		currentTime = this->startMovementTime;
	}

	const double deltaT = (currentTime - this->startMovementTime);
	if (deltaT >= destTime)
	{
		this->bArrived = true;
		newPos = this->destPoint;
	}
	else
	{
		//Compute next point:
		double interpolate = 0;
		interpolate += deltaT;
		interpolate = fmod(interpolate, destTime);
		newPos = this->ComputeNextPoint(interpolate / destTime);
	}
	return newPos;
}

Point Parabola::ComputeNextPoint(const double time)
{
	const double f_t = -4.0f * this->height * time * time + 4.0f * this->height * time;

	//Update destination point.
	//It's necessary if the target is moving.
	//If the target is dead, destination point remains unchanged.
	if (this->target.expired() == false)
	{
		auto targetPoint = this->target.lock()->GetPosition();
		this->destPoint = Point{ targetPoint->x, targetPoint->y };
	}

	//Linear interpolation
	const double X = std::lerp((double)this->startPoint.x, (double)this->destPoint.x, time);
	const double Y = std::lerp((double)this->startPoint.y, (double)this->destPoint.y, time);
	//const double Z = std::lerp((double)this->startPoint.z, (double)this->destPoint.z, time);

	return Point(static_cast<uint32_t>(X), static_cast<uint32_t>(f_t + Y));
}
#pragma endregion
