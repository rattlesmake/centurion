/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include "cell.h"

class PreComputedMap
{
public:
	PreComputedMap(const int _height, const int _width);

	#pragma region Getters:
	[[nodiscard]] uint32_t GetHeight(void) const;
	[[nodiscard]] uint32_t GetWidth(void) const;
	#pragma endregion

	#pragma region Public members:
	void AddWall(const uint32_t index); 
	void AddWall(const uint32_t row, const uint32_t col); 
	void PerformPreComputation(void);
	[[nodiscard]] const Cell* const  GetCell(const uint32_t r, const uint32_t c) const;
	[[nodiscard]] bool IsEmpty(const int r, const int c) const;
	#pragma endregion
private:
	vector<Cell> cells;
	int height;
	int width;

	#pragma region Private members:
	void IdentifyPrimaryJumpPoint(void);
	void ComputeDistances(void);
	void StraightLeftRight(void);
	void StraightUpDown(void);
	void MainDiagonal(void);
	void SecondaryDiagonal(void);
	[[nodiscard]] inline bool IsWall(const int r, const int c) const;
	[[nodiscard]] bool IsPrimaryJumpPoint(const int r, const int c, const int rowDir, const int colDir) const;
	#pragma endregion
};
