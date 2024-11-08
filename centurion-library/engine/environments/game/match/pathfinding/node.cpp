#include "node.h"

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster.h>


Node::Node(const uint32_t _xGrid, const uint32_t _yGrid) :
    xGrid(_xGrid), yGrid(_yGrid)
{
}

#pragma region Operators:
bool Node::operator<(const Node& other) const noexcept
{
    return (this->fCost < other.hCost);
}

bool Node::operator==(const Node& other) const noexcept
{
    return (this->xGrid == other.xGrid && this->yGrid == other.yGrid);
}
#pragma endregion

#pragma region Public static members:
bool Node::IsValid(const uint32_t xGrid, const uint32_t yGrid, SurfaceGrid& surfaceGridRef)
{
    //Check if point is out of the grid
    if (xGrid >= surfaceGridRef.GetClusterGridSizeX() || yGrid >= surfaceGridRef.GetClusterGridSizeY() * GRID_CELL_SIZE)
        return false;
    //Get cluster coordinates by grid coordinates
    const int32_t xCluster = surfaceGridRef.XGridToXCluster(xGrid);
    const int32_t yCluster = surfaceGridRef.YGridToYCluster(yGrid);
    //Check if the cell in the cluster of the grid is a wall
    if (surfaceGridRef.GetClusterRefByCellCoords(xGrid, yGrid).GetCellCRef(xCluster, yCluster).IsWall() == true)
        return false;
    return true;
}
#pragma endregion
