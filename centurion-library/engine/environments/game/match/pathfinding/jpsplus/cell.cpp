#include "cell.h"

#pragma region Constructor:
Cell::Cell(void)
{
	this->bIsTraversable = true; //The current cell can be crossed.
	this->distancesArray = { 0 }; 
	this->jumpPointFlags = 0; //The current cell is not a jump point from any cardinal direction. 
}
#pragma endregion

#pragma region Setters and getters
void Cell::SetTraversability(const bool _bTraversability)
{
	bIsTraversable = _bTraversability;
}

void Cell::SetDistance(const uint8_t dir, const int distance)
{
	switch (dir)
	{
		case Directions::Down:
			distancesArray[Directions::Down] = distance;
			break;
		case Directions::DownRight:
			distancesArray[Directions::DownRight] = distance;
			break;
		case Directions::Right:
			distancesArray[Directions::Right] = distance;
			break;
		case Directions::UpRight:
			distancesArray[Directions::UpRight] = distance;
			break;
		case Directions::Up:
			distancesArray[Directions::Up] = distance;
			break;
		case Directions::UpLeft:
			distancesArray[Directions::UpLeft] = distance;
			break;
		case Directions::Left:
			distancesArray[Directions::Left] = distance;
			break;
		case Directions::DownLeft:
			distancesArray[Directions::DownLeft] = distance;
			break;
		default:
			break;
	}
}

int Cell::GetDistance(const uint8_t dir) const
{
	return (dir < NUMBER_OF_DIRECTIONS) ? distancesArray[dir] : 0;
}

Cell::jw_distancesPtr_t Cell::GetDistances(void) const
{
	return &(this->distancesArray);
}
#pragma endregion

void Cell::MarkAsJumpPoint(const uint8_t dir)
{
	switch (dir)
	{
	case Directions::Down:
		this->jumpPointFlags |= BitfieldDirections::MovingDown;
		break;
	case Directions::Right:
		this->jumpPointFlags |= BitfieldDirections::MovingRight;
		break;
	case Directions::Up:
		this->jumpPointFlags |= BitfieldDirections::MovingUp;
		break;
	case Directions::Left:
		this->jumpPointFlags |= BitfieldDirections::MovingLeft;
		break;
	default:
		break;
	}
}

bool Cell::IsTraversable(void) const
{
	return this->bIsTraversable;
}

bool Cell::IsPrimaryJumpPoint(const uint8_t dir) const
{
	bool bIsJumpPointFromDir = false;
	switch (dir)
	{
	case Directions::Down:
		bIsJumpPointFromDir = (this->jumpPointFlags & BitfieldDirections::MovingDown) > 0;
		break;
	case Directions::Right:
		bIsJumpPointFromDir = (this->jumpPointFlags & BitfieldDirections::MovingRight) > 0;
		break;
	case Directions::Up:
		bIsJumpPointFromDir = (this->jumpPointFlags & BitfieldDirections::MovingUp) > 0;
		break;
	case Directions::Left:
		bIsJumpPointFromDir = (this->jumpPointFlags & BitfieldDirections::MovingLeft) > 0;
		break;
	default:
		break;
	}

	return bIsJumpPointFromDir;
}
