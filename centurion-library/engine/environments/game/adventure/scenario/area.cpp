#include "area.h"

#include <header.h>
#include <math_utils.h>
#include <camera.h>
#include <viewport.h>
#include <mouse.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/classes/gobject.h>
#include <dialogWindows.h>

#include <rectangle_shader.h>
#include <circle_shader.h>

std::weak_ptr<Surface> Area::surfaceW;

#pragma region AREA
uint8_t Area::nTabs = 0;
void Area::SetTabs(const uint8_t tabs)
{
	Area::nTabs = tabs;
}

void Area::SetSurface(const std::shared_ptr<Surface>& surface)
{
	Area::surfaceW = surface;
}

std::ostream& operator<<(std::ostream& out, const Area& area) noexcept
{
	const string tabs(Area::nTabs, '\t');
	auto size = glm::ivec2(area.GetSize());
	auto name = area.GetName();
	auto center = glm::ivec2(area.CalculateCenter());

	out << tabs << "<area shape='rectangle' name='" << name << "' x='" << center.x << "' y='" << center.y << "' width='" << size.x << "' height='" << size.y << "'>\n";

	for (auto o : area.objectsInside)
	{
		std::string xOffset = std::to_string((int)o->GetPositionX() - (int)center.x);
		std::string yOffset = std::to_string((int)o->GetPositionY() - (int)center.y);
		out << tabs + "\t<object class='" + o->GetClassName() + "' xOffset='" + xOffset + "' yOffset='" + yOffset + "' />\n";
	}

	out << tabs << "</area>\n";
	return out;
}

CircularArea* Area::AsCircularArea(void)
{
	return (CircularArea*)this;
}
RectangularArea* Area::AsRectangularArea(void)
{
	return (RectangularArea*)this;
}
Area::Area(void)
{
}
Area::~Area(void)
{
}
void Area::Render(void)
{
	this->Draw();
}
glm::vec2 Area::GetCenter(void) const
{
	return this->center;
}
void Area::SetPosition(const glm::ivec2 position)
{
	this->position = position;
	this->center = this->CalculateCenter();
}
void Area::SetPosition(const int x, const int y)
{
	this->position = glm::vec2(x, y);
	this->center = this->CalculateCenter();
}
void Area::SetPosition(const glm::vec2 position)
{
	this->position = position;
	this->center = this->CalculateCenter();
}
void Area::SetBackgroundColor(const glm::vec4 color)
{
	this->backgroundColor = color;
}
void Area::SetBorderColor(const glm::vec4 color)
{
	this->borderColor = color;
}
void Area::SetName(const std::string& name)
{
	this->name = name;
}
std::string Area::GetName(void) const
{
	return this->name;
}
void Area::Draw(const glm::ivec4 color)
{
	assert(false); // virtual method
}

glm::vec2 Area::CalculateCenter(void) const
{
	assert(false); // virtual method
	return glm::vec2();
}

glm::vec2 Area::GetBottomLeftPosition(void) const
{
	assert(false); // virtual method
	return glm::vec2();
}
void Area::ClearObjectsInside(void)
{
	this->objectsInside.clear();
}
std::vector<glm::vec2> Area::GetListOfCoordinates() const
{
	auto blPos = this->GetBottomLeftPosition();
	auto size = this->GetSize();
	float x0 = blPos.x;
	float x1 = blPos.x + size.x;
	float y0 = blPos.y;
	float y1 = blPos.y + size.y;
	return { glm::vec2(x0, y0), glm::vec2(x1, y0), glm::vec2(x1, y1), glm::vec2(x0, y1) };
}
bool Area::IsHovered(void) const
{
	float xMouse = rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	float yMouse = rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
	return this->IsPointInArea(glm::vec2(xMouse, yMouse));
}
void Area::AddObjectInside(const GObject* objInside)
{
	this->objectsInside.push_back(objInside);
}

#pragma endregion

#pragma region RECTANGULAR AREA

