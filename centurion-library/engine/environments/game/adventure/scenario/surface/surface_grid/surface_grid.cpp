#include "surface_grid.h"

#include <png_shader.h>
#include <engine.h>
#include <encode_utils.h>
#include <services/logservice.h>
#include <fileservice.h>
#include <zipservice.h>
#include <mouse.h>
#include <viewport.h>
#include <camera.h>
#include <environments/game/classes/objectsStuff/objectGrid.h>
#include <environments/game/classes/objectsStuff/objectPoint.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster.h>

/*
* Grid sizes:
*	Small    -->   440 x 280     -->   gcd(440, 280) = 40
*	Medium   -->   920 x 600     -->   gcd(920, 600) = 40
*	Big      -->   1880 x 1240   -->   gcd(1880, 1240) = 40
*/

#ifndef CLUSTERS_NUMBER_IN_X
#define CLUSTERS_NUMBER_IN_X        40   //How much cluster would like to have along X
#endif // !CLUSTERS_NUMBER_IN_X

#ifndef CLUSTERS_NUMBER_IN_Y
#define CLUSTERS_NUMBER_IN_Y        40   //How much cluster would like to have along Y
#endif // !CLUSTERS_NUMBER_IN_Y

#ifndef CLUSTERS_NUMBER
#define CLUSTERS_NUMBER       CLUSTERS_NUMBER_IN_X * CLUSTERS_NUMBER_IN_Y  //40 x 40 = 1600 clusters in the original grid
#endif // !CLUSTERS_NUMBER

/*
* If clusters number = 1600 (40*40) then each cluster has a size of:
*	Small    -->   11 x 7
*	Medium   -->   23 x 15
*	Big      -->   47 x 31
*/

#pragma region Constructor and destructor
SurfaceGrid::SurfaceGrid(const glm::vec2& _visibleMapSize, const std::string& _mapSize) :
	visibleMapSize(_visibleMapSize),
	GRID_SIZE_X(static_cast<uint32_t>(_visibleMapSize.x / GRID_CELL_SIZE)),
	GRID_SIZE_Y(static_cast<uint32_t>(_visibleMapSize.y / GRID_CELL_SIZE)),
	GRID_ARRAY_SIZE(GRID_SIZE_X * GRID_SIZE_Y),
	CLUSTER_GRID_SIZE_X(GRID_SIZE_X / CLUSTERS_NUMBER_IN_X),
	CLUSTER_GRID_SIZE_Y(GRID_SIZE_Y / CLUSTERS_NUMBER_IN_Y),
	surfaceGridMatrix(GRID_ARRAY_SIZE),
	mapSize(_mapSize)
{
	assert((GRID_SIZE_X % CLUSTERS_NUMBER_IN_X) == 0);
	assert((GRID_SIZE_Y % CLUSTERS_NUMBER_IN_Y) == 0);

	//Create clusters
	for (uint32_t r = 0; r < CLUSTERS_NUMBER_IN_Y; ++r) //y
	{
		this->clusters.push_back(std::vector<SurfaceGridCluster>());
		for (uint32_t c = 0; c < CLUSTERS_NUMBER_IN_X; ++c) //x
		{
			const uint32_t index = c + CLUSTERS_NUMBER_IN_X * r;
			this->clusters[r].push_back(SurfaceGridCluster{ (*this), index });
		}
		this->clusters[r].shrink_to_fit();
	}
	this->clusters.shrink_to_fit();

	// cil_image properties for a correct render
	this->rgb255 = true;
	this->bVerticallyFlipped = true;
	this->alpha = 0.5f;
}

std::shared_ptr<SurfaceGrid> SurfaceGrid::Create(const glm::vec2& _visibleMapSize, const std::string& _mapSize)
{
	std::shared_ptr<SurfaceGrid> new_grid = std::shared_ptr<SurfaceGrid>(new SurfaceGrid(_visibleMapSize, _mapSize));
	new_grid->me = new_grid;
	new_grid->CreateGridTexture();
	return new_grid;
}
SurfaceGrid::~SurfaceGrid(void)
{
}
#pragma endregion

