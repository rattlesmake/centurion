#include "grid_graphic.h"
#include <imgui.h>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <chrono>

namespace
{
	//const unsigned int CELL_SIZE = 5; // 96;// 5;
	std::vector<unsigned char> PASS_MATRIX;
};

void GridGraphic::Initialize()
{
	// read image
	int imageXSize, imageYSize, n;
	unsigned char* gridImage = 0;// stbi_load("grid.png", &imageXSize, &imageYSize, &n, 0);

	this->gridWidth = imageXSize;
	this->gridHeight = imageYSize;
	const uint64_t size = ((uint64_t)this->gridWidth * (uint64_t)this->gridHeight) << 2;
	PASS_MATRIX = std::vector<unsigned char>(size, 0);
	this->pathfinding = new Pathfinding{ (int)gridHeight, (int)gridWidth };

	// fill vector
	for (size_t i = 0; i < (uint64_t)this->gridHeight * this->gridWidth; i++)
	{
		auto row = i / this->gridWidth; // y
		auto col = i % this->gridWidth; // x

		if ((int)gridImage[i * n] == 0) // passable
		{
			if ((row % 2 == 0 && col % 2 != 0) || (row % 2 != 0 && col % 2 == 0))
			{
				PASS_MATRIX[i * 4 + 0] = 100;
				PASS_MATRIX[i * 4 + 1] = 100;
				PASS_MATRIX[i * 4 + 2] = 100;
				PASS_MATRIX[i * 4 + 3] = 255;
			}
			else
			{
				PASS_MATRIX[i * 4 + 0] = 125;
				PASS_MATRIX[i * 4 + 1] = 125;
				PASS_MATRIX[i * 4 + 2] = 125;
				PASS_MATRIX[i * 4 + 3] = 255;
			}
		}
		else
		{
			this->SetCellAsNotPassable(i, col, row, false);
		}
	}

	//stbi_image_free(gridImage);

	this->GenerateTexture();
	this->StartIndex = 0;
	this->EndIndex = this->gridHeight * this->gridWidth - 1;
	this->SetCellAsStartPoint(0, 0, 0);
	this->SetCellAsEndPoint(this->gridHeight * this->gridWidth - 1, this->gridWidth - 1, this->gridHeight - 1);
	this->UpdateTexture();
}

void GridGraphic::Render(unsigned int windowWidth, unsigned int windowHeight)
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Grid", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Image((void*)(intptr_t)this->TextureID, ImVec2((float)windowWidth, (float)windowHeight));

	this->KeyboardInput();

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
	{
		auto pos = ImGui::GetMousePos();
		auto btn = ImGui::IsItemClicked(ImGuiMouseButton_Left) ? 0 : 1;
		this->MouseInput(pos.x, pos.y, (float)windowWidth, (float)windowHeight, btn);
	}
	if (!PRE_COMPUTATION_DONE && ImGui::IsKeyPressed(GLFW_KEY_SPACE))
	{
		this->StartPreComputation();
		PRE_COMPUTATION_DONE = true;
	}
	if (PRE_COMPUTATION_DONE && ImGui::IsKeyPressed(GLFW_KEY_ENTER))
	{
		this->CalculatePath();
	}

	ImGui::PopStyleVar();
	ImGui::End();
}

GridGraphic::~GridGraphic()
{
	if (this->pathfinding != nullptr)
		delete pathfinding;
}

void GridGraphic::MouseInput(float xPos, float yPos, float windowWidth, float windowHeight, int btn)
{
	auto relPosX = xPos / windowWidth;
	auto relPosY = yPos / windowHeight;
	auto gridPosX = (int)(relPosX * this->gridWidth);
	auto gridPosY = (int)(relPosY * this->gridHeight);
	auto gridPos = gridPosX + gridPosY * this->gridWidth;

	if (ALT_KEEP_PRESSED)
	{
		if (btn == 0) // left
		{
			this->SetCellAsNotPassable(gridPos, gridPosX, gridPosY);
		}
		else // right
		{
			this->SetCellAsPassable(gridPos, gridPosX, gridPosY);
		}
	}
	else
	{
		if (btn == 0) // left
		{
			this->SetCellAsStartPoint(gridPos, gridPosX, gridPosY);
		}
		else // right
		{
			this->SetCellAsEndPoint(gridPos, gridPosX, gridPosY);
		}
	}
}

void GridGraphic::SetCellAsNotPassable(unsigned int index, unsigned int x, unsigned int y, bool updateTex)
{
	const uint64_t newIndex = (uint64_t)index << 2;
	PASS_MATRIX[newIndex + 0] = 0;
	PASS_MATRIX[newIndex + 1] = 0;
	PASS_MATRIX[newIndex + 2] = 0;
	PASS_MATRIX[newIndex + 3] = 255;
	if (updateTex) this->UpdateTexture();
	this->pathfinding->GetPreComputedMap().AddWall(index);
}