RectangularArea::RectangularArea(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin) :
	Area()
{
	this->origin = origin;
	this->position = position;
	this->width = width;
	this->height = height;
	this->center = this->CalculateCenter();
}
RectangularArea::~RectangularArea(void)
{
}
void RectangularArea::EnableGizmo(void)
{
	this->bGizmoActive = true;
}
void RectangularArea::DisableGizmo(void)
{
	this->bGizmoActive = false;
}
RectangularArea RectangularArea::Deserialize(tinyxml2::XMLElement& areaTag)
{
	RectangularArea newArea = RectangularArea();
	newArea.name = tinyxml2::TryParseStrAttribute(&areaTag, "name");
	newArea.position.x = tinyxml2::TryParseIntAttribute(&areaTag, "x");
	newArea.position.y = tinyxml2::TryParseIntAttribute(&areaTag, "y");
	newArea.width = tinyxml2::TryParseIntAttribute(&areaTag, "width");
	newArea.height = tinyxml2::TryParseIntAttribute(&areaTag, "height");
	newArea.origin = AreaOrigin::Center;
	return newArea;
}
bool RectangularArea::IsPointInArea(glm::vec2 pt) const
{
	bool bX = pt.x >= this->center.x - this->width / 2.f && pt.x <= this->center.x + this->width / 2.f;
	bool bY = pt.y >= this->center.y - this->height / 2.f && pt.y <= this->center.y + this->height / 2.f;
	return bX && bY;
}
bool RectangularArea::Intersect(const RectangularArea& other) const
{
	// check sui centri
	if (this->IsPointInArea(other.GetCenter()) == true || other.IsPointInArea(this->GetCenter()) == true)
		return true;

	// intersezioni di vertici
	for (auto const& pt : other.GetListOfCoordinates())
	{
		if (this->IsPointInArea(pt))
			return true;
	}
	for (auto const& pt : this->GetListOfCoordinates())
	{
		if (other.IsPointInArea(pt))
			return true;
	}

	// intersezioni di spigoli
	// case 1
	if (this->GetListOfCoordinates()[0].x <= other.GetListOfCoordinates()[0].x     // check Left-X
		&& this->GetListOfCoordinates()[1].x >= other.GetListOfCoordinates()[1].x  // check Right-X
		&& this->GetListOfCoordinates()[0].y >= other.GetListOfCoordinates()[0].y  // check Bottom-Y
		&& this->GetListOfCoordinates()[2].y <= other.GetListOfCoordinates()[2].y) // check Top-Y
		return true;

	// case 2
	if (this->GetListOfCoordinates()[0].x >= other.GetListOfCoordinates()[0].x     // check Left-X
		&& this->GetListOfCoordinates()[1].x <= other.GetListOfCoordinates()[1].x  // check Right-X
		&& this->GetListOfCoordinates()[0].y <= other.GetListOfCoordinates()[0].y  // check Bottom-Y
		&& this->GetListOfCoordinates()[2].y >= other.GetListOfCoordinates()[2].y) // check Top-Y
		return true;

	return false;
}
bool RectangularArea::Intersect(const glm::ivec2 position, const uint32_t width, const uint32_t height, const AreaOrigin origin) const
{
	return this->Intersect(RectangularArea(position, width, height, origin));
}
void RectangularArea::AddToDrawQueue(void) const
{
	auto& rectShader = rattlesmake::geometry::rectangle_shader::get_instance();
	rectShader.add_to_queue(
		this->center.x, 
		this->center.y,
		this->width, 
		this->height, 
		this->borderColor, 
		this->backgroundColor, 
		(uint8_t)AreaOrigin::Center
	);
}
glm::vec2 RectangularArea::GetSize(void) const
{
	return glm::vec2(this->width, this->height);
}
std::vector<uint32_t> RectangularArea::GetVerticesInside(void) const
{
	auto chunkWidenessPx = (int)Area::surfaceW.lock()->GetTerrainChunkWidenessPx();
	auto triangleSize = (int)Area::surfaceW.lock()->GetGapBetweenVerticesPx();

	glm::ivec2 bottomleftPos = this->GetBottomLeftPosition(); // bottom-left by default
	glm::ivec2 size = this->GetSize();

	auto verticesInside = std::vector<uint32_t>();
	for (int ix = bottomleftPos.x; ix < bottomleftPos.x + size.x; ix += triangleSize)
	{
		for (int iy = bottomleftPos.y; iy < bottomleftPos.y + size.y; iy += triangleSize)
		{
			unsigned int idx = 0;
			auto xVertexCoord = ix + chunkWidenessPx; // shift by 1 chunk because of margin around the map
			auto yVertexCoord = iy + chunkWidenessPx; // shift by 1 chunk because of margin around the map
			if (Area::surfaceW.lock()->TryGetVertexIndexByMapCoordinates(xVertexCoord, yVertexCoord, &idx, false))
			{
				verticesInside.push_back(idx);
			}
		}
	}
	return verticesInside;
}
void RectangularArea::Draw(const glm::ivec4 color)
{
	if (this->bGizmoActive) this->ManageGizmo();

	auto& rectShader = rattlesmake::geometry::rectangle_shader::get_instance();
	rectShader.draw(this->position.x, this->position.y, width, height, color, glm::vec4(0.f), (uint32_t)this->origin);

	// GIZMO rectangles
	if (this->bGizmoActive == true)
	{
		auto gizmoSize = 8.f * rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() / rattlesmake::peripherals::viewport::get_instance().GetHeight();
		rectShader.draw(this->position.x, this->position.y + height, gizmoSize, gizmoSize, color, glm::vec4(0.f), (uint8_t)AreaOrigin::Center); // top-left
		rectShader.draw(this->position.x + width, this->position.y + height, gizmoSize, gizmoSize, color, glm::vec4(0.f), (uint8_t)AreaOrigin::Center); // top-right
		rectShader.draw(this->position.x + width, this->position.y, gizmoSize, gizmoSize, color, glm::vec4(0.f), (uint8_t)AreaOrigin::Center); // bottom-right
		rectShader.draw(this->position.x, this->position.y, gizmoSize, gizmoSize, color, glm::vec4(0.f), (uint8_t)AreaOrigin::Center); // bottom-left
		rectShader.draw(this->position.x + width / 2.f, this->position.y + height / 2.f, gizmoSize, gizmoSize, color, glm::vec4(0.f), (uint8_t)AreaOrigin::Center); // center
	}
}

