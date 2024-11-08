/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

/// <summary>
/// 
/// This namespace contains all methods about the file GRID of each object
/// and the GRID MATRIX (SurfaceGrid), a huge matrix of 0/1 that indicates if that point
/// is passable or not. The idea is to store every GRID file in a std::map 
/// where the key is the class name. In each object there will be a pointer 
/// to the corresponding GRID file.
/// 
/// The GRID file contains hexadecimal (HEX) values. They are converted to 
/// binary (e.g. A = 1010) in order to obtain four matrix cells.
/// 
/// The map GRID grids (or matrices) are (1D) arrays of uint8_t values.
/// To obtain a value using a couple of coordinates (x,y) we perform an
/// arithmetic operation (every 2D matrix can be converted to 1D array).
/// 
/// It's very important to set correctly the const size values.
/// 
/// The arrays, that are quite onerous and costly in terms of memory, are 
/// stored in the .cpp file, in the anonymous namespace. We get access 
/// to those arrays using pointer methods.
/// 
/// </summary>

#ifndef GRID_CELL_SIZE
#define GRID_CELL_SIZE 32
#endif

#include <stdint.h>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>
#include <utility>  //std::pair<T1, T2>
#include <png.h>
#include <glm.hpp>  //glm::vec2

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cell.h>

class Grid;
class Point;
class SurfaceGridCluster;
//namespace rattlesmake { namespace image { class png; }; };

enum class GridOperations : uint8_t
{
	E_GRID_CLEAR = 0,          //To mark one or more cells as passable
	E_GRID_OVERLAP = 1,        //Indicate to apply only "not passable" values of a GObject grid
};

class SurfaceGrid : public rattlesmake::image::png
{
public:

	static std::shared_ptr<SurfaceGrid> Create(const glm::vec2& _visibleMapSize, const std::string& _mapSize);

	SurfaceGrid(const SurfaceGrid& other) = delete;
	SurfaceGrid& operator=(const SurfaceGrid& other) = delete;
	~SurfaceGrid(void);

	[[nodiscard]] uint32_t GetGridSizeX(void) const;
	[[nodiscard]] uint32_t GetGridSizeY(void) const;
	[[nodiscard]] uint32_t GetGridArraySize(void) const;
	[[nodiscard]] const std::vector<CellValues>& GetGridCRef(void) const;
	[[nodiscard]] uint32_t GetClusterGridSizeX(void) const;
	[[nodiscard]] uint32_t GetClusterGridSizeY(void) const;
	[[nodiscard]] uint32_t GetClustersNumber(void) const;
	[[nodiscard]] SurfaceGridCluster& GetClusterRefByCellIndex(const uint32_t gridCellIndex);
	[[nodiscard]] SurfaceGridCluster& GetClusterRefByCellCoords(const uint32_t xGrid, const uint32_t yGrid);

	void ClearGrid(void);
	void RenderGrid(void);

	/// <summary>
	/// This function updates the grid of the game map.
	/// This function must be called one time after reading/generating a map 
	/// and called each time an object is inserted into the editor
	/// </summary>
	void UpdateGrid(void);
	void EnableGrid(void);
	void DisableGrid(void);
	[[nodiscard]] bool IsGridEnabled(void) const;

	void UpdateObjectGrid(const std::shared_ptr<Grid>& pg, const Point& gobjPos, const GridOperations operationToDo);
	[[nodiscard]] bool CheckObjectGridAvailability(const std::shared_ptr<Grid>& pg, const Point& gobjPos);
	[[nodiscard]] bool IsPointAvailableFromMapCoords(const uint32_t xMap, const uint32_t yMap) const;

	void SetGridCellValueByMapCoords(const uint32_t xMap, const uint32_t yMap, const bool bPassable, const bool bFourVertexAround, const GridOperations operationToDo);
	[[nodiscard]] std::optional<uint32_t> TryGetIndexByMapCoords(const uint32_t xMap, const uint32_t yMap);

	void FindClusteresEntrances(void);

	[[nodiscard]] uint32_t XGridToXCluster(const uint32_t xGrid) const;
	[[nodiscard]] uint32_t YGridToYCluster(const uint32_t yGrid) const;

protected:

	explicit SurfaceGrid(const glm::vec2& _visibleMapSize, const std::string& _mapSize);

	// inherited by png
	void update_texture(const uint8_t* image_data) override;
	uint32_t bind_texture(const uint8_t* image_data, const uint32_t width, const uint32_t height) override;

private:
	[[nodiscard]] CellValues GetGridValueByIndex(const uint32_t idx) const;
	void SetGridCellValueByIndex(const uint32_t index, const CellValues value);
	[[nodiscard]] CellValues GetGridValueFromCoordinates(const uint32_t xMap, const uint32_t yMap) const;
	void SetGridCellValueByCoordinates(const uint32_t xMap, const uint32_t yMap, const CellValues val);
	[[nodiscard]] std::optional<std::pair<uint32_t, uint32_t>> GetGridCellPosition(const Point& position, const uint32_t horizontal_size, const uint32_t vertical_size);
	void CreateGridTexture(void);


	const uint32_t GRID_SIZE_X;
	const uint32_t GRID_SIZE_Y;
	const uint32_t GRID_ARRAY_SIZE;
	const uint32_t CLUSTER_GRID_SIZE_X;
	const uint32_t CLUSTER_GRID_SIZE_Y;
	glm::vec2 visibleMapSize;
	std::string mapSize;

	std::vector<CellValues> surfaceGridMatrix;
	std::vector<std::vector<SurfaceGridCluster>> clusters;

	bool bIsEnabled = false;

	std::mutex m;
};
