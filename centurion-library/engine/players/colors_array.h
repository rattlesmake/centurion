/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

#include "color.h"

#ifndef COLORS_ARRAY_SIZE
#define COLORS_ARRAY_SIZE 16
#endif

class ColorsArray
{
public:
	explicit ColorsArray(tinyxml2::XMLElement* el);

	[[nodiscard]] Color& GetDefaultColor(const uint8_t idx);
private:
	std::array<Color, COLORS_ARRAY_SIZE> defaultColors;
	Color BLACK;
};