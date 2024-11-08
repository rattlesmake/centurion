/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <stdint.h>

enum PathfindingNodeStatus
{
	OnNone,
	OnOpenList,
	OnClosedList
};

struct PathfindingNode
{
public:
	PathfindingNode* parentNodePtr;
	int row;
	int col;
	uint32_t givenCost;
	uint32_t finalCost;
	uint16_t iterations;
	uint8_t listInWhichIAm;
	uint8_t directionFromParent;

	PathfindingNode(const uint32_t _row, const uint32_t _col)
		: parentNodePtr(nullptr), row(_row), col(_col), givenCost(0), finalCost(0), iterations(0), listInWhichIAm(PathfindingNodeStatus::OnNone), directionFromParent(0)
	{
	}
};

struct CmpPathfindingNodePtrs
{
public:
	bool operator()(const PathfindingNode* lhs, const PathfindingNode* rhs) const
	{
		const uint32_t pl = lhs->finalCost;
		const uint32_t pr = rhs->finalCost;

		return (pl != pr) ? pl < pr : lhs > rhs;
	}
};
