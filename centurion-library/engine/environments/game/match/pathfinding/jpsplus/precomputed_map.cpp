#include "precomputed_map.h"

#ifndef DEBUG_CODE
#define DEBUG_CODE 0
#endif 

#pragma region Constructor
PreComputedMap::PreComputedMap(const int _height, const int _width)
	: height((uint32_t)_height), width((uint32_t)_width)
{
	this->cells = std::vector<Cell>{(uint64_t)this->height * this->width, Cell()};
}
#pragma endregion

#pragma region Getters:
uint32_t PreComputedMap::GetHeight(void) const
{
	return this->height;
}

uint32_t PreComputedMap::GetWidth(void) const
{
	return this->width;
}
#pragma endregion

#pragma region Public members:
void PreComputedMap::AddWall(const uint32_t index)
{
	this->cells[index].SetTraversability(false);
}

void PreComputedMap::AddWall(const uint32_t row, const uint32_t col)
{
	const uint32_t index = col + (row * this->width);
	this->cells[index].SetTraversability(false);
}

void PreComputedMap::PerformPreComputation(void)
{
	this->IdentifyPrimaryJumpPoint();
	this->ComputeDistances();

	#if DEBUG_CODE
	std::cout << "NORD OVEST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::UpLeft) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "NORD" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::Up) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "NORD EST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::UpRight) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "EST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::Right) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "SUD EST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::DownRight) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "SUD" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::Down) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "SUD OVEST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::DownLeft) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "OVEST" << std::endl;
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			std::cout << this->cells[index].GetDistance(Cell::Directions::Left) << " ";
		}
		std::cout << std::endl;
	}
	#endif
}

const Cell* const PreComputedMap::GetCell(const uint32_t r, const uint32_t c) const
{
	const uint32_t index = c + (r * this->width);
	return (&this->cells[index]);
}

bool PreComputedMap::IsEmpty(const int r, const int c) const
{
	return !(this->IsWall(r, c));
}
#pragma endregion

#pragma region Private members:
void PreComputedMap::IdentifyPrimaryJumpPoint(void)
{
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; c++)
		{
			if (this->IsEmpty(r, c) == true) //A wall can't obviously be a jump point.
			{
				const uint32_t index = c + (r * this->width);
				if (this->IsPrimaryJumpPoint(r, c, 1, 0) == true) //Chek if the node is a primary jump node traveling to the node from south.
				{
					this->cells[index].MarkAsJumpPoint(Cell::Directions::Down);
				}
				if (this->IsPrimaryJumpPoint(r, c, -1, 0) == true)
				{
					this->cells[index].MarkAsJumpPoint(Cell::Directions::Up); //Chek if the node is a primary jump node traveling to the node from north.
				}
				if (this->IsPrimaryJumpPoint(r, c, 0, 1) == true)
				{
					this->cells[index].MarkAsJumpPoint(Cell::Directions::Right); //Chek if the node is a primary jump node traveling to the node from east.
				}
				if (this->IsPrimaryJumpPoint(r, c, 0, -1) == true)
				{
					this->cells[index].MarkAsJumpPoint(Cell::Directions::Left); //Chek if the node is a primary jump node traveling to the node from west.
				}
			}
		}
	}
}

void PreComputedMap::ComputeDistances(void)
{
	this->StraightLeftRight();
	this->StraightUpDown();
	this->MainDiagonal();
	this->SecondaryDiagonal();
}

void PreComputedMap::StraightLeftRight(void)
{
	//Calculate distant jump points/walls (Up and Down)
	for (int r = 0; r < this->height; ++r)
	{
		{
			int countMovingLeft = -1;
			bool bJumpPointLastSeen = false;
			for (int c = 0; c < this->width; ++c)
			{
				const uint32_t index = c + (r * this->width);
				if (this->IsWall(r, c) == true)
				{
					countMovingLeft = -1;
					bJumpPointLastSeen = false;
					this->cells[index].SetDistance(Cell::Directions::Left, 0);
					continue;
				}

				countMovingLeft++;

				if (bJumpPointLastSeen == true)
					this->cells[index].SetDistance(Cell::Directions::Left, countMovingLeft);
				else // Wall last seen
					this->cells[index].SetDistance(Cell::Directions::Left, -countMovingLeft);
				
				if (this->cells[index].IsPrimaryJumpPoint(Cell::Directions::Left) == true)
				{
					countMovingLeft = 0;
					bJumpPointLastSeen = true;
				}
			}
		}
		
		{
			int countMovingRight = -1;
			bool bJumpPointLastSeen = false;
			for (int c = this->width - 1; c >= 0; --c)
			{
				const uint32_t index = c + (r * this->width);
				if (this->IsWall(r, c) == true)
				{
					countMovingRight = -1;
					bJumpPointLastSeen = false;
					this->cells[index].SetDistance(Cell::Directions::Right, 0);
					continue;
				}

				countMovingRight++;

				if (bJumpPointLastSeen == true)
					this->cells[index].SetDistance(Cell::Directions::Right, countMovingRight);				
				else //Wall last seen
					this->cells[index].SetDistance(Cell::Directions::Right, -countMovingRight);

				if (this->cells[index].IsPrimaryJumpPoint(Cell::Directions::Right) == true)
				{
					countMovingRight = 0;
					bJumpPointLastSeen = true;
				}
			}
		}
		
	}
}


