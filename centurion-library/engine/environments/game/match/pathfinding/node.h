/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>

class SurfaceGrid;

class Node
{
    friend class Pathfinder;
public:
    Node(const uint32_t _xGrid, const uint32_t _yGrid);

    [[nodiscard]] bool operator<(const Node& other) const noexcept;
    [[nodiscard]] bool operator==(const Node& other) const noexcept;

    [[nodiscard]] static bool IsValid(const uint32_t xGrid, const uint32_t yGrid, SurfaceGrid& surfaceGridRef);
private:
    uint32_t yGrid = 0;  //Row
    uint32_t xGrid = 0;  //Column
    uint32_t parentX = 0;
    uint32_t parentY = 0;
    double gCost = 0;
    double hCost = 0;
    double fCost = 0;  //An admissible cost estimate for all points in the set of states that forms an interval
};
