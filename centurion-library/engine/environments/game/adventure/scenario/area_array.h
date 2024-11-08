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

class Scenario;

class AreaArray
{
	friend class Scenario;

public:
	friend std::ostream& operator<<(std::ostream& out, const AreaArray& areaArray) noexcept;
	~AreaArray(void);

	static void SetTabs(const uint8_t tabs);

	void Deserialize(tinyxml2::XMLElement& areasTag);

	void Render(void);
	void RemoveArea(const std::string& areaName);

	// always BOTTOM-LEFT origin!!
	void AddArea(const std::string& areaName, const glm::ivec2 position, const glm::ivec2 size);
	
	void EnableAreaDrawing(const std::string& areaName);
	void EnableAreaGizmo(const std::string& areaName);
	void EnableAreasRendering(void);
	void DisableAreaDrawing(void);
	void DisableAreasRendering(void);
	[[nodiscard]] bool IsDrawingArea(void);
	[[nodiscard]] bool IsRenderingAreas(void);
	[[nodiscard]] bool IsGizmoActive(void);
	[[nodiscard]] std::vector<std::string> GetNames(void);

	std::list<std::weak_ptr<Area>>* GetAreasListPtr(void);

private:
	AreaArray(); // private. we can instanciate it only in Scenario class 

	std::unordered_map<std::string, std::shared_ptr<Area>> areasMap;
	std::list<std::weak_ptr<Area>> areasList;

	DrawingArea drawingArea;
	std::string newAreaName;
	std::string gizmoAreaName;
	bool bDrawingArea = false;
	bool bRenderingAreas = false;
	bool bGizmoActive = false;

	static uint8_t nTabs;
};
