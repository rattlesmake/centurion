#include "pathfinding.h"

/*
 * Ideal choice of fixed-point equivalent to 1.0 that can almost perfectly represent sqrt(2) and (sqrt(2) - 1) in whole numbers
 * 1.000000000 = 2378
 * 0.414213624 = 985 / 2378
 * 1.414213625 = 3363 / 2378
 * 1.414213562 = Actual sqrt(2)
 * 0.00000006252 = Difference between actual sqrt(2) and fixed-point sqrt(2)
 */
#define FIXED_POINT_MULTIPLIER 2378
#define FIXED_POINT_SHIFT(x) ((x) * FIXED_POINT_MULTIPLIER)
#define SQRT_2 3363
#define SQRT_2_MINUS_ONE 985

#pragma region Constructor:
Pathfinding::Pathfinding(const int mapHeight, const int mapWidth)
	: pcm(mapHeight, mapWidth), destRow(0), destCol(0), destNode(nullptr), nIterations(1), openList(nullptr), fastOpenList(nullptr), nodesCreatedMap(nullptr)
{
}
#pragma endregion

#pragma region Public members:
PreComputedMap& Pathfinding::GetPreComputedMap(void)
{
	return this->pcm;
}

std::list<glm::ivec2> Pathfinding::GetPath(const glm::ivec2& pStart, const glm::ivec2& pDest)
{
	std::list<glm::ivec2> path;

	//Init
	this->openList = new OpenList();
	this->fastOpenList = new stack<PathfindingNode*>;
	this->nodesCreatedMap = new unordered_map<pair<int, int>, PathfindingNode*, hash_pair>;

	//Create destination node
	this->destRow = (uint32_t) pDest.y;
	this->destCol = (uint32_t) pDest.x;
	this->destNode = this->CreateNode(this->destRow, this->destCol);

	this->nIterations += 1;

	//Create starting node
	PathfindingNode* startNode = this->CreateNode((uint32_t)pStart.y, (uint32_t)pStart.x);
	startNode->listInWhichIAm = PathfindingNodeStatus::OnOpenList;
	startNode->iterations = this->nIterations;

	//Search a path
	const uint8_t status = this->SearchPath(startNode);
	if (status == PathStatus::PathFound)
	{
		this->FinalizePath(path);
	}

	//Pay memory debt
	while (this->nodesCreatedList.empty() == false)
	{
		delete this->nodesCreatedList.front();
		this->nodesCreatedList.pop_front();
	}
	delete this->nodesCreatedMap;
	this->nodesCreatedMap = nullptr;
	delete this->fastOpenList;
	this->fastOpenList = nullptr;
	delete this->openList;
	this->openList = nullptr;

	this->nIterations = 1;
	return path;
}
#pragma endregion

#pragma region Private members:
inline PathfindingNode* Pathfinding::CreateNode(const uint32_t r, const uint32_t c)
{
	pair<int, int> coordinates{ r,c }; 
	PathfindingNode* node;
	if (this->nodesCreatedMap->contains(coordinates) == false)
	{
		node = new PathfindingNode(r, c);
		this->nodesCreatedList.push_back(node);
		this->nodesCreatedMap->insert({ std::move(coordinates) , node });
	}
	else
		node = this->nodesCreatedMap->at(coordinates);

	return node;
}

