#pragma once

#include "precomputed_map.h"

#if GOAL_BOUNDING
#include "open_list.h"

class DijkstraFloodfill
{
public:
	[[nodiscard]] static std::shared_ptr<DijkstraFloodfill> CreateDijkstraFloodfill(PreComputedMap* _pcm);
	~DijkstraFloodfill(void);

	[[nodiscard]] uint16_t GetCurrentInteration(void) const;
	[[nodiscard]] const DijkstraPathfindingNode* const GetDijkstraNode(const int r, const int c) const;
	void Flood(const int r, const int c);
private:
	explicit DijkstraFloodfill(PreComputedMap* _pcm);

	[[nodiscard]] inline DijkstraPathfindingNode* CreateDijkstraNode(const uint32_t r, const uint32_t c);
	void SearchAllDirections(DijkstraPathfindingNode* currentNode);
	void SearchDown(DijkstraPathfindingNode* currentNode);
	void SearchDownRight(DijkstraPathfindingNode* currentNode);
	void SearchRight(DijkstraPathfindingNode* currentNode);
	void SearchUpRight(DijkstraPathfindingNode* currentNode);
	void SearchUp(DijkstraPathfindingNode* currentNode);
	void SearchUpLeft(DijkstraPathfindingNode* currentNode);
	void SearchLeft(DijkstraPathfindingNode* currentNode);
	void SearchDownLeft(DijkstraPathfindingNode* currentNode);
	void PlaceNodeInOpenList(DijkstraPathfindingNode* newSuccessor, DijkstraPathfindingNode* currentNode, const uint8_t startDirection, const uint8_t parentDirection, const uint32_t givenCost);

	PreComputedMap* pcm;
	uint16_t nIterations; //This allows us to know if a node has been touched this iteration (faster than clearing all the nodes before each search).
	dijkstraOpenList_t* openList;

	list<DijkstraPathfindingNode*> nodesCreatedList;
	unordered_map<pair<int, int>, DijkstraPathfindingNode*, hash_pair>* nodesCreatedMap;	
};
#endif
