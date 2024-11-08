/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>
#include <functional> // std::hash

class SurfaceGridClusterEntrance
{
public:
    SurfaceGridClusterEntrance(const uint32_t _y, const uint32_t _x);
    SurfaceGridClusterEntrance(SurfaceGridClusterEntrance&& other) noexcept;
    SurfaceGridClusterEntrance& operator=(SurfaceGridClusterEntrance&& other) noexcept;
    [[nodiscard]] bool operator==(const SurfaceGridClusterEntrance& other) const noexcept;
    ~SurfaceGridClusterEntrance(void);

    [[nodiscard]] uint32_t GetY(void) const;
    [[nodiscard]] uint32_t GetX(void) const;
private:
    uint32_t y;
    uint32_t x;
};

struct SurfaceGridClusterEntranceHashFunc
{
public:
    size_t operator()(const SurfaceGridClusterEntrance& sgce) const;
};