uint8_t Pathfinding::SearchPath(PathfindingNode* startNode)
{
	// Special case for the starting node
	if (startNode->row == this->destNode->row && startNode->col == this->destNode->col)
		return Pathfinding::PathFound;

	{
		Cell::jw_distancesPtr_t distances = this->pcm.GetCell(startNode->row, startNode->col)->GetDistances();
		///Explore all directions
		this->SearchDown(startNode, distances->at(Cell::Directions::Down));
		this->SearchDownLeft(startNode, distances->at(Cell::Directions::DownLeft));
		this->SearchLeft(startNode, distances->at(Cell::Directions::Left));
		this->SearchUpLeft(startNode, distances->at(Cell::Directions::UpLeft));
		this->SearchUp(startNode, distances->at(Cell::Directions::Up));
		this->SearchUpRight(startNode, distances->at(Cell::Directions::UpRight));
		this->SearchRight(startNode, distances->at(Cell::Directions::Right));
		this->SearchDownRight(startNode, distances->at(Cell::Directions::DownRight));
		startNode->listInWhichIAm = PathfindingNodeStatus::OnClosedList;
	}
	
	while (openList->IsEmpty() == false || this->fastOpenList->empty() == false)
	{
		PathfindingNode* currentNode = nullptr;
		
		if (this->fastOpenList->empty() == false)
		{
			currentNode = this->fastOpenList->top();
			this->fastOpenList->pop();
		}
		else
		{
			currentNode = this->openList->ExtractTop();
		}

		if (currentNode->row == this->destNode->row && currentNode->col == this->destNode->col)
			return PathStatus::PathFound;

		///Explore nodes based on parent
		Cell::jw_distancesPtr_t distances = this->pcm.GetCell(currentNode->row, currentNode->col)->GetDistances();
		const uint8_t parentDir = currentNode->directionFromParent;
		if (parentDir == Cell::Directions::Down)
		{
			this->SearchLeft(currentNode, distances->at(Cell::Directions::Left));
			this->SearchDownLeft(currentNode, distances->at(Cell::Directions::DownLeft));
			this->SearchDown(currentNode, distances->at(Cell::Directions::Down));
			this->SearchDownRight(currentNode, distances->at(Cell::Directions::DownRight));
			this->SearchRight(currentNode, distances->at(Cell::Directions::Right));
		}
		else if (parentDir == Cell::Directions::DownRight)
		{
			this->SearchDown(currentNode, distances->at(Cell::Directions::Down));
			this->SearchDownRight(currentNode, distances->at(Cell::Directions::DownRight));
			this->SearchRight(currentNode, distances->at(Cell::Directions::Right));
		}
		else if (parentDir == Cell::Directions::Right)
		{
			this->SearchDown(currentNode, distances->at(Cell::Directions::Down));
			this->SearchDownRight(currentNode, distances->at(Cell::Directions::DownRight));
			this->SearchRight(currentNode, distances->at(Cell::Directions::Right));
			this->SearchUpRight(currentNode, distances->at(Cell::Directions::UpRight));
			this->SearchUp(currentNode, distances->at(Cell::Directions::Up));
		}
		else if (parentDir == Cell::Directions::UpRight)
		{
			this->SearchRight(currentNode, distances->at(Cell::Directions::Right));
			this->SearchUpRight(currentNode, distances->at(Cell::Directions::UpRight));
			this->SearchUp(currentNode, distances->at(Cell::Directions::Up));
		}
		else if (parentDir == Cell::Directions::Up)
		{
			this->SearchRight(currentNode, distances->at(Cell::Directions::Right));
			this->SearchUpRight(currentNode, distances->at(Cell::Directions::UpRight));
			this->SearchUp(currentNode, distances->at(Cell::Directions::Up));
			this->SearchUpLeft(currentNode, distances->at(Cell::Directions::UpLeft));
			this->SearchLeft(currentNode, distances->at(Cell::Directions::Left));
		}
		else if (parentDir == Cell::Directions::UpLeft)
		{
			this->SearchUp(currentNode, distances->at(Cell::Directions::Up));
			this->SearchUpLeft(currentNode, distances->at(Cell::Directions::UpLeft));
			this->SearchLeft(currentNode, distances->at(Cell::Directions::Left));
		}
		else if (parentDir == Cell::Directions::Left)
		{
			this->SearchUp(currentNode, distances->at(Cell::Directions::Up));
			this->SearchUpLeft(currentNode, distances->at(Cell::Directions::UpLeft));
			this->SearchLeft(currentNode, distances->at(Cell::Directions::Left));
			this->SearchDownLeft(currentNode, distances->at(Cell::Directions::DownLeft));
			this->SearchDown(currentNode, distances->at(Cell::Directions::Down));
		}
		else if (parentDir == Cell::Directions::DownLeft)
		{
			this->SearchLeft(currentNode, distances->at(Cell::Directions::Left));
			this->SearchDownLeft(currentNode, distances->at(Cell::Directions::DownLeft));
			this->SearchDown(currentNode, distances->at(Cell::Directions::Down));
		}
		currentNode->listInWhichIAm = PathfindingNodeStatus::OnClosedList;
	}
	
	return PathStatus::NoPathExists;
}


