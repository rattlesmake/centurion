/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <optional>

class Clouds
{
	friend class Surface;
public:

	void Render();
	~Clouds();

private:
	uint32_t mapWidth = 0;
	uint32_t mapHeight = 0;

	Clouds(const uint32_t _mapWidth, const uint32_t _mapHeight);
};
