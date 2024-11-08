/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>

enum class AreaOrigin
{
	// IMPORTANT
	// the number are fixed
	// because they are also used in shaders
	BottomLeft = 0,
	TopLeft = 1,
	Center = 2,
	TopRight = 3,
	BottomRight = 4
};

class CircularArea;
class Scenario;
class RectangularArea;
class SelectionArea;
class GObject;
class Surface;

#pragma region FIXED AREAS (THEY ARE NOT GOING TO BE DRAWN)
// these areas need to be instanced with given position and size

// if the goal is ONLY to draw rectangle/circles
// i suggest to use them to instance and render circles/rectangles on the fly
// and leave them to be deleted automatically after rendering

// if the goal is to store areas and their information, no problem,
// create a vector/list/map with rectangle/circle areas

// TODO: change position/size with Setters.

class Area
{
	friend class GObject;

public:
	friend std::ostream& operator<<(std::ostream& out, const Area& area) noexcept;

	static void SetTabs(const uint8_t tabs);	
	static void SetSurface(const std::shared_ptr<Surface>& surface);

	CircularArea* AsCircularArea(void);
	RectangularArea* AsRectangularArea(void);
	~Area(void);
	Area(void);
	void Render(void);
	virtual bool IsPointInArea(glm::vec2 pt) const = 0;
	virtual bool Intersect(const RectangularArea& other) const = 0;
	virtual std::vector<uint32_t> GetVerticesInside(void) const = 0;
	virtual glm::vec2 GetSize(void) const = 0;
	glm::vec2 GetCenter(void) const;
	void SetPosition(const glm::ivec2 position);
	void SetPosition(const int x, const int y);
	void SetPosition(const glm::vec2 position);
	void SetBackgroundColor(const glm::vec4 color);
	void SetBorderColor(const glm::vec4 color);
	void SetName(const std::string& name);
	std::string GetName(void) const;
	virtual glm::vec2 GetBottomLeftPosition(void) const;
	void ClearObjectsInside(void);
	std::vector<glm::vec2> GetListOfCoordinates() const;
	bool IsHovered(void) const;


protected:
	virtual void Draw(const glm::ivec4 color = glm::ivec4(255, 255, 255, 255));
	virtual glm::vec2 CalculateCenter(void) const;

	AreaOrigin origin = AreaOrigin::BottomLeft;
	glm::vec2 center = glm::vec2(0);
	glm::vec2 position = glm::vec2(0);
	std::string name;
	glm::vec4 backgroundColor = glm::vec4(0);
	glm::vec4 borderColor = glm::vec4(255);

	static std::weak_ptr<Surface> surfaceW;
	static uint8_t nTabs;
private:
	void AddObjectInside(const GObject* objInside); // only GObject can use it
	std::vector<const GObject*> objectsInside;
};

class RectangularArea : public Area
{
public:
	static RectangularArea Deserialize(tinyxml2::XMLElement& areaTag);

	RectangularArea(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin);
	~RectangularArea(void);
	void EnableGizmo(void);
	void DisableGizmo(void);
	bool IsPointInArea(glm::vec2 pt) const override;
	bool Intersect(const RectangularArea& other) const override;
	bool Intersect(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin) const;
	void AddToDrawQueue(void) const;
	glm::vec2 GetSize(void) const override;
	std::vector<uint32_t> GetVerticesInside(void) const override;
	glm::vec2 GetBottomLeftPosition(void) const override;
	RectangularArea(void) {}
protected:
	void Draw(const glm::ivec4 color = glm::ivec4(255, 255, 255, 255)) override;
	void ManageGizmo(void);
	glm::vec2 CalculateCenter(void) const override;

	uint32_t width = 0;
	uint32_t height = 0;
	bool bGizmoActive = false;
	bool bGizmoEditing = false;
	glm::vec2 gizmoLeftClick;
	glm::vec2 gizmoInitialPos;
	float gizmoInitialWidth = 0.f, gizmoInitialHeight = 0.f;

	enum class GizmoOrigin
	{
		BottomLeft = 0,
		TopLeft = 1,
		TopRight = 2,
		BottomRight = 3,
		Center = 4
	} gizmoOrigin;
};

class CircularArea : public Area
{
public:
	CircularArea(const glm::ivec2 position, const uint32_t radius, const AreaOrigin origin);
	~CircularArea(void);
	bool IsPointInArea(glm::vec2 pt) const override;
	bool Intersect(const RectangularArea& other) const override;
	glm::vec2 GetSize(void) const override;
	std::vector<uint32_t> GetVerticesInside(void) const override;
	void SetRadius(const uint32_t radius);
	void SetRadius(const float radius);
	uint32_t GetRadius(void);
	glm::vec2 GetBottomLeftPosition(void) const override;
protected:
	void Draw(const glm::ivec4 color = glm::ivec4(255, 255, 255, 255)) override;
	glm::vec2 CalculateCenter(void) const override;

private:
	uint32_t radius = 0;
};

#pragma endregion


#pragma region DRAWING AREA (IT IS BEING DRAWN AND FOLLOWS MOUSE CLICKING/HOLDING/POSITION)
// this class is useful to render an area that is being drawn by the user
// when drawing process is finished, i suggest to take size and position 
// and replace it with a fixed rectangle area

// TODO: at the moment only rectangle areas are supported
// TODO: it could be singleton?

class DrawingArea : public RectangularArea
{
	friend class SelectionArea;
public:
	DrawingArea(void);
	~DrawingArea(void);
	bool Render(void); // returns false if area is too small (1px wide or high) or not rendered (USEFUL FOR SELECTION AREA)
	std::pair<bool, RectangularArea> DrawWithMouse(void); // returns true when mouse is released and area isn't too small
private:
	// drawing members
	void ResetStartLastValues();
	void SetStartValues();
	void SetLastValues();
	void CalculateSizeAndPosition();
	int startX = 0, startY = 0, lastX = 0, lastY = 0;
};

#pragma endregion


#pragma region SELECTION AREA (BASED ON DRAWING AREA)
// wrapper class to facilitate the use of area
// it could contain some selection area logic

// TODO: at the moment only rectangle areas are supported
// TODO: it could be singleton?

class SelectionArea
{
	friend class Scenario;
public:
	~SelectionArea(void);
	void Render(void);
	bool IsActive(void);
	bool IsPointInArea(glm::vec2 pt) const;
	void ResetDrawingPoints(void);
	bool Intersect(RectangularArea& other_area);
protected:
	SelectionArea(void); // only instanciable in Scenario class
	bool bIsActive = false;
	DrawingArea area = DrawingArea();
};

#pragma endregion