void Pathfinding::FinalizePath(std::list<glm::ivec2>& finalPath)
{
	PathfindingNode* prevNode = nullptr;
	PathfindingNode* curNode = this->nodesCreatedList.back();

	while (curNode != nullptr)
	{
		const glm::ivec2 loc{ curNode->col, curNode->row };

		/*
		if (prevNode != nullptr)
		{
			//Insert extra nodes if needed
			int xDiff = curNode->col - prevNode->col;
			int yDiff = curNode->row - prevNode->row;

			int xInc = 0;
			int yInc = 0;

			if (xDiff > 0) 
			{ 
				xInc = 1; 
			}
			else if (xDiff < 0) 
			{ 
				xInc = -1; 
				xDiff = -xDiff; 
			}

			if (yDiff > 0) 
			{ 
				yInc = 1; 
			}
			else if (yDiff < 0) 
			{ 
				yInc = -1; 
				yDiff = -yDiff; 
			}

			int x = prevNode->col;
			int y = prevNode->row;
			int steps = xDiff - 1;
			if (yDiff > xDiff)  
				steps = yDiff - 1; 

			for (int i = 0; i < steps; i++)
			{
				x += xInc;
				y += yInc;

				const glm::vec2 locNew{ x, y };
				finalPath.push_back(locNew);
			}
		}
		*/

		finalPath.push_back(loc);
		prevNode = curNode;
		curNode = curNode->parentNodePtr;
	}
	std::reverse(finalPath.begin(), finalPath.end());
}

void Pathfinding::SearchDown(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	//Consider straight line to destination
	if (col == this->destCol && row < this->destRow)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		if ((row + absJumpDistance) >= this->destRow)
		{
			const uint32_t diff = this->destRow - row;
			const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(diff);
			PathfindingNode* newSuccessor = this->destNode;
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Down, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		//Directly jump
		int newRow = row + jumpDistance;
		uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, col);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Down, givenCost);
	}
}

void Pathfinding::SearchDownRight(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	///Check for destination in general direction (straight line to Goal or Target Jump Point)
	if (currentNode->row < this->destRow && currentNode->col < this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		const int diffRow = this->destRow - row;
		const int diffCol = this->destCol - col;
		int smallerDiff = diffRow;
		if (diffCol < smallerDiff)
			smallerDiff = diffCol;

		if (smallerDiff <= absJumpDistance)
		{
			const int newRow = row + smallerDiff;
			const int newCol = col + smallerDiff;
			uint32_t givenCost = currentNode->givenCost + (SQRT_2 * smallerDiff);
			PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::DownRight, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newRow = row + jumpDistance;
		const int newCol = col + jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::DownRight, givenCost);
	}
}

void Pathfinding::SearchRight(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	// Consider straight line to Goal
	if (row == this->destRow && col < this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		if ((col + absJumpDistance) >= this->destCol)
		{
			const uint32_t diff = this->destCol - col;
			const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(diff);
			PathfindingNode* newSuccessor = this->destNode;
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Right, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		int newCol = col + jumpDistance;
		uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(row, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Right, givenCost);
	}
}

void Pathfinding::SearchUpRight(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	///Check for destination in general direction (straight line to Goal or Target Jump Point)
	if (row > this->destRow && col < this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		const int diffRow = row - this->destRow;
		const int diffCol = this->destCol - col;
		int smallerDiff = diffRow;
		if (diffCol < smallerDiff)
			smallerDiff = diffCol;

		if (smallerDiff <= absJumpDistance)
		{
			const int newRow = row - smallerDiff;
			const int newCol = col + smallerDiff;
			const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * smallerDiff);
			PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::UpRight, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newRow = row - jumpDistance;
		const int newCol = col + jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::UpRight, givenCost);
	}
}

void Pathfinding::SearchUp(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	// Consider straight line to Goal
	if (col == this->destCol && row > this->destRow)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		if ((row - absJumpDistance) <= this->destRow)
		{
			const uint32_t diff = row - this->destRow;
			const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(diff);
			PathfindingNode* newSuccessor = this->destNode;
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Up, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newRow = row - jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, col);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Up, givenCost);
	}
}

void Pathfinding::SearchUpLeft(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	///Check for destination in general direction (straight line to Goal or Target Jump Point)
	if (row > this->destRow && col > this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		const int diffRow = row - this->destRow;
		const int diffCol = col - this->destCol;
		int smallerDiff = diffRow;
		if (diffCol < smallerDiff) 
			smallerDiff = diffCol;

		if (smallerDiff <= absJumpDistance)
		{
			const int newRow = row - smallerDiff;
			const int newCol = col - smallerDiff;
			const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * smallerDiff);
			PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::UpLeft, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newRow = row - jumpDistance;
		const int newCol = col - jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::UpLeft, givenCost);
	}
}

