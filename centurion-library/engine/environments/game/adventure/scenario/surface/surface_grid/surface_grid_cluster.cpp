#include "surface_grid_cluster.h"

#pragma region Constructors and destructor:
SurfaceGridCluster::SurfaceGridCluster(const SurfaceGrid& _surface, const uint32_t _clusterIndex) :
	surface(_surface), 
	clusterIndex(_clusterIndex),
	surfaceGridPortion(_surface.GetClusterGridSizeY(), std::vector<SurfaceGridCell>(_surface.GetClusterGridSizeX()))
{
}

SurfaceGridCluster::SurfaceGridCluster(SurfaceGridCluster&& other) noexcept:
	surface(other.surface), clusterIndex(other.clusterIndex), surfaceGridPortion(std::move(other.surfaceGridPortion))
{
}

SurfaceGridCluster::~SurfaceGridCluster(void)
{
}
#pragma endregion

#pragma region Public members:
void SurfaceGridCluster::MakeFullyFree(void)
{
	for (auto& row : this->surfaceGridPortion)
		for (auto& column : row)
			column.MarkAsFree();
}

void SurfaceGridCluster::SetCellValue(const uint32_t xCluster, const uint32_t yCluster, const CellValues value)
{
	if (yCluster < this->surfaceGridPortion.size() && xCluster < this->surfaceGridPortion[yCluster].size())
		this->surfaceGridPortion.at(yCluster).at(xCluster).SetValue(value);
}

const SurfaceGridCell& SurfaceGridCluster::GetCellCRef(const uint32_t xCluster, const uint32_t yCluster) const
{
	if (yCluster >= this->surfaceGridPortion.size() || xCluster >= this->surfaceGridPortion[yCluster].size())
		throw std::runtime_error("Bad cluster coords"); //TODO - aggiungere custom exception
	return this->surfaceGridPortion.at(yCluster).at(xCluster);
}

const std::vector<std::vector<SurfaceGridCell>>& SurfaceGridCluster::GetGridCRef(void) const
{
	return this->surfaceGridPortion;
}

void SurfaceGridCluster::SetLeftEntrance(const uint32_t entranceY)
{
	const uint32_t entranceX = this->surface.GetClusterGridSizeX() - 1;
	this->leftEntrances.insert(SurfaceGridClusterEntrance(entranceY, entranceX));
}

void SurfaceGridCluster::SetRightEntrance(const uint32_t entranceY)
{
	const uint32_t entranceX = 0;
	this->leftEntrances.insert(SurfaceGridClusterEntrance(entranceY, entranceX));
}

void SurfaceGridCluster::SetUpEntrance(const uint32_t entranceX)
{
	const uint32_t entranceY = 0;
	this->leftEntrances.insert(SurfaceGridClusterEntrance(entranceY, entranceX));
}

void SurfaceGridCluster::SetBottomEntrance(const uint32_t entranceX)
{
	const uint32_t entranceY = this->surface.GetClusterGridSizeY() - 1;
	this->leftEntrances.insert(SurfaceGridClusterEntrance(entranceY, entranceX));
}
#pragma endregion
