#include "pathfinder.h"

#include <cstdlib>  //std::abs
#include <cmath>

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cell.h>


#ifndef EUCLIDEAN_HEURISTIC
#define EUCLIDEAN_HEURISTIC       1
#endif // !EUCLIDEAN_HEURISTIC

#ifndef MANHATTANN_HEURISTIC
#define MANHATTAN_HEURISTIC       2
#endif // !MANHATTAN_HEURISTIC

#ifndef HEURISTIC
#define HEURISTIC                 EUCLIDEAN_HEURISTIC
#endif // !HEURISTIC


#pragma region Constructors and destructor:
Pathfinder::Pathfinder(std::shared_ptr<SurfaceGrid> _surfaceGrid) :
	surfaceGrid(std::move(_surfaceGrid))
{
	this->fbh = std::shared_ptr<FibonacciHeap<int>>(new FibonacciHeap<int>());
}

Pathfinder::~Pathfinder(void)
{
}
#pragma endregion


#pragma region Public members:
void Pathfinder::Init(void)
{
	this->surfaceGrid->FindClusteresEntrances();
}
#pragma endregion


#pragma region Public static members:
double Pathfinder::ComputeHeuristicCost(const Node& node1, const Node& node2)
{
	double heuristicCost = -1.0f;
	#if HEURISTIC == EUCLIDEAN_HEURISTIC
	heuristicCost = sqrt((static_cast<int64_t>(node1.xGrid) - node2.xGrid) * (static_cast<int64_t>(node1.xGrid) - node2.xGrid)
		+ (static_cast<int64_t>(node1.yGrid) - node2.yGrid) * (static_cast<int64_t>(node1.yGrid) - node2.yGrid));
	#elif HEURISTIC == MANHATTAN_HEURISTIC
	heuristicCost = abs(static_cast<int64_t>(node1.xGrid) - node2.xGrid) + abs(static_cast<int64_t>(node1.yGrid) - node2.yGrid);
	#endif 
	return heuristicCost;
}
#pragma endregion

