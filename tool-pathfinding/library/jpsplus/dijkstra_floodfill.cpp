#include "dijkstra_floodfill.h"

#if GOAL_BOUNDING

#define FIXED_POINT_MULTIPLIER 100000
#define FIXED_POINT_SHIFT(x) ((x) * FIXED_POINT_MULTIPLIER)
#define FIXED_POINT_ONE FIXED_POINT_MULTIPLIER
#define FIXED_POINT_SQRT_2 141421

#pragma region Constructor and destructor
DijkstraFloodfill::DijkstraFloodfill(PreComputedMap* _pcm)
	: pcm(_pcm), nodesCreatedMap(nullptr), openList(nullptr), nIterations(1)
{
	this->nodesCreatedMap = new unordered_map<pair<int, int>, DijkstraPathfindingNode*, hash_pair>;
}

DijkstraFloodfill::~DijkstraFloodfill(void)
{
	while (this->nodesCreatedList.empty() == false)
	{
		delete this->nodesCreatedList.front();
		this->nodesCreatedList.pop_front();
	}
	if (this->nodesCreatedMap != nullptr)
	{
		delete this->nodesCreatedMap;
		this->nodesCreatedMap = nullptr;
	}
	if (this->openList != nullptr)
	{
		delete this->openList;
		this->openList = nullptr;
	}
}
#pragma endregion

#pragma region Public static members:
std::shared_ptr<DijkstraFloodfill> DijkstraFloodfill::CreateDijkstraFloodfill(PreComputedMap* _pcm)
{
	return std::shared_ptr<DijkstraFloodfill>(new DijkstraFloodfill(_pcm));
}
#pragma endregion


#pragma region Public members:
uint16_t DijkstraFloodfill::GetCurrentInteration(void) const
{
	return this->nIterations;
}

const DijkstraPathfindingNode* const DijkstraFloodfill::GetDijkstraNode(const int r, const int c) const
{
	return this->nodesCreatedMap->at({ r,c });
}

void DijkstraFloodfill::Flood(const int r, const int c)
{
	//Init 
	if (this->openList == nullptr)
		this->openList = new dijkstraOpenList_t();

	this->nIterations++;

	if (this->pcm->IsEmpty(r, c) == true)
	{
		//Begin with starting node
		DijkstraPathfindingNode* startNode = this->CreateDijkstraNode(r, c);
		startNode->directionFromStart = 255;
		startNode->directionFromParent = 255;
		startNode->iterations = this->nIterations;
		startNode->listInWhichIAm = PathfindingNodeStatus::OnOpenList;

		///Explore all directions
		this->SearchAllDirections(startNode);

		startNode->listInWhichIAm = PathfindingNodeStatus::OnClosedList;
	}

	while (this->openList->empty() == false)
	{
		DijkstraPathfindingNode* currentNode = this->openList->top();
		this->openList->pop();
		const uint8_t parentDir = currentNode->directionFromParent;
		if (parentDir == Cell::Directions::Down)
		{
			this->SearchLeft(currentNode);
			this->SearchDownLeft(currentNode);
			this->SearchDown(currentNode);
			this->SearchDownRight(currentNode);
			this->SearchRight(currentNode);
		}
		else if (parentDir == Cell::Directions::DownRight)
		{
			this->SearchDown(currentNode);
			this->SearchDownRight(currentNode);
			this->SearchRight(currentNode);
		}
		else if (parentDir == Cell::Directions::Right)
		{
			this->SearchDown(currentNode);
			this->SearchDownRight(currentNode);
			this->SearchRight(currentNode);
			this->SearchUpRight(currentNode);
			this->SearchUp(currentNode);
		}
		else if (parentDir == Cell::Directions::UpRight)
		{
			this->SearchRight(currentNode);
			this->SearchUpRight(currentNode);
			this->SearchUp(currentNode);
		}
		else if (parentDir == Cell::Directions::Up)
		{
			this->SearchRight(currentNode);
			this->SearchUpRight(currentNode);
			this->SearchUp(currentNode);
			this->SearchUpLeft(currentNode);
			this->SearchLeft(currentNode);
		}
		else if (parentDir == Cell::Directions::UpLeft)
		{
			this->SearchUp(currentNode);
			this->SearchUpLeft(currentNode);
			this->SearchLeft(currentNode);
		}
		else if (parentDir == Cell::Directions::Left)
		{
			this->SearchUp(currentNode);
			this->SearchUpLeft(currentNode);
			this->SearchLeft(currentNode);
			this->SearchDownLeft(currentNode);
			this->SearchDown(currentNode);
		}
		else if (parentDir == Cell::Directions::DownLeft)
		{
			this->SearchLeft(currentNode);
			this->SearchDownLeft(currentNode);
			this->SearchDown(currentNode);
		}
		currentNode->listInWhichIAm = PathfindingNodeStatus::OnClosedList;
	}

	//Pay memory debt
	delete this->openList;
	this->openList = nullptr;
}
#pragma endregion

