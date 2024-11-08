#include "area_array.h"
#include "scenario.h"
#include <engine.h>
#include <environments/game/igame.h>
#include <environments/game/igameUi.h>
#include <environments/game/editor/editor.h>
#include <ui.h>
#include <mouse.h>
#include <viewport.h>
#include <camera.h>

uint8_t AreaArray::nTabs = 0;

AreaArray::AreaArray()
{
}
AreaArray::~AreaArray(void)
{
}

void AreaArray::SetTabs(const uint8_t tabs)
{
	AreaArray::nTabs = tabs;
}

void AreaArray::Deserialize(tinyxml2::XMLElement& areasTag)
{
	for (tinyxml2::XMLElement* _area = areasTag.FirstChildElement("area"); _area != NULL; _area = _area->NextSiblingElement())
	{
		auto shape = tinyxml2::TryParseStrAttribute(_area, "shape");
		if (shape == "rectangle")
		{
			auto newArea = RectangularArea::Deserialize(*_area);
			this->AddArea(newArea.GetName(), newArea.GetBottomLeftPosition(), newArea.GetSize());
		}
		else if (shape == "circle")
		{
			//TODO
		}
	}
}

std::ostream& operator<<(std::ostream& out, const AreaArray& areaArray) noexcept
{
	const string nTabs(AreaArray::nTabs, '\t');
	out << nTabs << "<areas>\n" << nTabs << "\t<!-- x and y values always refer to center origin -->\n";
	Area::SetTabs(AreaArray::nTabs + 1);
	for (auto const& area : areaArray.areasList)
	{
		if (area.expired()) 
			continue;

		if (area.lock()->GetName().empty() == false)
			out << (*(area.lock()));
		
		area.lock()->ClearObjectsInside(); // objects inside are set by GObject serialize function
	}
	out << nTabs << "</areas>";
	return out;
}

void AreaArray::Render(void)
{
	auto ui = Engine::GetInstance().GetEnvironment()->AsIGame()->GetUIRef();
	assert(ui);

	// disable drawing area
	if (bDrawingArea == true && rattlesmake::peripherals::mouse::get_instance().RightClick)
	{
		bDrawingArea = false;
		ui->ShowIframes();
		ui->ShowMenubar();
	}

	// disable gizmo
	if (bGizmoActive == true && rattlesmake::peripherals::mouse::get_instance().RightClick)
	{
		bGizmoActive = false;
		this->areasMap[this->gizmoAreaName]->AsRectangularArea()->DisableGizmo();
		this->gizmoAreaName = "";
		ui->ShowIframes();
		ui->ShowMenubar();
	}

	// render drawing area
	if (bDrawingArea == true)
	{
		ui->HideIframes();
		ui->HideMenubar();

		auto draw_result = drawingArea.DrawWithMouse();
		if (draw_result.first == true)
		{
			auto& area_drawn = draw_result.second;
			this->AddArea(newAreaName, area_drawn.GetBottomLeftPosition(), area_drawn.GetSize());
			Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentAdventure()->MarkAsEdited();
		}
	}

	// render gizmo area
	if (bGizmoActive)
	{
		ui->HideIframes();
		ui->HideMenubar();

		this->areasMap[this->gizmoAreaName]->Render();
	}

	// render areas
	if (bRenderingAreas)
	{
		auto area_iterator = this->areasList.begin();
		while (area_iterator != this->areasList.end())
		{
			auto area = (*area_iterator).lock();
			if (area)
			{
				if (area->GetName() != this->gizmoAreaName)
					area->Render();
				
				area_iterator++;
			}
			else
			{
				area_iterator = this->areasList.erase(area_iterator);
			}
		}
	}
}

void AreaArray::RemoveArea(const std::string& areaName)
{
	if (this->areasMap.contains(areaName))
	{
		this->areasMap.erase(areaName);
	}
}

void AreaArray::AddArea(const std::string& areaName, const glm::ivec2 position, const glm::ivec2 size)
{
	if (areaName.empty()) return;
	if (size.x <= 1 || size.y <= 1) return;

	this->areasMap[areaName] = std::shared_ptr<Area>(new RectangularArea(position, size.x, size.y, AreaOrigin::BottomLeft));
	this->areasMap[areaName]->SetName(areaName);
	this->areasList.push_back(this->areasMap[areaName]);
}

void AreaArray::EnableAreaDrawing(const std::string& areaName)
{
	this->newAreaName = areaName;
	this->bDrawingArea = areaName.empty() == false;
}
void AreaArray::EnableAreaGizmo(const std::string& areaName)
{
	this->gizmoAreaName = areaName;
	this->bGizmoActive = areaName.empty() == false;
	if (this->bGizmoActive)
		this->areasMap[this->gizmoAreaName]->AsRectangularArea()->EnableGizmo();
}
void AreaArray::EnableAreasRendering(void)
{
	this->bRenderingAreas = true;
}
void AreaArray::DisableAreaDrawing(void)
{
	this->bDrawingArea = false;
}
void AreaArray::DisableAreasRendering(void)
{
	this->bRenderingAreas = false;
}
bool AreaArray::IsDrawingArea(void)
{
	return this->bDrawingArea;
}
bool AreaArray::IsRenderingAreas(void)
{
	return this->bRenderingAreas;
}

bool AreaArray::IsGizmoActive(void)
{
	return this->bGizmoActive;
}

std::vector<std::string> AreaArray::GetNames(void)
{
	std::vector<std::string> names;

	auto area_iterator = this->areasList.begin();
	while (area_iterator != this->areasList.end())
	{
		auto area = (*area_iterator).lock();
		if (area)
		{
			names.push_back(area->GetName());
		}
		area_iterator++;
	}
	return names;
}

std::list<std::weak_ptr<Area>>* AreaArray::GetAreasListPtr(void)
{
	return &(this->areasList);
}