#pragma region Public members:
uint32_t SurfaceGrid::GetGridSizeX(void) const
{
	return this->GRID_SIZE_X;
}

uint32_t SurfaceGrid::GetGridSizeY(void) const
{
	return this->GRID_SIZE_Y;
}

uint32_t SurfaceGrid::GetGridArraySize(void) const
{
	return this->GRID_ARRAY_SIZE;
}

const std::vector<CellValues>& SurfaceGrid::GetGridCRef(void) const
{
	return this->surfaceGridMatrix;
}

uint32_t SurfaceGrid::GetClusterGridSizeX(void) const
{
	return this->CLUSTER_GRID_SIZE_X;
}

uint32_t SurfaceGrid::GetClusterGridSizeY(void) const
{
	return this->CLUSTER_GRID_SIZE_Y;
}

uint32_t SurfaceGrid::GetClustersNumber(void) const
{
	return this->CLUSTER_GRID_SIZE_X * this->CLUSTER_GRID_SIZE_Y;
}

SurfaceGridCluster& SurfaceGrid::GetClusterRefByCellIndex(const uint32_t gridCellIndex)
{
	const uint32_t row = gridCellIndex / this->GRID_SIZE_X;      //y
	const uint32_t column = gridCellIndex % this->GRID_SIZE_X;   //x
	assert(gridCellIndex < this->GRID_ARRAY_SIZE);
	return this->GetClusterRefByCellCoords(column, row);
}

SurfaceGridCluster& SurfaceGrid::GetClusterRefByCellCoords(const uint32_t xGrid, const uint32_t yGrid)
{
	const uint32_t clusterY = yGrid / this->CLUSTER_GRID_SIZE_Y; //Matrix row
	const uint32_t clusterX = xGrid / this->CLUSTER_GRID_SIZE_X; //Matrix column
	return this->clusters[clusterY][clusterX];
}

void SurfaceGrid::ClearGrid(void)
{
	//In each cluster of the grid, mark each cell as free
	for (uint32_t r = 0; r < CLUSTERS_NUMBER_IN_Y; ++r) //y
	{
		for (uint32_t c = 0; c < CLUSTERS_NUMBER_IN_X; ++c) //x
		{
			this->clusters[r][c].MakeFullyFree();
		}
	}

	//Mark each cell as free in the main grid
	for (uint32_t i = 0; i < this->GRID_ARRAY_SIZE; i++)
	{
		this->surfaceGridMatrix[i] = CellValues::E_FREE;
	}
}

void SurfaceGrid::RenderGrid(void)
{
	if (this->IsGridEnabled() == false) 
		return;
	
	auto& imageShader = rattlesmake::image::png_shader::get_instance();
	auto& camera = rattlesmake::peripherals::camera::get_instance();
	auto& viewport = rattlesmake::peripherals::viewport::get_instance();

	auto imageWidth = static_cast<uint32_t>(this->visibleMapSize.x);
	auto imageHeight = static_cast<uint32_t>(this->visibleMapSize.y);
	auto flag = rattlesmake::image::png_flags_::png_flags_NoRepeat;
	
	// render
	imageShader.apply_matrices(camera.GetProjectionMatrix(), camera.GetViewMatrix());
	this->render(0, 0, flag, imageWidth, imageHeight);
	imageShader.apply_matrices(viewport.GetProjectionMatrix(), viewport.GetViewMatrix());
}

void SurfaceGrid::UpdateGrid(void)
{
	if (this->textureData.expired())
		this->CreateGridTexture();

	this->update_texture((uint8_t*)this->surfaceGridMatrix.data());
}

void SurfaceGrid::EnableGrid(void)
{
	this->bIsEnabled = true;
}

void SurfaceGrid::DisableGrid(void)
{
	this->bIsEnabled = false;
}

bool SurfaceGrid::IsGridEnabled(void) const
{
	return this->bIsEnabled;
}