void Pathfinding::SearchLeft(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	// Consider straight line to Goal
	if (row == this->destRow && col > this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		if ((col - absJumpDistance) <= this->destCol)
		{
			const uint32_t diff = col - this->destCol;
			const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(diff);
			PathfindingNode* newSuccessor = this->destNode;
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Left, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newCol = col - jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + FIXED_POINT_SHIFT(jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(row, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::Left, givenCost);
	}
}

void Pathfinding::SearchDownLeft(PathfindingNode* currentNode, const int jumpDistance)
{
	const int row = currentNode->row;
	const int col = currentNode->col;

	///Check for destination in general direction (straight line to Goal or Target Jump Point)
	if (row < this->destRow && col > this->destCol)
	{
		const int absJumpDistance = (jumpDistance >= 0) ? jumpDistance : (-jumpDistance);

		const int diffRow = this->destRow - row;
		const int diffCol = col - this->destCol;
		int smallerDiff = diffRow;
		if (diffCol < smallerDiff)
			smallerDiff = diffCol;

		if (smallerDiff <= absJumpDistance)
		{
			const int newRow = row + smallerDiff;
			const int newCol = col - smallerDiff;
			const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * smallerDiff);
			PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
			this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::DownLeft, givenCost);
			return;
		}
	}

	if (jumpDistance > 0)
	{
		///Directly jump
		const int newRow = row + jumpDistance;
		const int newCol = col - jumpDistance;
		const uint32_t givenCost = currentNode->givenCost + (SQRT_2 * jumpDistance);
		PathfindingNode* newSuccessor = this->CreateNode(newRow, newCol);
		this->PlaceNodeInOpenList(newSuccessor, currentNode, Cell::Directions::DownLeft, givenCost);
	}
}

void Pathfinding::PlaceNodeInOpenList(PathfindingNode* newSuccessor, PathfindingNode* currentNode, const uint8_t parentDirection, const uint32_t givenCost)
{	
	if (newSuccessor->iterations != this->nIterations)
	{
		///Place a new node on the Open list
		const uint32_t heuristicCost = Pathfinding::ComputeOctileHeuristicCost(newSuccessor->row, newSuccessor->col, this->destRow, this->destCol);

		newSuccessor->parentNodePtr = currentNode;
		newSuccessor->directionFromParent = parentDirection;
		newSuccessor->givenCost = givenCost;
		newSuccessor->finalCost = givenCost + heuristicCost;
		newSuccessor->listInWhichIAm = PathfindingNodeStatus::OnOpenList;
		newSuccessor->iterations = this->nIterations;

		if (newSuccessor->finalCost <= currentNode->finalCost)
		{
			this->fastOpenList->push(newSuccessor);
		}
		else
		{
			this->openList->Push(newSuccessor);
		}
	}
	else if (givenCost < newSuccessor->givenCost) 
	{
		///Update a node on the Open list
		const uint32_t heuristicCost = newSuccessor->finalCost - newSuccessor->givenCost;

		newSuccessor->parentNodePtr = currentNode;
		newSuccessor->directionFromParent = parentDirection;
		newSuccessor->givenCost = givenCost;
		newSuccessor->finalCost = givenCost + heuristicCost;

		this->openList->Update(newSuccessor);
	}
}
#pragma endregion

#pragma region Static members:
inline uint32_t Pathfinding::ComputeOctileHeuristicCost(const int row1, const int col1, const int row2, const int col2)
{
	//Compute octile heuristic cost (optimized: minDiff * SQRT_2_MINUS_ONE + maxDiff)
	const uint32_t diffRow = std::abs(row2 - row1);
	const uint32_t diffCol = std::abs(col2 - col1);
	const uint32_t heuristicCost = (diffRow <= diffCol) ? (diffRow * SQRT_2_MINUS_ONE) + FIXED_POINT_SHIFT(diffCol) : (diffCol * SQRT_2_MINUS_ONE) + FIXED_POINT_SHIFT(diffRow);
	return heuristicCost;
}
#pragma endregion

