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

#ifndef MAX_SIGHT_VALUE
#define MAX_SIGHT_VALUE       3500
#endif


class PlayableAttributes
{
public:
	PlayableAttributes(const PlayableAttributes& other) = delete;
	PlayableAttributes& operator=(const PlayableAttributes& other) = delete;

	[[nodiscard]] uint16_t GetSight(void) const noexcept;
	void SetSight(const uint16_t _sight);

	[[nodiscard]] std::string GetSingularName(void) const noexcept;
	void SetSingularName(std::string _singularName);

	[[nodiscard]] std::string GetIconName(void);
	void SetIconName(std::string _iconName);
protected:
	PlayableAttributes(void) = default;

	[[nodiscard]] const std::string& GetSingularNameCRef(void) const noexcept;

	void SetAttributes(const classData_t& objData, gobjData_t* dataSource, const bool _temporary);
	void GetXmlAttributesAsBinaryData(std::vector<byte_t>& data) const;
private:
	uint16_t sight = 0;
	std::string singularName;
	std::string iconName;
};
