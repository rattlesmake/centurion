/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "objectPoint.h"
#include <string>
#include <memory>

class GObject;
class Playable;

/// <summary>
/// Target class = Playable GObject + Point
/// </summary>
class Target
{
public:
	Target(const Target& other) = delete;
	Target& operator=(const Target& other) = delete;
	[[nodiscard]] static std::shared_ptr<Target> CreateTarget(void);
	[[nodiscard]] static std::shared_ptr<Target> CreateTarget(std::weak_ptr<Playable> tg, const std::shared_ptr<Point> pt);

	#pragma region To Script methods:
	[[nodiscard]] std::shared_ptr<Playable>GetObject(void) const;
	[[nodiscard]] std::shared_ptr<Point> GetPoint(void) const;
	#pragma endregion

	[[nodiscard]] uint16_t GetMinDistanceRequiredFromTarget(void) const;
	void Set(std::weak_ptr<Playable> tg, const std::shared_ptr<Point> pt);
	void SetObject(std::weak_ptr<Playable> tg);
	void SetPoint(std::shared_ptr<Point> pt);
	void SetMinDistanceRequiredFromTarget(const uint16_t _minDistanceRequiredFromTarget);
	[[nodiscard]] bool IsEmptyTarget(void) const;

	void GetBinRepresentation(std::vector<uint8_t>& data) const;
	void InitByBinData(std::vector<uint8_t>& data, uint32_t& offset, const std::unordered_map<uniqueID_t, std::weak_ptr<GObject>>& objsMap);
private:
	Target(void);
	std::weak_ptr<Playable> objTarget;
	std::shared_ptr<Point> point;

	//The minimum distance the GObject must be from its target
	uint16_t minDistanceRequiredFromTarget = 0;
};