void SurfaceGrid::UpdateObjectGrid(const std::shared_ptr<Grid>& pg, const Point& gobjPos, const GridOperations operationToDo)
{
	//Grid not available
	if (!pg)
		return;

	//Get the size of the current grid.
	const uint32_t horizontal_size = pg->GetSizeX();
	const uint32_t vertical_size = pg->GetSizeY();
	const auto pos = this->GetGridCellPosition(gobjPos, horizontal_size, vertical_size);
	
	//if (pos.has_value() == false)
	//	return;
	assert(pos.has_value());
	
	const uint32_t x = pos.value().first;
	const uint32_t y = pos.value().second;

	for (uint32_t iy = 0; iy < vertical_size; ++iy)
	{
		for (uint32_t ix = 0; ix < horizontal_size; ++ix)
		{
			const uint32_t yOffset = (vertical_size - iy + static_cast<int>(y));
			const uint32_t xOffset = ix + static_cast<int>(x);
			const uint32_t idx = yOffset * this->GRID_SIZE_X + xOffset;
			if (idx >= this->GRID_ARRAY_SIZE)
				continue;
			try
			{
				if (pg->GetValueByCoordinates(ix, iy) == CellValues::E_WALL)
				{
					SurfaceGridCluster& cluster = this->GetClusterRefByCellCoords(xOffset, yOffset);
					if (operationToDo == GridOperations::E_GRID_CLEAR)
					{
						this->surfaceGridMatrix.at(idx) = CellValues::E_FREE;
						cluster.SetCellValue(this->XGridToXCluster(xOffset), this->YGridToYCluster(yOffset), CellValues::E_FREE);
					}
					else if (operationToDo == GridOperations::E_GRID_OVERLAP)
					{
						this->surfaceGridMatrix.at(idx) = CellValues::E_WALL;
						cluster.SetCellValue(this->XGridToXCluster(xOffset), this->YGridToYCluster(yOffset), CellValues::E_WALL);
					}
				}
			}
			catch (const std::exception& ex)
			{
				#if CENTURION_DEBUG_MODE
				std::cout << "[DEBUG] x = " << std::to_string(xOffset) << " ; y = " << std::to_string(yOffset) << " --> " << ex.what() << std::endl;
				#endif // CENTURION_DEBUG_MODE
				throw ex;
			}
		}
	}
}

bool SurfaceGrid::CheckObjectGridAvailability(const std::shared_ptr<Grid>& pg, const Point& gobjPos)
{
	if (!pg) //Grid not available (GObject doesn't have a grid)
		return true; //In this case, consider GObject as ALWAYS placeable

	const uint32_t horizontal_size = pg->GetSizeX();
	const uint32_t vertical_size = pg->GetSizeY();
	const auto pos = this->GetGridCellPosition(gobjPos, horizontal_size, vertical_size);
	if (pos.has_value() == false)
		return false;
	const uint32_t x = pos.value().first;
	const uint32_t y = pos.value().second;

	if (x + horizontal_size >= this->GRID_SIZE_X || y + vertical_size >= this->GRID_SIZE_Y)
		return false;

	bool bAvailable = true;
	for (uint32_t iy = 0; iy < vertical_size && bAvailable == true; iy++)
	{
		for (uint32_t ix = 0; ix < horizontal_size && bAvailable == true; ix++)
		{
			const uint32_t idx = (vertical_size - iy + static_cast<int>(y)) * this->GRID_SIZE_X + ix + static_cast<int>(x);
			if (idx >= this->GRID_ARRAY_SIZE || (pg->GetValueByCoordinates(ix, iy) == CellValues::E_WALL && this->surfaceGridMatrix.at(idx) == CellValues::E_WALL) )
				bAvailable = false;
		}
	}
	return bAvailable;
}

bool SurfaceGrid::IsPointAvailableFromMapCoords(const uint32_t xMap, const uint32_t yMap) const
{
	auto c = GetGridValueFromCoordinates(xMap, yMap);
	return c == CellValues::E_FREE;
}

