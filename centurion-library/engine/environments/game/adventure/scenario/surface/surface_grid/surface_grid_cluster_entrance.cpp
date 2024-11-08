#include "surface_grid_cluster_entrance.h"

#pragma region Constructors, operators and destructor:
SurfaceGridClusterEntrance::SurfaceGridClusterEntrance(const uint32_t _y, const uint32_t _x) :
	y(_y), x(_x)
{
}

SurfaceGridClusterEntrance::SurfaceGridClusterEntrance(SurfaceGridClusterEntrance&& other) noexcept :
	y(other.y), x(other.x)
{
}

SurfaceGridClusterEntrance& SurfaceGridClusterEntrance::operator=(SurfaceGridClusterEntrance&& other) noexcept
{
	if (this != (&other))
	{
		this->x = other.x;
		this->y = other.y;
	}
	return (*this);
}

bool SurfaceGridClusterEntrance::operator==(const SurfaceGridClusterEntrance& other) const noexcept
{
	return (this->x == other.x && this->y == other.y);
}

SurfaceGridClusterEntrance::~SurfaceGridClusterEntrance(void)
{
}
#pragma endregion


#pragma region Public members:
uint32_t SurfaceGridClusterEntrance::GetY(void) const
{
	return this->y;
}
uint32_t SurfaceGridClusterEntrance::GetX(void) const
{
	return this->x;
}
#pragma endregion


#pragma region Hash function:
size_t SurfaceGridClusterEntranceHashFunc::operator()(const SurfaceGridClusterEntrance& sgce) const
{
	return (std::hash<uint32_t>()(sgce.GetX())) ^ (std::hash<uint32_t>()(sgce.GetY()));
}
#pragma endregion