void RectangularArea::ManageGizmo(void)
{
	auto& mouse = rattlesmake::peripherals::mouse::get_instance();
	auto gizmoSize = 8.f * rattlesmake::peripherals::viewport::get_instance().GetHeightZoomed() / rattlesmake::peripherals::viewport::get_instance().GetHeight();

	if (mouse.Release)
	{
		this->bGizmoEditing = false;
		this->gizmoLeftClick = glm::vec2(0, 0);
		this->gizmoInitialPos = this->position;
		this->gizmoInitialWidth = width;
		this->gizmoInitialHeight = height;
		return;
	}

	if (mouse.LeftClick)
	{
		bool bTopLeftHover = ((mouse.GetXMapCoordinate() >= this->position.x - gizmoSize / 2.f && mouse.GetXMapCoordinate() <= this->position.x + gizmoSize / 2.f) &&
			(mouse.GetYMapCoordinate() >= this->position.y + height - gizmoSize / 2.f && mouse.GetYMapCoordinate() <= this->position.y + height + gizmoSize / 2.f));

		bool bTopRightHover = ((mouse.GetXMapCoordinate() >= this->position.x + width - gizmoSize / 2.f && mouse.GetXMapCoordinate() <= this->position.x + width + gizmoSize / 2.f) &&
			(mouse.GetYMapCoordinate() >= this->position.y + height - gizmoSize / 2.f && mouse.GetYMapCoordinate() <= this->position.y + height + gizmoSize / 2.f));

		bool bBottomRightHover = ((mouse.GetXMapCoordinate() >= this->position.x + width - gizmoSize / 2.f && mouse.GetXMapCoordinate() <= this->position.x + width + gizmoSize / 2.f) &&
			(mouse.GetYMapCoordinate() >= this->position.y - gizmoSize / 2.f && mouse.GetYMapCoordinate() <= this->position.y + gizmoSize / 2.f));

		bool bBottomLeftHover = ((mouse.GetXMapCoordinate() >= this->position.x - gizmoSize / 2.f && mouse.GetXMapCoordinate() <= this->position.x + gizmoSize / 2.f) &&
			(mouse.GetYMapCoordinate() >= this->position.y - gizmoSize / 2.f && mouse.GetYMapCoordinate() <= this->position.y + gizmoSize / 2.f));

		bool bCenterHover = ((mouse.GetXMapCoordinate() >= this->position.x + width / 2.f - gizmoSize / 2.f && mouse.GetXMapCoordinate() <= this->position.x + width / 2.f + gizmoSize / 2.f) &&
			(mouse.GetYMapCoordinate() >= this->position.y + height / 2.f - gizmoSize / 2.f && mouse.GetYMapCoordinate() <= this->position.y + height / 2.f + gizmoSize / 2.f));

		this->bGizmoEditing = true;
		this->gizmoLeftClick = glm::vec2(mouse.GetXMapCoordinate(), mouse.GetYMapCoordinate());
		this->gizmoInitialPos = this->position;
		this->gizmoInitialWidth = width;
		this->gizmoInitialHeight = height;

		if (bTopLeftHover)
		{
			this->gizmoOrigin = GizmoOrigin::TopLeft;
		}
		else if (bTopRightHover)
		{
			this->gizmoOrigin = GizmoOrigin::TopRight;
		}
		else if (bBottomRightHover)
		{
			this->gizmoOrigin = GizmoOrigin::BottomRight;
		}
		else if (bBottomLeftHover)
		{
			this->gizmoOrigin = GizmoOrigin::BottomLeft;
		}
		else if (bCenterHover)
		{
			this->gizmoOrigin = GizmoOrigin::Center;
		}
		else
		{
			this->bGizmoEditing = false;
			this->gizmoLeftClick = glm::vec2(0, 0);
		}
	}

	if ((mouse.LeftClick || mouse.LeftHold) && this->bGizmoEditing)
	{
		glm::vec2 delta = glm::vec2(mouse.GetXMapCoordinate(), mouse.GetYMapCoordinate()) - this->gizmoLeftClick;

		float minimumSize = 4.f;

		// x-axis
		switch (this->gizmoOrigin)
		{
		case GizmoOrigin::TopLeft:
			if (this->gizmoInitialWidth - delta.x > minimumSize)
			{
				this->position.x = this->gizmoInitialPos.x + delta.x;
				this->width = this->gizmoInitialWidth - delta.x;
			}
			break;
		case GizmoOrigin::BottomRight:
			if (this->gizmoInitialWidth + delta.x > minimumSize)
			{
				this->width = this->gizmoInitialWidth + delta.x;
			}
			break;
		case GizmoOrigin::BottomLeft:
			if (this->gizmoInitialWidth - delta.x > minimumSize)
			{
				this->position.x = this->gizmoInitialPos.x + delta.x;
				this->width = this->gizmoInitialWidth - delta.x;
			}
			break;
		case GizmoOrigin::TopRight:
			if (this->gizmoInitialWidth + delta.x > minimumSize)
			{
				this->width = this->gizmoInitialWidth + delta.x;
			}
			break;
		case GizmoOrigin::Center:
			this->position.x = this->gizmoInitialPos.x + delta.x;
			break;
		default:
			break;
		}

		// y-axis
		switch (this->gizmoOrigin)
		{
		case GizmoOrigin::TopLeft:
			if (this->gizmoInitialHeight + delta.y > minimumSize)
			{
				this->height = this->gizmoInitialHeight + delta.y;
			}
			break;
		case GizmoOrigin::BottomRight:
			if (this->gizmoInitialHeight - delta.y > minimumSize)
			{
				this->position.y = this->gizmoInitialPos.y + delta.y;
				this->height = this->gizmoInitialHeight - delta.y;
			}
			break;
		case GizmoOrigin::BottomLeft:
			if (this->gizmoInitialHeight - delta.y > minimumSize)
			{
				this->position.y = this->gizmoInitialPos.y + delta.y;
				this->height = this->gizmoInitialHeight - delta.y;
			}
			break;
		case GizmoOrigin::TopRight:
			if (this->gizmoInitialHeight + delta.y > minimumSize)
			{
				this->height = this->gizmoInitialHeight + delta.y;
			}
			break;
		case GizmoOrigin::Center:
			this->position.y = this->gizmoInitialPos.y + delta.y;
			break;
		default:
			break;
		}
	}
}

