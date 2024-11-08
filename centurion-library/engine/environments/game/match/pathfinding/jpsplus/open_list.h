/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include "pathfinding_node.h"
#include <set>

class OpenList
{
public:
	void Push(PathfindingNode* pNode);
	[[nodiscard]] PathfindingNode* ExtractTop(void);
	void Update(PathfindingNode* pNode);
	[[nodiscard]] bool IsEmpty(void) const;
	[[nodiscard]] uint32_t GetSize(void) const;
private:
	std::set<PathfindingNode*, CmpPathfindingNodePtrs> elements;
};
