#include "surface_grid_cell.h"

#pragma region Constructors and destructor:
SurfaceGridCell::SurfaceGridCell(void) :
	value(CellValues::E_FREE)
{
}

SurfaceGridCell::SurfaceGridCell(const CellValues _value) :
	value(_value)
{
}

SurfaceGridCell::~SurfaceGridCell(void)
{
}
#pragma endregion

#pragma region Public members:
void SurfaceGridCell::SetValue(const CellValues _value)
{
	this->value = _value;
}

bool SurfaceGridCell::IsWall(void) const
{
	return this->value == CellValues::E_WALL;
}

bool SurfaceGridCell::IsFree(void) const
{
	return (this->IsWall() == false);
}

void SurfaceGridCell::MarkAsFree(void)
{
	this->value = CellValues::E_FREE;
}

void SurfaceGridCell::MarkAsWall(void)
{
	this->value = CellValues::E_WALL;
}
#pragma endregion
