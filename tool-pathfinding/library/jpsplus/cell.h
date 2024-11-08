#pragma once

//If goal bounding is used (1) or not (0)
#ifndef GOAL_BOUNDING
#define GOAL_BOUNDING	0
#endif 

#include "header.h"

#ifndef NUMBER_OF_DIRECTIONS
#define NUMBER_OF_DIRECTIONS		8
#endif 

#ifndef NUMBER_OF_EDGES_PER_CELL
#define NUMBER_OF_EDGES_PER_CELL	4
#endif 

class Cell
{
public:
	enum Directions
	{
		Down = 0,			//South
		DownRight = 1,		//South-East
		Right = 2,			//East
		UpRight = 3,		//North-East
		Up = 4,				//North
		UpLeft = 5,			//North-West
		Left = 6,			//West
		DownLeft = 7,		//South-West
		All = 8				
	};

	enum BitfieldDirections
	{
		MovingDown = 1 << 0,	//00000001
		MovingRight = 1 << 1,	//00000010
		MovingUp = 1 << 2,		//00000100
		MovingLeft = 1 << 3,	//00001000
	};

	#if GOAL_BOUNDING
	enum GoalBoundsEnum
	{
		MinRow = 0,
		MaxRow = 1,
		MinCol = 2,
		MaxCol = 3
	};
	#endif 

	//Distances from a jump point or from a wall.
	typedef array<int, NUMBER_OF_DIRECTIONS> jw_distances_t;
	typedef const jw_distances_t* const jw_distancesPtr_t;
	
	#if GOAL_BOUNDING
	//Goal bounds (bounds[NUMBER_OF_DIRECTIONS][NUMBER_OF_EDGES_PER_CELL])
	typedef array<array<uint16_t, NUMBER_OF_EDGES_PER_CELL>, NUMBER_OF_DIRECTIONS> goalBounds_t;
	#endif 

	#pragma region Constructor:
	Cell(const uint32_t _height, const uint32_t _width);
	#pragma endregion

	#pragma region Setters and getters
	void SetTraversability(const bool _bTraversability);
	void SetDistance(const uint8_t dir, const int distance);
	[[nodiscard]] int GetDistance(const uint8_t dir) const;
	[[nodiscard]] jw_distancesPtr_t GetDistances(void) const;
	#if GOAL_BOUNDING
	void SetGoalBound(const uint8_t dir, const uint8_t edge, const uint16_t value);
	[[nodiscard]] uint8_t GetGoalBound(const uint8_t dir, const uint8_t edge) const;
	#endif
	#pragma endregion
	void MarkAsJumpPoint(const uint8_t dir);
	[[nodiscard]] bool IsTraversable(void) const;
	[[nodiscard]] bool IsPrimaryJumpPoint(const uint8_t dir) const;
private:
	uint8_t jumpPointFlags;
	bool bIsTraversable;
	jw_distances_t distancesArray;
	#if GOAL_BOUNDING
	goalBounds_t boundsMatrix;
	#endif
};
