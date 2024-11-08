/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>

enum class CellValues : uint8_t
{
	E_FREE = 0,
	E_WALL = 1,
};

class SurfaceGridCell
{
public:
	SurfaceGridCell(void);
	explicit SurfaceGridCell(const CellValues _value);
	~SurfaceGridCell(void);

	void SetValue(const CellValues _value);

	[[nodiscard]] bool IsWall(void) const;
	[[nodiscard]] bool IsFree(void) const;

	void MarkAsFree(void);
	void MarkAsWall(void);
private:
	CellValues value;
};