void PreComputedMap::StraightUpDown(void)
{
	//Calculate distant jump points/walls (Up and Down)
	for (int c = 0; c < this->width; ++c)
	{
		{
			int countMovingUp = -1;
			bool bJumpPointLastSeen = false;
			for (int r = 0; r < this->height; ++r)
			{
				const uint32_t index = c + (r * this->width);
				if (this->IsWall(r, c) == true)
				{
					countMovingUp = -1;
					bJumpPointLastSeen = false;
					this->cells[index].SetDistance(Cell::Directions::Up, 0);
					continue;
				}

				countMovingUp++;

				if (bJumpPointLastSeen == true)
					this->cells[index].SetDistance(Cell::Directions::Up, countMovingUp);
				else //Wall last seen
					this->cells[index].SetDistance(Cell::Directions::Up, -countMovingUp);

				if (this->cells[index].IsPrimaryJumpPoint(Cell::Directions::Up) == true)
				{
					countMovingUp = 0;
					bJumpPointLastSeen = true;
				}
			}
		}

		{
			int countMovingDown = -1;
			bool bJumpPointLastSeen = false;
			for (int r = this->height - 1; r >= 0; --r)
			{
				const uint32_t index = c + (r * this->width);
				if (this->IsWall(r, c) == true)
				{
					countMovingDown = -1;
					bJumpPointLastSeen = false;
					this->cells[index].SetDistance(Cell::Directions::Down, 0);
					continue;
				}

				countMovingDown++;

				if (bJumpPointLastSeen == true)
					this->cells[index].SetDistance(Cell::Directions::Down, countMovingDown);
				else //Wall last seen
					this->cells[index].SetDistance(Cell::Directions::Down, -countMovingDown);

				if (this->cells[index].IsPrimaryJumpPoint(Cell::Directions::Down) == true)
				{
					countMovingDown = 0;
					bJumpPointLastSeen = true;
				}
			}
		}
	}
}

void PreComputedMap::MainDiagonal(void)
{
	//Calculate distant jump points/walls (Diagonally UpLeft and UpRight)
	for (int r = 0; r < this->height; ++r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			const uint32_t indexUpLeft = (c - 1) + ((r - 1) * this->width);
			const uint32_t indexUpRight = (c + 1) + ((r - 1) * this->width);
			if (this->IsEmpty(r, c) == true)
			{
				if (r == 0 || c == 0 || (this->IsWall(r - 1, c) == true || this->IsWall(r, c - 1) == true || this->IsWall(r - 1, c - 1) == true))
					this->cells[index].SetDistance(Cell::Directions::UpLeft, 0); //Wall one away
				else if (this->IsEmpty(r - 1, c) == true && this->IsEmpty(r, c - 1) == true && (this->cells[indexUpLeft].GetDistance(Cell::Directions::Up) > 0 || this->cells[indexUpLeft].GetDistance(Cell::Directions::Left) > 0))
					this->cells[index].SetDistance(Cell::Directions::UpLeft, 1); //Diagonal jump point one away
				else
				{
					//Increment from last	
					const int jumpDistance = this->cells[indexUpLeft].GetDistance(Cell::Directions::UpLeft);

					if (jumpDistance > 0)
						this->cells[index].SetDistance(Cell::Directions::UpLeft, 1 + jumpDistance);
					else 
						this->cells[index].SetDistance(Cell::Directions::UpLeft, -1 + jumpDistance);
				}


				if (r == 0 || c == (this->width - 1) || (this->IsWall(r - 1, c) == true || this->IsWall(r, c + 1) == true || this->IsWall(r - 1, c + 1) == true))
					this->cells[index].SetDistance(Cell::Directions::UpRight, 0); //Wall one away
				else if (this->IsEmpty(r - 1, c) == true && this->IsEmpty(r, c + 1) == true && (this->cells[indexUpRight].GetDistance(Cell::Directions::Up) > 0 || this->cells[indexUpRight].GetDistance(Cell::Directions::Right) > 0))
					this->cells[index].SetDistance(Cell::Directions::UpRight, 1); //Diagonal jump point one away
				else
				{
					//Increment from last	
					const int jumpDistance = this->cells[indexUpRight].GetDistance(Cell::Directions::UpRight);

					if (jumpDistance > 0)
						this->cells[index].SetDistance(Cell::Directions::UpRight, 1 + jumpDistance);
					else
						this->cells[index].SetDistance(Cell::Directions::UpRight, -1 + jumpDistance);
				}
			}
		}
	}
}