void GridGraphic::SetCellAsPassable(unsigned int index, unsigned int x, unsigned int y)
{
	this->ClearCell(index);
}

void GridGraphic::SetCellAsPathPoint(unsigned int index, unsigned int x, unsigned int y)
{
	const uint64_t newIndex = (uint64_t)index << 2;
	PASS_MATRIX[newIndex + 0] = 255;
	PASS_MATRIX[newIndex + 1] = 255;
	PASS_MATRIX[newIndex + 2] = 255;
	PASS_MATRIX[newIndex + 3] = 255;
	this->UpdateTexture();
}

void GridGraphic::SetCellAsStartPoint(unsigned int index, unsigned int x, unsigned int y)
{
	this->ClearCell(this->StartIndex);
	const uint64_t newIndex = (uint64_t)index << 2;
	PASS_MATRIX[newIndex + 0] = 0;
	PASS_MATRIX[newIndex + 1] = 255;
	PASS_MATRIX[newIndex + 2] = 0;
	PASS_MATRIX[newIndex + 3] = 255;
	this->StartIndex = index;
	this->StartX = x;
	this->StartY = y;
	this->UpdateTexture();
}

void GridGraphic::SetCellAsEndPoint(unsigned int index, unsigned int x, unsigned int y)
{
	this->ClearCell(this->EndIndex);
	const uint64_t newIndex = (uint64_t)index << 2;
	PASS_MATRIX[newIndex + 0] = 255;
	PASS_MATRIX[newIndex + 1] = 0;
	PASS_MATRIX[newIndex + 2] = 0;
	PASS_MATRIX[newIndex + 3] = 255;
	this->EndIndex = index;
	this->EndX = x;
	this->EndY = y;
	this->UpdateTexture();
}

void GridGraphic::ClearCell(unsigned int index)
{
	auto row = index / this->gridWidth;
	auto col = index % this->gridWidth;
	const uint64_t newIndex = (uint64_t)index << 2;;
	if ((row % 2 == 0 && col % 2 != 0) || (row % 2 != 0 && col % 2 == 0))
	{
		PASS_MATRIX[newIndex + 0] = 100;
		PASS_MATRIX[newIndex + 1] = 100;
		PASS_MATRIX[newIndex + 2] = 100;
		PASS_MATRIX[newIndex + 3] = 255;
	}
	else
	{
		PASS_MATRIX[newIndex + 0] = 125;
		PASS_MATRIX[newIndex + 1] = 125;
		PASS_MATRIX[newIndex + 2] = 125;
		PASS_MATRIX[newIndex + 3] = 255;
	}
	this->UpdateTexture();
}

void GridGraphic::KeyboardInput()
{
	if (ImGui::IsKeyPressed(GLFW_KEY_LEFT_ALT))
	{
		ALT_KEEP_PRESSED = true;
	}
	if (ALT_KEEP_PRESSED && ImGui::IsKeyReleased(GLFW_KEY_LEFT_ALT))
	{
		ALT_KEEP_PRESSED = false;
	}
}

void GridGraphic::StartPreComputation()
{
	std::cout << "Starting pre computation" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	this->pathfinding->GetPreComputedMap().PerformPreComputation();
	auto end = std::chrono::high_resolution_clock::now();
	auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Stop path precomputation after " << executionTime << std::endl;
}

void GridGraphic::CalculatePath()
{
	std::cout << "Starting path computation" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();
	std::list<glm::ivec2> path = this->pathfinding->GetPath(glm::ivec2{ this->StartX,this->StartY }, glm::ivec2{ this->EndX,this->EndY });
	auto end = std::chrono::high_resolution_clock::now();
	if (path.size() > 0)
	{
		path.pop_front();
		if (path.size() > 0)
			path.pop_back();
		for (auto const& cell : path)
		{
			auto gridPosX = (int)(cell.x);
			auto gridPosY = (int)(cell.y);
			auto gridPos = gridPosX + gridPosY * this->gridWidth;
			this->SetCellAsPathPoint(gridPos, gridPosX, gridPosY);
		}
	}
	else
		std::cout << "Path not found" << std::endl;
	auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Stop path computation after " << executionTime << std::endl;
}

void GridGraphic::GenerateTexture()
{
	glGenTextures(1, &this->TextureID);
	glBindTexture(GL_TEXTURE_2D, this->TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->gridWidth, this->gridHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, PASS_MATRIX.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GridGraphic::UpdateTexture()
{
	glBindTexture(GL_TEXTURE_2D, this->TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, this->gridWidth, this->gridHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, PASS_MATRIX.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}
