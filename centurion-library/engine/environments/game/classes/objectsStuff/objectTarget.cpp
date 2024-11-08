#include "objectTarget.h"
#include <environments/game/classes/playable.h>

#include <bin_data_interpreter.h>

Target::Target(void) 
{
	this->point = std::shared_ptr<Point>(new Point());
}

std::shared_ptr<Target> Target::CreateTarget(void)
{
	return std::shared_ptr<Target>(new Target());
}

std::shared_ptr<Target> Target::CreateTarget(std::weak_ptr<Playable> tg, const std::shared_ptr<Point> pt)
{
	std::shared_ptr<Target> newTarget{ new Target() };
	newTarget->Set(tg, pt);
	return newTarget;
}

uint16_t Target::GetMinDistanceRequiredFromTarget(void) const
{
	return this->minDistanceRequiredFromTarget;
}

void Target::Set(std::weak_ptr<Playable> tg, std::shared_ptr<Point> pt)
{
	this->point = std::move(pt);
	this->objTarget = std::move(tg);
}

void Target::SetObject(std::weak_ptr<Playable> tg)
{
	this->objTarget = std::move(tg);
}

std::shared_ptr<Playable> Target::GetObject(void) const
{
	return (this->objTarget.expired() == false) ? this->objTarget.lock() : std::shared_ptr<Playable>();
}

void Target::SetPoint(std::shared_ptr<Point> pt)
{
	//ASSERTION (to verify next): I set set a point only if I don't have an object as target (in this case, in fact, the target point is the object point).
	assert(this->objTarget.expired() == true);
	this->point = std::move(pt);
}

void Target::SetMinDistanceRequiredFromTarget(const uint16_t _minDistanceRequiredFromTarget)
{
	this->minDistanceRequiredFromTarget = _minDistanceRequiredFromTarget;
}

std::shared_ptr<Point> Target::GetPoint(void) const
{
	return (this->objTarget.expired() == true) ? this->point : this->objTarget.lock()->GetPosition();
}

bool Target::IsEmptyTarget(void) const
{
	//TODO
	return true;
}

void Target::GetBinRepresentation(std::vector<uint8_t>& data) const
{
	//Save min distance required from target
	BinaryDataInterpreter::PushUInt16(data, this->minDistanceRequiredFromTarget);

	//If the target has a PlayableGObject, save its unique ID, otherwise save 0 (that's invalid ID)
	if (this->objTarget.expired() == false)
		BinaryDataInterpreter::PushUInt32(data, this->objTarget.lock()->GetUniqueID());
	else
	{
		BinaryDataInterpreter::PushUInt32(data, 0);
		//Save point bin representation
		this->point->GetBinRepresentation(data);
	}
}

void Target::InitByBinData(std::vector<uint8_t>& data, uint32_t& offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap)
{
	//Load min distance required from target
	this->minDistanceRequiredFromTarget = BinaryDataInterpreter::ExtractUInt16(data, offset);

	//Load uniqueID of the PlayableGObject belonging to the target
	const uniqueID_t ID = BinaryDataInterpreter::ExtractUInt32(data, offset);
	if (ID != 0)
	{
		//If here, the target had a PlayableGObject, so get it.
		if (objsMap.contains(ID) == false || objsMap.at(ID).expired() == true || objsMap.at(ID).lock()->IsPlayableGObject() == false)
			throw BinaryDeserializerException("Invalid target ID (" + std::to_string(ID) + ")");
		this->objTarget = std::static_pointer_cast<Playable>(objsMap.at(ID).lock());
		assert(this->objTarget.expired() == false);
	}
	else //== 0 --> target is simply a point
	{
		//Load point bin representation
		this->point->InitByBinData(data, offset);
	}
}