glm::vec2 RectangularArea::CalculateCenter(void) const
{
	switch (this->origin)
	{
	case AreaOrigin::BottomLeft:
		return glm::vec2(this->position.x + this->width / 2.f, this->position.y + this->height / 2.f);
	case AreaOrigin::TopLeft:
		return glm::vec2(this->position.x + this->width / 2.f, this->position.y - this->height / 2.f);
	case AreaOrigin::Center:
		return this->position;
	case AreaOrigin::TopRight:
		return glm::vec2(this->position.x - this->width / 2.f, this->position.y - this->height / 2.f);
	case AreaOrigin::BottomRight:
		return glm::vec2(this->position.x - this->width / 2.f, this->position.y + this->height / 2.f);
	default:
		return glm::vec2(0);
	}
}

glm::vec2 RectangularArea::GetBottomLeftPosition(void) const
{
	switch (this->origin)
	{
	case AreaOrigin::BottomLeft:
		return this->position;
	case AreaOrigin::TopLeft:
		return glm::vec2(this->position.x, this->position.y - this->height);
	case AreaOrigin::Center:
		return glm::vec2(this->position.x - this->width / 2, this->position.y - this->height / 2);
	case AreaOrigin::TopRight:
		return glm::vec2(this->position.x - this->width, this->position.y - this->height);
	case AreaOrigin::BottomRight:
		return glm::vec2(this->position.x - this->width, this->position.y);
	default:
		return glm::vec2(0);
	}
}

