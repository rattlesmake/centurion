/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cctype>
#include <memory>
#include <string>

#include <centurion_typedef.hpp>

class GObjectAttributes
{
public:
	GObjectAttributes(const GObjectAttributes& other) = delete;
	GObjectAttributes& operator=(const GObjectAttributes& other) = delete;

	[[nodiscard]] bool IsWaterObject(void) const noexcept;
	[[nodiscard]] bool IsAlwaysVisibleInGameMinimap(void) const noexcept;

	[[nodiscard]] std::string GetRaceName(void) const noexcept;
	
protected:
	GObjectAttributes(void) = default;
	void SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool _temporary);
	void GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const;
private:
	bool bIsWaterObject = false;
	bool bAlwaysVisibleInGameMinimap = false;
	std::string raceName;

#pragma region THEY NEEDS TO BE REMOVED WHEN "PLAYABLE" CLASS IS IMPLEMENTED!
	uint16_t sight = 0;
	std::string iconName;
#pragma endregion
};
