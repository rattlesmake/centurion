/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include "precomputed_map.h"
#include "open_list.h"

namespace path_finding
{
	class Pathfinding
	{
	public:
		enum PathStatus
		{
			Searching,
			PathFound,
			NoPathExists
		};

		Pathfinding(const int mapHeight, const int mapWidth);

		[[nodiscard]] PreComputedMap& GetPreComputedMap(void);
		[[nodiscard]] std::list<glm::ivec2> GetPath(const glm::ivec2& pStart, const glm::ivec2& pDest);
	private:
		//Precomputed data (Jump points and walls)
		PreComputedMap pcm;

		uint16_t nIterations; //This allows us to know if a node has been touched this iteration (faster than clearing all the nodes before each search)
		//Destination node
		int destRow; //Y
		int destCol; //X
		PathfindingNode* destNode;

		list<PathfindingNode*> nodesCreatedList;
		unordered_map<pair<int, int>, PathfindingNode*, hash_pair>* nodesCreatedMap;

		//Open list
		OpenList* openList;
		stack<PathfindingNode*>* fastOpenList;

		[[nodiscard]] inline PathfindingNode* CreateNode(const uint32_t r, const uint32_t c);
		[[nodiscard]] uint8_t SearchPath(PathfindingNode* startNode);
		void FinalizePath(std::list<glm::ivec2>& finalPath);
		void SearchDown(PathfindingNode* currentNode, const int jumpDistance);
		void SearchDownRight(PathfindingNode* currentNode, const int jumpDistance);
		void SearchRight(PathfindingNode* currentNode, const int jumpDistance);
		void SearchUpRight(PathfindingNode* currentNode, const int jumpDistance);
		void SearchUp(PathfindingNode* currentNode, const int jumpDistance);
		void SearchUpLeft(PathfindingNode* currentNode, const int jumpDistance);
		void SearchLeft(PathfindingNode* currentNode, const int jumpDistance);
		void SearchDownLeft(PathfindingNode* currentNode, const int jumpDistance);
		void PlaceNodeInOpenList(PathfindingNode* newSuccessor, PathfindingNode* currentNode, const uint8_t parentDirection, const uint32_t givenCost);
		void Smooth(std::list<glm::ivec2>& finalPath) const;

		[[nodiscard]] static inline uint32_t ComputeOctileHeuristicCost(const int row1, const int col1, const int row2, const int col2);
		[[nodiscard]] inline bool IsWalkable(const std::list<glm::ivec2>::iterator& pointA, const std::list<glm::ivec2>::iterator& pointB) const;
	};

	bool Init(const int mapHeight, const int mapWidth);
	bool Reset(void);
	Pathfinding* Get(void);
};