void PreComputedMap::SecondaryDiagonal(void)
{
	//Calculate distant jump points/walls (Diagonally DownLeft and DownRight)
	for (int r = this->height - 1; r >= 0; --r)
	{
		for (int c = 0; c < this->width; ++c)
		{
			const uint32_t index = c + (r * this->width);
			const uint32_t indexDownLeft = (c - 1) + ((r + 1) * this->width);
			const uint32_t indexDownRight = (c + 1) + ((r + 1) * this->width);
			if (this->IsEmpty(r, c) == true)
			{
				if (r == (this->height - 1) || c == 0 || (this->IsWall(r + 1, c) == true || this->IsWall(r, c - 1) == true || this->IsWall(r + 1, c - 1) == true))		
					this->cells[index].SetDistance(Cell::Directions::DownLeft, 0); //Wall one away
				else if (this->IsEmpty(r + 1, c) && this->IsEmpty(r, c - 1) && (this->cells[indexDownLeft].GetDistance(Cell::Directions::Down) > 0 || this->cells[indexDownLeft].GetDistance(Cell::Directions::Left) > 0))
					this->cells[index].SetDistance(Cell::Directions::DownLeft, 1); //Diagonal jump point one away
				else
				{
					//Increment from last
					const int jumpDistance = this->cells[indexDownLeft].GetDistance(Cell::Directions::DownLeft);

					if (jumpDistance > 0)
						this->cells[index].SetDistance(Cell::Directions::DownLeft, 1 + jumpDistance);
					else 
						this->cells[index].SetDistance(Cell::Directions::DownLeft, -1 + jumpDistance);
				}


				if (r == (this->height - 1) || c == (this->width - 1) || (this->IsWall(r + 1, c) == true || this->IsWall(r, c + 1) == true || this->IsWall(r + 1, c + 1) == true))
					this->cells[index].SetDistance(Cell::Directions::DownRight, 0); //Wall one away
				else if (this->IsEmpty(r + 1, c) == true && this->IsEmpty(r, c + 1) == true && (this->cells[indexDownRight].GetDistance(Cell::Directions::Down) > 0 || this->cells[indexDownRight].GetDistance(Cell::Directions::Right) > 0))
					this->cells[index].SetDistance(Cell::Directions::DownRight, 1); //Diagonal jump point one away
				else
				{
					//Increment from last
					const int jumpDistance = this->cells[indexDownRight].GetDistance(Cell::Directions::DownRight);

					if (jumpDistance > 0)
						this->cells[index].SetDistance(Cell::Directions::DownRight, 1 + jumpDistance);
					else
						this->cells[index].SetDistance(Cell::Directions::DownRight, -1 + jumpDistance);
				}
			}
		}
	}
}

inline bool PreComputedMap::IsWall(const int r, const int c) const
{
	const uint32_t rowBoundsCheck = r;
	const uint32_t colBoundsCheck = c;
	const uint32_t index = c + (r * this->width);
	return (rowBoundsCheck < (uint32_t) this->height && colBoundsCheck < (uint32_t) this->width) ? !(this->cells[index].IsTraversable()) : true;
}

bool PreComputedMap::IsPrimaryJumpPoint(const int r, const int c, const int rowDir, const int colDir) const
{
	return
		this->IsEmpty(r - rowDir, c - colDir) &&						//Parent not a wall (not necessary)
		((this->IsEmpty(r + colDir, c + rowDir) &&						//1st forced neighbor
		this->IsWall(r - rowDir + colDir, c - colDir + rowDir)) ||		//1st forced neighbor (continued)
		((this->IsEmpty(r - colDir, c - rowDir) &&						//2nd forced neighbor
		this->IsWall(r - rowDir - colDir, c - colDir - rowDir))));		//2nd forced neighbor (continued)
}
#pragma endregion
