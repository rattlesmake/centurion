/*
* ---------------------------
* CLASS READER AND EDITOR FOR CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <GL/glew.h>
#include <pathfinding.h>

class GridGraphic
{
public:
	void Initialize();
	void Render(unsigned int windowWidth, unsigned int windowHeight);
	~GridGraphic();
private:
	unsigned int gridWidth;
	unsigned int gridHeight;
	GLuint TextureID;
	void StartPreComputation();
	void CalculatePath();
	void GenerateTexture();
	void UpdateTexture();
	void MouseInput(float xPos, float yPos, float windowWidth, float windowHeight, int btn);
	void SetCellAsNotPassable(unsigned int index, unsigned int x, unsigned int y, bool updateTex = true);
	void SetCellAsPassable(unsigned int index, unsigned int x, unsigned int y);
	void SetCellAsPathPoint(unsigned int index, unsigned int x, unsigned int y);
	void SetCellAsStartPoint(unsigned int index, unsigned int x, unsigned int y);
	void SetCellAsEndPoint(unsigned int index, unsigned int x, unsigned int y);
	void ClearCell(unsigned int index);

	unsigned int StartIndex, StartX, StartY, EndIndex, EndX, EndY;

	Pathfinding* pathfinding;

	void KeyboardInput();
	bool ALT_KEEP_PRESSED = false;
	bool PRE_COMPUTATION_DONE = false;
};