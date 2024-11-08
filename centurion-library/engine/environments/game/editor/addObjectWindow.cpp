#include "addObjectWindow.h"
#include "editor.h"

#include <mouse.h>
#include <keyboard.h>
#include <viewport.h>
#include <environments/game/adventure/adventure.h>
#include <imgui.h>
#include <encode_utils.h>
#include <stl_utils.h>
#include <services/sqlservice.h>
#include <icons.h>
#include <environments/game/classes/objectsSet/objects_collection.h>
#include <environments/game/classes/objectsSet/objects_list.h>

void AddObjectWindow::Initialize(const std::vector<std::pair<std::string, std::string>>& source)
{
	for (auto const& el : source)
	{
		ObjSum os;
		os.className = el.second;
		os.selected = false;

		this->values.push_back(os);
	}
}

void AddObjectWindow::Render()
{
	this->CheckIfOpen();

	if (this->bIsOpened && Engine::GetInstance().GetEnvironment()->AsEditor()->IsInsertingObject() == false)
	{
		// size depends on filter (if empty or not)
		//filter.empty() ? ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY_nofilter)) : ImGui::SetNextWindowSize(ImVec2(sizeX, sizeY_filter));
		ImGui::SetNextWindowPos(ImVec2(posX, posY));
		ImGui::SetNextWindowSize(ImVec2(0, 0));
		ImGui::Begin("##AddObjectWindow__");
		ImGui::Text((SqlService::GetInstance().GetTranslation("e_text_add_object", false) + ":").c_str());
		ImGui::Separator();

		// input text
		{
			ImGui::PushItemWidth(sizeX);
			if (this->bIsOpening == true)
			{
				ImGui::SetKeyboardFocusHere(0);
				this->bIsOpening = false;
			}
			ImGui::InputText("##AddObjectWindowText__", input_buf, IM_ARRAYSIZE(input_buf), ImGuiInputTextFlags_AnyInputReturnsTrue);
			if(ImGui::IsItemActive() == false)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(15);
				ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
				ImGui::Text((const char*)GetIconUTF8Reference("ICON_SEARCH"));
				ImGui::PopFont();
				ImGui::SameLine();
				ImGui::Text(SqlService::GetInstance().GetTranslation("e_text_search", false).c_str());
			}
			ImGui::PopItemWidth();
			filter = input_buf;
			Encode::ToLowerCase(filter);
			std::trim(filter);
		}

		// text list
		{
			ImVec2 listBoxSize = ImVec2(sizeX, sizeY_listBox);
			if (filter.empty() == false && ImGui::ListBoxHeader("##AddObjectWindowTextList__", listBoxSize))
			{
				for (size_t i = 0; i < this->values.size(); i++)
				{
					std::string current_value_class_name = this->values[i].className;
					bool current_value_bool = this->values[i].selected;

					Encode::ToLowerCase(current_value_class_name);
					std::trim(current_value_class_name);

					if (current_value_class_name.find(filter) == std::string::npos)
						continue;

					if (ImGui::Selectable(this->values[i].className.c_str(), this->values[i].selected))
					{
						for (size_t ii = 0; ii < this->values.size(); ii++) { this->values[ii].selected = (i == ii); }

						Engine::GetInstance().GetEnvironment()->AsEditor()->InsertObject(this->values[i].className, 1);
					}
				}
				ImGui::ListBoxFooter();
			}
		}

		ImGui::End();
	}
}

void AddObjectWindow::CheckIfOpen(void)
{
#pragma region Variables
	auto const& mouse = rattlesmake::peripherals::mouse::get_instance();
	auto & keyboard = rattlesmake::peripherals::keyboard::get_instance();
	auto & viewport = rattlesmake::peripherals::viewport::get_instance();

	auto const& vpWidth = viewport.GetWidth();
	auto const& vpHeight = viewport.GetHeight();

	auto adventure = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentAdventure();
#pragma endregion

	// if the adventure is not opened return
	if (adventure->IsOpened() == false)
	{
		return;
	}
	auto selectedObjs = Engine::GetInstance().GetEnvironment()->AsIGame()->Selos();

	if (mouse.RightClick && (selectedObjs == nullptr || selectedObjs->Count() == 0))
	{
		this->posX = mouse.GetXRightClick();
		this->posY = vpHeight - mouse.GetYRightClick();

		// fix window position
		
		// too near to right
		if (this->posX + this->sizeX >= vpWidth)
		{
			this->posX = this->posX - this->sizeX;
		}
		// too near to bottom
		//if (this->posY + 100.f >= vpHeight)
		//{
		//	this->posY = this->posY - 100.f;
		//}

		this->bIsOpened = true;
		this->bIsOpening = true;
	}

	if (mouse.LeftClick || keyboard.IsKeyPressed(256)) // 256 = GLFW_KEY_ESCAPE
	{
		this->bIsOpened = false;
		this->bIsOpening = false;
	}
}

bool AddObjectWindow::IsOpened(void)
{
	return this->bIsOpened;
}