#pragma endregion

#pragma region CIRCULAR AREA

CircularArea::CircularArea(const glm::ivec2 position, const uint32_t radius, const AreaOrigin origin) :
	Area()
{
	this->origin = origin;
	this->position = position;
	this->radius = radius;
	this->center = this->CalculateCenter();
}
CircularArea::~CircularArea(void)
{
}
bool CircularArea::IsPointInArea(glm::vec2 pt) const
{
	return (Math::euclidean_distance(pt.x, pt.y, this->center.x, this->center.y) <= this->radius);
}
bool CircularArea::Intersect(const RectangularArea& other) const
{
	for (auto const& pt : other.GetListOfCoordinates())
	{
		if (this->IsPointInArea(pt)) return true;
	}
	return this->IsPointInArea(other.GetCenter());
}
std::vector<uint32_t> CircularArea::GetVerticesInside(void) const
{
	auto chunkWidenessPx = (int)Area::surfaceW.lock()->GetTerrainChunkWidenessPx();
	auto triangleSize = (int)Area::surfaceW.lock()->GetGapBetweenVerticesPx();

	float startX = (float)(this->center.x - this->radius);
	float startY = (float)(this->center.y - this->radius);
	startX = float(int(startX / (float)triangleSize) * triangleSize);
	startY = float(int(startY / (float)triangleSize) * triangleSize);

	auto verticesInside = std::vector<uint32_t>();
	for (int ix = (int)startX; ix < this->center.x + (int)this->radius; ix += triangleSize)
	{
		for (int iy = (int)startY; iy < this->center.y + (int)this->radius; iy += triangleSize)
		{
			if (this->IsPointInArea(glm::vec2(ix, iy)) == false) continue;
			unsigned int idx = 0;
			if (Area::surfaceW.lock()->TryGetVertexIndexByMapCoordinates(ix + chunkWidenessPx, iy + chunkWidenessPx, &idx, false))
			{
				verticesInside.push_back(idx);
			}
		}
	}
	return verticesInside;
}
glm::vec2 CircularArea::GetSize(void) const
{
	return glm::vec2(this->radius * 2.f, this->radius * 2.f);
}
void CircularArea::SetRadius(const uint32_t radius)
{
	this->radius = radius;
	this->center = this->CalculateCenter();
}
void CircularArea::SetRadius(const float radius)
{
	this->radius = (uint32_t)radius;
	this->center = this->CalculateCenter();
}
uint32_t CircularArea::GetRadius(void)
{
	return this->radius;
}
void CircularArea::Draw(const glm::ivec4 color)
{
	auto& circleShader = rattlesmake::geometry::circle_shader::get_instance();
	circleShader.draw(color, this->position.x, this->position.y, this->radius, (uint32_t)this->origin);
}

glm::vec2 CircularArea::CalculateCenter(void) const
{
	switch (this->origin)
	{
	case AreaOrigin::BottomLeft:
		return glm::vec2(this->position.x + this->radius, this->position.y + this->radius);
	case AreaOrigin::TopLeft:
		return glm::vec2(this->position.x + this->radius, this->position.y - this->radius);
	case AreaOrigin::Center:
		return this->position;
	case AreaOrigin::TopRight:
		return glm::vec2(this->position.x - this->radius, this->position.y - this->radius);
	case AreaOrigin::BottomRight:
		return glm::vec2(this->position.x - this->radius, this->position.y + this->radius);
	default:
		return glm::vec2(0);
	}
}