#pragma region Private members:
inline DijkstraPathfindingNode* DijkstraFloodfill::CreateDijkstraNode(const uint32_t r, const uint32_t c)
{
	if (r < 0 || c < 0 || r >= this->pcm->GetHeight() || c >= this->pcm->GetWidth())
	{
		return nullptr;
	}
	pair<int, int> coordinates{ r,c };
	DijkstraPathfindingNode* node;
	if (this->nodesCreatedMap->contains(coordinates) == false)
	{
		node = new DijkstraPathfindingNode(r, c);
		this->nodesCreatedList.push_back(node);
		this->nodesCreatedMap->insert({ std::move(coordinates) , node });
	}
	else
		node = this->nodesCreatedMap->at(coordinates);

	return node;
}

void DijkstraFloodfill::SearchAllDirections(DijkstraPathfindingNode* currentNode)
{
	//DOWN, DOWNRIGHT, RIGHT, UPRIGHT, UP, UPLEFT, LEFT, DOWNLEFT
	static const array<int8_t, NUMBER_OF_DIRECTIONS> offsetRow{ 1, 1, 0, -1, -1, -1,  0,  1 };
	static const array<int8_t, NUMBER_OF_DIRECTIONS> offsetCol{ 0, 1, 1,  1,  0, -1, -1, -1 };

	for (uint8_t i = 0; i < NUMBER_OF_DIRECTIONS; i++)
	{
		//Check if out of grid bounds
		const uint32_t neighborRow = currentNode->row + offsetRow[i];
		if (neighborRow >= this->pcm->GetHeight())
			continue;
		const uint32_t neighborCol = currentNode->col + offsetCol[i];
		if (neighborCol >= this->pcm->GetWidth())
			continue;

		//Valid tile --> Get the node
		DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(neighborRow, neighborCol);

		//Check if blocked
		if (this->pcm->IsEmpty(neighborRow, neighborCol) == false)
			continue;
		//Check if diagonal blocked
		const bool bIsDiagonal = (i & 0x1) == 1;
		if (bIsDiagonal == true && (this->pcm->IsEmpty(currentNode->row + offsetRow[i], currentNode->col) == false || this->pcm->IsEmpty(currentNode->row, currentNode->col + offsetCol[i]) == false))
			continue;

		const uint32_t costToNextNode = bIsDiagonal ? FIXED_POINT_SQRT_2 : FIXED_POINT_ONE;
		const uint8_t dir = i;

		this->PlaceNodeInOpenList(newSuccessor, currentNode, dir, dir, costToNextNode);
	}
}

void DijkstraFloodfill::SearchDown(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_ONE;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row + 1, currentNode->col);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::Down, givenCost);
}

void DijkstraFloodfill::SearchDownRight(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SQRT_2;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row + 1, currentNode->col + 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::DownRight, givenCost);
}

void DijkstraFloodfill::SearchRight(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_ONE;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row, currentNode->col + 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::Right, givenCost);
}

void DijkstraFloodfill::SearchUpRight(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SQRT_2;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row - 1, currentNode->col + 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::UpRight, givenCost);
}

void DijkstraFloodfill::SearchUp(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_ONE;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row - 1, currentNode->col);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::Up, givenCost);
}

void DijkstraFloodfill::SearchUpLeft(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SQRT_2;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row - 1, currentNode->col - 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::UpLeft, givenCost);
}

void DijkstraFloodfill::SearchLeft(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_ONE;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row, currentNode->col - 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::Left, givenCost);
}

void DijkstraFloodfill::SearchDownLeft(DijkstraPathfindingNode* currentNode)
{
	const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SQRT_2;
	DijkstraPathfindingNode* newSuccessor = this->CreateDijkstraNode(currentNode->row + 1, currentNode->col - 1);
	this->PlaceNodeInOpenList(newSuccessor, currentNode, currentNode->directionFromStart, Cell::Directions::DownLeft, givenCost);
}

void DijkstraFloodfill::PlaceNodeInOpenList(DijkstraPathfindingNode* newSuccessor, DijkstraPathfindingNode* currentNode, const uint8_t startDirection, const uint8_t parentDirection, const uint32_t givenCost)
{
	if (newSuccessor == nullptr)
		return;
	if (newSuccessor->iterations != this->nIterations)
	{
		//Place node on the Open list
		newSuccessor->parentNodePtr = currentNode;
		newSuccessor->directionFromStart = startDirection;
		newSuccessor->directionFromParent = parentDirection;
		newSuccessor->givenCost = givenCost;
		newSuccessor->listInWhichIAm = PathfindingNodeStatus::OnOpenList;
		newSuccessor->iterations = this->nIterations;

		newSuccessor->handle = this->openList->push(newSuccessor);
	}
	else if (givenCost < newSuccessor->givenCost && newSuccessor->listInWhichIAm == PathfindingNodeStatus::OnOpenList)
	{
		//Update node on the Open list
		this->openList->erase(newSuccessor->handle);
		newSuccessor->parentNodePtr = currentNode;
		newSuccessor->directionFromStart = startDirection;
		newSuccessor->directionFromParent = parentDirection;
		newSuccessor->givenCost = givenCost;
		newSuccessor->handle = this->openList->push(newSuccessor);
	}
}

#pragma endregion

#endif
