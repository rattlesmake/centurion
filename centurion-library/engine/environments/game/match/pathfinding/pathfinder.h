/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>

#include <environments/game/match/pathfinding/node.h>
#include <stl_extention/fibonacci_heap.h>

class SurfaceGrid;

class Pathfinder
{
public:
	explicit Pathfinder(std::shared_ptr<SurfaceGrid> _surfaceGrid);
	Pathfinder(const Pathfinder& other) = delete;
	Pathfinder& operator= (const Pathfinder& other) = delete;
	~Pathfinder(void);

	void Init(void);

	[[nodiscard]] static double ComputeHeuristicCost(const Node& node1, const Node& node2);
private:
	std::shared_ptr<SurfaceGrid> surfaceGrid;
	std::shared_ptr<FibonacciHeap<int>> fbh;
};
