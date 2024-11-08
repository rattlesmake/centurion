/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <environments/game/adventure/scenario/area.h>

#ifndef CHANGE_TERRAIN_DISABLED
#define CHANGE_TERRAIN_DISABLED 0
#endif

#ifndef CHANGE_TERRAIN_HEIGHT
#define CHANGE_TERRAIN_HEIGHT 1
#endif

#ifndef CHANGE_TERRAIN_TEXTURE
#define CHANGE_TERRAIN_TEXTURE 2
#endif

#ifndef CHANGE_TERRAIN_SEA
#define CHANGE_TERRAIN_SEA 3
#endif

#ifndef MIN_BRUSH_DIAMETER
#define MIN_BRUSH_DIAMETER 1
#endif

#ifndef DEFAULT_BRUSH_DIAMETER
#define DEFAULT_BRUSH_DIAMETER 5
#endif

#ifndef MAX_BRUSH_DIAMETER
#define MAX_BRUSH_DIAMETER 10
#endif

class Terrain;

class TerrainBrush
{
public:
	void Initialize(std::string type, std::string textureName, float heightValue);
	void Enable() { this->isEnabled = true; }
	void Disable();
	bool IsEnabled() const { return this->isEnabled; }
	void Create();
	void Render();
	void UpdateDiameter(const uint16_t currentDiameter);
	void Apply();
	bool IsCreated() { return this->isCreated; }
	~TerrainBrush() {}
	TerrainBrush();

private:
	class Data
	{
		friend class TerrainBrush;
	public:

		bool GetPassable() { return this->passable; }
		float GetValue() { return this->value; }
		uint8_t GetType() { return this->type; }
	private:
		Data() { }
		Data(TerrainBrush* myCreator, std::string type, std::string textureName, float heightValue);
		bool passable = true;
		float value = 0.f;
		uint8_t type = CHANGE_TERRAIN_DISABLED;
	} data;

	void ApplyTexture(const float value, const bool passable);
	void ApplyHeight(const float value);
	void ApplySea(const bool deepSea);
	void UpdateDiameterScroll(void);
	CircularArea brushAreaToDraw;
	CircularArea brushAreaForCalculations;
	bool isCreated = false;
	bool isEnabled = false;
	uint16_t currentDiameter = 1;

	std::weak_ptr<Terrain> terrain;
};