glm::vec2 CircularArea::GetBottomLeftPosition(void) const
{
	switch (this->origin)
	{
	case AreaOrigin::BottomLeft:
		return this->position;
	case AreaOrigin::TopLeft:
		return glm::vec2(this->position.x, this->position.y - this->radius * 2.f);
	case AreaOrigin::Center:
		return glm::vec2(this->position.x - this->radius, this->position.y - this->radius);;
	case AreaOrigin::TopRight:
		return glm::vec2(this->position.x - this->radius * 2.f, this->position.y - this->radius * 2.f);
	case AreaOrigin::BottomRight:
		return glm::vec2(this->position.x - this->radius * 2.f, this->position.y);
	default:
		return glm::vec2(0);
	}
}

#pragma endregion

#pragma region DRAWING AREA

DrawingArea::DrawingArea(void)
{
}
DrawingArea::~DrawingArea(void)
{
}
bool DrawingArea::Render(void)
{
	{
		//Deny rendering if some iframe or dialog window is open.
		//N.B.: This code was put here in order to disable rectangle area rendering.
		//So, if this code should generate problems with other types of areas, please do not simply delete it but move it in another proper place :)
		auto ui = Engine::GetInstance().GetEnvironment()->AsIGame()->GetUIRef();
		if (ui->AnyIframeIsOpened() == true || gui::IsAnyDialogWindowActive() == true)
			return false;
	}
	// this method is useful for selection area

	auto& mouse = rattlesmake::peripherals::mouse::get_instance();

	this->width = 0;
	this->height = 0;

	if (mouse.Release)
	{
		this->ResetStartLastValues();
		return false;
	}

	if (mouse.LeftClick)
	{
		this->ResetStartLastValues();
		this->SetStartValues();
		return false;
	}

	if (mouse.LeftHold)
	{
		this->SetLastValues();
		this->CalculateSizeAndPosition();
	}
	if (this->width <= 2 || this->height <= 2)
		return false;

	this->Draw(); // uses the rectangular area draw method
	return true;
}

std::pair<bool, RectangularArea> DrawingArea::DrawWithMouse(void)
{
	// logic very similar to render, but more useful for area drawing
	// returns true when mouse is released

	auto& mouse = rattlesmake::peripherals::mouse::get_instance();

	if (mouse.Release)
	{
		if (this->width > 1 && this->height > 1)
		{
			auto output = std::pair<bool, RectangularArea>(true, *this->AsRectangularArea());
			this->ResetStartLastValues();
			return output;
		}
		else
		{
			return std::pair<bool, RectangularArea>(false, *this->AsRectangularArea());
		}
	}

	this->width = 0;
	this->height = 0;

	if (mouse.LeftClick)
	{
		this->ResetStartLastValues();
		this->SetStartValues();
		return std::pair<bool, RectangularArea>(false, *this->AsRectangularArea());
	}

	if (mouse.LeftHold)
	{
		this->SetLastValues();
		this->CalculateSizeAndPosition();
	}

	if (this->width <= 2 || this->height <= 2)
		return std::pair<bool, RectangularArea>(false, *this->AsRectangularArea());

	this->Draw(); // uses the rectangular area draw method
	return std::pair<bool, RectangularArea>(false, *this->AsRectangularArea());
}

void DrawingArea::CalculateSizeAndPosition()
{
	this->width = (uint32_t)std::abs(this->lastX - this->startX);
	this->height = (uint32_t)std::abs(this->lastY - this->startY);
	this->SetPosition(glm::ivec2(std::min(this->startX, this->lastX), std::min(this->startY, this->lastY)));
}
void DrawingArea::ResetStartLastValues()
{
	this->startX = (int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	this->startY = (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
	this->lastX = (int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	this->lastY = (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
}
void DrawingArea::SetStartValues()
{
	this->startX = (int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	this->startY = (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
}
void DrawingArea::SetLastValues()
{
	this->lastX = (int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	this->lastY = (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
}

#pragma endregion

#pragma region SELECTION AREA

SelectionArea::SelectionArea(void)
{
}
void SelectionArea::ResetDrawingPoints(void)
{
	this->area.ResetStartLastValues();
}
SelectionArea::~SelectionArea(void)
{
}
bool SelectionArea::IsActive(void)
{
	return this->bIsActive;
}
bool SelectionArea::IsPointInArea(glm::vec2 pt) const
{
	return this->bIsActive && area.IsPointInArea(pt);
}
void SelectionArea::Render(void)
{
	this->bIsActive = area.Render(); // perform render of area
}
bool SelectionArea::Intersect(RectangularArea& other_area)
{
	return this->area.Intersect(other_area);
}
#pragma endregion
