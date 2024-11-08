#include "menubar.h"
#include "ui.h"

#include <services/pyservice.h>
#include <services/sqlservice.h>
#include <icons.h>
#include <imgui_internal.h>


#pragma region Constructors and destructor:
gui::Menubar::Menubar(void)
{
}

gui::Menubar::Menubar(tinyxml2::XMLElement* el)
{
	if (el->Attribute("name") != nullptr)
	{
		std::string name = tinyxml2::TryParseStrAttribute(el, "name");
		this->type = "menu";
		this->text = SqlService::GetInstance().GetTranslation(name, false);
		this->condition = tinyxml2::TryParseStrAttribute(el, "condition");
		if (this->condition.empty() == false) 
			this->condition = this->condition + PyService::GetInstance().GetArgs(IEnvironment::Environments::e_editor);
	}
	else
	{
		this->type = "main-level";
	}

	for (tinyxml2::XMLElement* _item = el->FirstChildElement("menuItemArray")->FirstChildElement(); _item != nullptr; _item = _item->NextSiblingElement())
	{
		string type = _item->Attribute("type");
		if (type == "command")
		{
			Menubar option;
			option.type = std::move(type);
			std::string name = tinyxml2::TryParseStrAttribute(_item, "name");
			option.text = SqlService::GetInstance().GetTranslation(name, false);
			option.icon = tinyxml2::TryParseStrAttribute(_item, "icon");
			option.onclickOpen = tinyxml2::TryParseStrAttribute(_item, "onclick_open");
			option.onclick = tinyxml2::TryParseStrAttribute(_item, "onclick");
			if (option.onclick.empty() == false)
				option.onclick = option.onclick + PyService::GetInstance().GetArgs(IEnvironment::Environments::e_editor);
			option.condition = tinyxml2::TryParseStrAttribute(_item, "condition");
			if (option.condition.empty() == false) 
				option.condition = option.condition + PyService::GetInstance().GetArgs(IEnvironment::Environments::e_editor);

			option.shortcut = tinyxml2::TryParseStrAttribute(_item, "shortcut");

			this->options.push_back(std::move(option));
		}
		else if (type == "menu")
		{
			Menubar _menu2{ _item };
			_menu2.icon = tinyxml2::TryParseStrAttribute(_item, "icon");
			this->options.push_back(std::move(_menu2));
		}
	}
}

gui::Menubar::Menubar(Menubar&& other) noexcept :
	isBlocked(other.isBlocked), isHidden(other.isHidden), options(std::move(other.options)), text(std::move(other.text)),
	icon(std::move(other.icon)), onclick(std::move(other.onclick)), onclickOpen(std::move(other.onclickOpen)),
	condition(std::move(other.condition)), shortcut(std::move(other.shortcut)), type(std::move(other.type))
{
}

gui::Menubar& gui::Menubar::operator=(Menubar&& other) noexcept
{
	if (this != &other)
	{
		this->isBlocked = other.isBlocked;
		this->isHidden = other.isHidden;
		this->options = std::move(other.options);
		this->text = std::move(other.text);
		this->icon = std::move(other.icon);
		this->onclick = std::move(other.onclick);
		this->onclickOpen = std::move(other.onclickOpen);
		this->condition = std::move(other.condition);
		this->shortcut = std::move(other.shortcut);
		this->type = std::move(other.type);
	}
	return (*this);
}

gui::Menubar::~Menubar(void)
{
}
#pragma endregion


void gui::Menubar::Render(UI* myCreator)
{
	ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("MenuBar")));
	if (this->type == "main-level")
	{
		ImGui::BeginMainMenuBar();
	}

	for (auto& cur_El : this->options)
	{
		bool conditionTrue = true;
		PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(cur_El.condition, PyInterpreter::PyEvaluationTypes::Boolean, &conditionTrue);
		ImVec4 col = conditionTrue == false ? ImVec4(0.5f, 0.5f, 0.5f, 1.f) : ImVec4(1.f, 1.f, 1.f, 1.f);

		// If the current element type is a drop down menu, draw a menu item
		if (cur_El.type == "menu")
		{
			ImGui::PushStyleColor(ImGuiCol_Text, col);

			if (ImGui::BeginMenu(cur_El.text.c_str(), !this->isBlocked && conditionTrue))
			{
				cur_El.Render(myCreator);
				ImGui::EndMenu();
			}
			ImGui::PopStyleColor();
		}

		//If the current element is a menu option, draw that option
		else if (cur_El.type == "command")
		{
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, conditionTrue == false);
			
			//If it has an icon, displays that just before the text
			if (cur_El.icon != "")
			{
				ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
				ImGui::Text((const char*)GetIconUTF8Reference(cur_El.icon));
				ImGui::PopFont();
				ImGui::SameLine();
			}

			ImGui::Indent();
			if (ImGui::MenuItem(cur_El.text.c_str(), cur_El.shortcut.c_str()))
			{
				if (conditionTrue == false)
				{
					ImGui::Unindent();
					ImGui::PopStyleColor();
					ImGui::PopItemFlag();
					continue;
				}
				if (cur_El.onclickOpen.empty() == false)
				{
					if (myCreator->GetIframesMap().contains(cur_El.onclickOpen))
						myCreator->GetIframesMap()[cur_El.onclickOpen]->Open();
				}
				else if (cur_El.onclick.empty() == false)
					PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(cur_El.onclick);

				ImGui::Unindent();
				ImGui::PopStyleColor();
				ImGui::PopItemFlag();
				break;
			}

			// The following instructions are added just because there is a break instruction some lines before;
			// they will prevent crashes or unexpected behaviors
			ImGui::Unindent();
			ImGui::PopStyleColor();
			ImGui::PopItemFlag();
		}
	}
	ImGui::PopFont();
	if (this->type == "main-level")
		ImGui::EndMainMenuBar();
}
