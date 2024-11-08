/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>

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

	//Distances from a jump point or from a wall.
	typedef array<int, NUMBER_OF_DIRECTIONS> jw_distances_t;
	typedef const jw_distances_t* const jw_distancesPtr_t;
	
	#pragma region Constructor:
	Cell(void);
	#pragma endregion

	#pragma region Setters and getters
	void SetTraversability(const bool _bTraversability);
	void SetDistance(const uint8_t dir, const int distance);
	[[nodiscard]] int GetDistance(const uint8_t dir) const;
	[[nodiscard]] jw_distancesPtr_t GetDistances(void) const;
	#pragma endregion
	void MarkAsJumpPoint(const uint8_t dir);
	[[nodiscard]] bool IsTraversable(void) const;
	[[nodiscard]] bool IsPrimaryJumpPoint(const uint8_t dir) const;
private:
	uint8_t jumpPointFlags;
	bool bIsTraversable;
	jw_distances_t distancesArray;
};