void SurfaceGrid::SetGridCellValueByMapCoords(const uint32_t xMap, const uint32_t yMap, const bool bPassable, const bool bFourVertexAround, const GridOperations operationToDo)
{
	const CellValues value = bPassable ? CellValues::E_FREE : CellValues::E_WALL;
	const std::optional<uint32_t> idxOpt = this->TryGetIndexByMapCoords(xMap, yMap);
	if (idxOpt.has_value() == false)
		return;
	const uint32_t index = idxOpt.value();
	if (index == 0)
		return;

	//Set the vertex cell 
	this->SetGridCellValueByIndex(index, value);

	if (bFourVertexAround == true)
	{
		//We set 4 cells around the vertex
		const bool bLeftMargin = (index % this->GRID_SIZE_X) > 0;
		const bool bBottomMargin = (index / this->GRID_SIZE_X) > 0;
		if (bLeftMargin) //left
			this->SetGridCellValueByIndex(index - 1, value);
		if (bBottomMargin) //bottom
			this->SetGridCellValueByIndex(index - this->GRID_SIZE_X, value);
		if (bBottomMargin && bLeftMargin) //bottom-left
			this->SetGridCellValueByIndex(index - 1 - this->GRID_SIZE_X, value);
	}
}

std::optional<uint32_t> SurfaceGrid::TryGetIndexByMapCoords(const uint32_t xMap, const uint32_t yMap)
{
	const uint32_t ix = xMap / GRID_CELL_SIZE;
	const uint32_t iy = yMap / GRID_CELL_SIZE;
	if (ix >= this->GRID_SIZE_X || iy >= this->GRID_SIZE_Y)
		return std::nullopt;
	const uint32_t idx = ix + iy * this->GRID_SIZE_X;
	if (idx >= this->GRID_ARRAY_SIZE)
		return std::nullopt;
	return idx;
}

void SurfaceGrid::FindClusteresEntrances(void)
{
	for (uint32_t y = 0; y < CLUSTERS_NUMBER_IN_Y - 1; ++y) //rows
	{
		for (uint32_t x = 0; x < CLUSTERS_NUMBER_IN_X - 1; ++x) //columns
		{
			SurfaceGridCluster& currentCluster = this->clusters[y][x];
			auto& currentClusterGrid = currentCluster.GetGridCRef();
			//Determines the entrances between the current cluster and the one to its right
			{
				SurfaceGridCluster& rightCluster = this->clusters[y][static_cast<uint64_t>(x) + 1];
				auto& rightClusterGrid = rightCluster.GetGridCRef();
				std::optional<uint32_t> clusterEntranceY;
				for (uint32_t clusterY = 0; clusterY < this->CLUSTER_GRID_SIZE_Y; clusterY++)
				{
					if (currentClusterGrid[clusterY][this->CLUSTER_GRID_SIZE_X - 1].IsFree() == true && rightClusterGrid[clusterY][0].IsFree() == true)
					{
						if (clusterEntranceY.has_value() == false)
						{
							clusterEntranceY = clusterY;
							currentCluster.SetRightEntrance(clusterEntranceY.value());
							rightCluster.SetLeftEntrance(clusterEntranceY.value());
						}
					}
					else if (clusterEntranceY.has_value() == true)
					{
						//Reset entrance in order to try to found a new entrance
						clusterEntranceY = std::nullopt;
					}
				}
			}

			//Determines the entrances between the current cluster and the one below it
			{
				auto& bottomCluster = this->clusters[static_cast<uint64_t>(y) + 1][x];
				auto& bottomClusterGrid = bottomCluster.GetGridCRef();
				std::optional<uint32_t> clusterEntranceX;
				for (uint32_t clusterX = 0; clusterX < this->CLUSTER_GRID_SIZE_X; clusterX++)
				{
					if (currentClusterGrid[this->CLUSTER_GRID_SIZE_Y - 1][clusterX].IsFree() == true && bottomClusterGrid[0][clusterX].IsFree() == true)
					{
						if (clusterEntranceX.has_value() == false)
						{
							clusterEntranceX = clusterX;
							currentCluster.SetBottomEntrance(clusterEntranceX.value());
							bottomCluster.SetUpEntrance(clusterEntranceX.value());
						}
					}
					else if (clusterEntranceX.has_value() == true)
					{
						//Reset entrance in order to try to found a new entrance
						clusterEntranceX = std::nullopt;
					}
				}
			}
		}
	}
}

