#pragma once

#include <stdint.h>
//#include <boost/heap/fibonacci_heap.hpp>

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

/*
struct DijkstraPathfindingNode
{
public:
	struct CmpDijkstraPathfindingNodePtrs
	{
	public:
		bool operator()(const DijkstraPathfindingNode* lhs, const DijkstraPathfindingNode* rhs) const
		{
			const uint32_t pl = lhs->givenCost;
			const uint32_t pr = rhs->givenCost;
			return (pl != pr) ? pl > pr : lhs == rhs;
		}
	};

	DijkstraPathfindingNode* parentNodePtr;
	int row;
	int col;
	uint32_t givenCost;
	uint16_t iterations;
	uint8_t listInWhichIAm;
	uint8_t directionFromStart;
	uint8_t directionFromParent;

	boost::heap::fibonacci_heap<DijkstraPathfindingNode*, boost::heap::compare<DijkstraPathfindingNode::CmpDijkstraPathfindingNodePtrs>>::handle_type handle;

	DijkstraPathfindingNode(const uint32_t _row, const uint32_t _col)
		: parentNodePtr(nullptr), row(_row), col(_col), givenCost(0), iterations(0), listInWhichIAm(PathfindingNodeStatus::OnNone), directionFromStart(0), directionFromParent(0)
	{
	}
};

typedef boost::heap::fibonacci_heap<DijkstraPathfindingNode*, boost::heap::compare<DijkstraPathfindingNode::CmpDijkstraPathfindingNodePtrs>> dijkstraOpenList_t;
*/