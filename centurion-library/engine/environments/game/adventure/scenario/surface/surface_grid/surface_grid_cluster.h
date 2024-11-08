/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <vector>
#include <unordered_set>

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid_cluster_entrance.h>


class SurfaceGridCluster
{
public:
	SurfaceGridCluster(const SurfaceGrid& _surface, const uint32_t _clusterIndex);
	SurfaceGridCluster(const SurfaceGridCluster& other) = delete;
	SurfaceGridCluster& operator=(const SurfaceGridCluster& other) = delete;
	SurfaceGridCluster(SurfaceGridCluster&& other) noexcept;
	~SurfaceGridCluster(void);

	void SetCellValue(const uint32_t xCluster, const uint32_t yCluster, const CellValues value);
	const SurfaceGridCell& GetCellCRef(const uint32_t xCluster, const uint32_t yCluster) const;

	void MakeFullyFree(void);

	const std::vector<std::vector<SurfaceGridCell>>& GetGridCRef(void) const;

	void SetLeftEntrance(const uint32_t entranceY);
	void SetRightEntrance(const uint32_t entranceY);
	void SetUpEntrance(const uint32_t entranceX);
	void SetBottomEntrance(const uint32_t entranceX);
private:
	uint32_t clusterIndex;
	const SurfaceGrid& surface;

	std::vector<std::vector<SurfaceGridCell>> surfaceGridPortion;
	std::unordered_set<SurfaceGridClusterEntrance, SurfaceGridClusterEntranceHashFunc> leftEntrances;
	std::unordered_set<SurfaceGridClusterEntrance, SurfaceGridClusterEntranceHashFunc> rightEntrances;
	std::unordered_set<SurfaceGridClusterEntrance, SurfaceGridClusterEntranceHashFunc> upEntrances;
	std::unordered_set<SurfaceGridClusterEntrance, SurfaceGridClusterEntranceHashFunc> bottomEntrances;
};