uint32_t SurfaceGrid::XGridToXCluster(const uint32_t xGrid) const
{
	return xGrid % this->CLUSTER_GRID_SIZE_X;
}

uint32_t SurfaceGrid::YGridToYCluster(const uint32_t yGrid) const
{
	return yGrid % this->CLUSTER_GRID_SIZE_Y;
}
#pragma endregion


#pragma region Private members:
CellValues SurfaceGrid::GetGridValueByIndex(const uint32_t idx) const
{
	assert(idx < this->GRID_ARRAY_SIZE);
	return this->surfaceGridMatrix.at(idx);
}

void SurfaceGrid::SetGridCellValueByIndex(const uint32_t index, const CellValues value)
{
	if (index >= this->GRID_ARRAY_SIZE)
		return;
	const std::lock_guard<std::mutex> lg{ this->m }; //Here we need a lock since some classes like TerrainBrush use more than one thread to apply textures
	this->surfaceGridMatrix.at(index) = value;
	const uint32_t xGrid = (index % this->GRID_SIZE_X);
	const uint32_t yGrid = (index / this->GRID_SIZE_X);
	this->GetClusterRefByCellIndex(index).SetCellValue(xGrid, yGrid, value);
}

CellValues SurfaceGrid::GetGridValueFromCoordinates(const uint32_t xMap, const uint32_t yMap) const
{
	if (yMap == 0 || xMap == 0)
		return CellValues::E_FREE;
	const uint32_t idx = yMap / GRID_CELL_SIZE * this->GRID_SIZE_X + xMap / GRID_CELL_SIZE;
	if (idx >= this->GRID_ARRAY_SIZE)
		return CellValues::E_FREE;
	return this->surfaceGridMatrix[idx];
}

void SurfaceGrid::SetGridCellValueByCoordinates(const uint32_t xMap, const uint32_t yMap, const CellValues val)
{
	const uint32_t idx = (this->GRID_SIZE_X * yMap + xMap);
	this->SetGridCellValueByIndex(idx, val);
}

std::optional<std::pair<uint32_t, uint32_t>> SurfaceGrid::GetGridCellPosition(const Point& gObjPos, const uint32_t horizontal_size, const uint32_t vertical_size)
{
	//These computation can produce negative numbers, so we must use int32_t instead of uint32_t
	const int32_t xGrid = static_cast<int32_t>(gObjPos.x) / GRID_CELL_SIZE - static_cast<int32_t>(horizontal_size >> 1);
	const int32_t yGrid = static_cast<int32_t>(gObjPos.y) / GRID_CELL_SIZE - static_cast<int32_t>(vertical_size >> 1);
	if (xGrid < 0 || yGrid < 0)
		return std::nullopt;
	return std::make_pair(xGrid, yGrid);
}

void SurfaceGrid::CreateGridTexture(void)
{
	this->stringID = "##grid_" + this->mapSize;
	auto& imageShader = rattlesmake::image::png_shader::get_instance();
	uint32_t width = this->GetGridSizeX();
	uint32_t height = this->GetGridSizeY();
	const uint8_t* data = (uint8_t*)this->surfaceGridMatrix.data();

	if (imageShader.check_if_png_in_cache(this->stringID) == false)
	{
		int w = 0, h = 0, n = 0;
		this->textureData = imageShader.add_image_data(this->bind_texture(data, width, height), this->stringID, width, height, 3);
	}
	else
	{
		this->textureData = imageShader.get_image_data(this->stringID);
	}
}
void SurfaceGrid::update_texture(const uint8_t* image_data)
{
	auto texData = this->textureData.lock();
	if (texData)
	{
		glBindTexture(GL_TEXTURE_2D, texData->get_opengl_texture_id());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texData->get_image_width(), texData->get_image_height(), GL_RED, GL_UNSIGNED_BYTE, image_data);
	}
}
uint32_t SurfaceGrid::bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height)
{
	uint32_t textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}
#pragma endregion
