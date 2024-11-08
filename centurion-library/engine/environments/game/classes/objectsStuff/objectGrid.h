/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cell.h>

/// <summary>
/// GObject grid
/// </summary>
class Grid
{
public:
	Grid(const Grid& other) = delete;
	Grid& operator=(const Grid& other) = delete;
	~Grid(void);

	[[nodiscard]] uint32_t GetSizeX(void) const;
	[[nodiscard]] uint32_t GetSizeY(void) const;
	[[nodiscard]] CellValues GetValueByCoordinates(const uint32_t X, const uint32_t Y);

	[[nodiscard]] static std::shared_ptr<Grid> GetOrReadGrid(const std::string& path, const std::string& className);
	[[nodiscard]] static std::shared_ptr<Grid> GetGrid(const std::string& className);
private:
	Grid(std::string _className, std::vector<CellValues>&& _gridData, const uint32_t _sizeX, const uint32_t _sizeY);

	[[nodiscard]] static std::tuple<uint32_t, uint32_t, std::vector<CellValues>> ReadGridFromXml(const std::string& path);
	static bool RemoveGrid(const std::string& className);

	static std::unordered_map<std::string, std::weak_ptr<Grid>> gridCache;

	std::string className;
	std::vector<CellValues> gridData;
	uint32_t sizeX = 0;
	uint32_t sizeY = 0;
};
