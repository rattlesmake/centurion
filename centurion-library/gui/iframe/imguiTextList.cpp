#include <iframe/imguiTextList.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <icons.h>
#include <services/pyservice.h>

namespace gui
{
	ImGuiTextList::ImGuiTextList(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElementWithOptions(ImGuiElement::ClassesTypes::e_textList, std::move(iframeSP))
	{
	}

	ImGuiTextList::~ImGuiTextList(void)
	{
	}

	void ImGuiTextList::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElementWithOptions::InitFromXmlElement(el, yoffset);

		el->QueryBoolAttribute("separator", &this->separator);
	}

	void ImGuiTextList::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->textListData.text_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->textListData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Header, iframeSkin->textListData.selected_background_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, iframeSkin->textListData.hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, iframeSkin->textListData.pressed_background_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, iframeSkin->textListData.slider_background_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, iframeSkin->textListData.slider_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, iframeSkin->textListData.slider_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, iframeSkin->textListData.slider_pressed_background_color);
		if (iframeSkin->textListData.has_border)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Border, iframeSkin->textListData.border_color);
		}
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->textListData.font));
		if (ImGui::ListBoxHeader(this->label.c_str(), this->GetSize()))
		{
			size_t k = 0;
			for (auto const& s : this->listOfTranslatedStrings)
			{
				if (this->icon != "")
				{
					ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
					if (s == "..")
						ImGui::Text((const char*)GetIconUTF8Reference("ICON_FOLDER_OPENED"));
					else
						ImGui::Text((const char*)GetIconUTF8Reference(this->icon));
					ImGui::PopFont();
					ImGui::SameLine();
				}
				assert(k < this->listOfBooleans.size());
				if (ImGui::Selectable(s.c_str(), this->listOfBooleans.at(k)))
				{
					this->bIsActive = true;
					this->selectedOption = this->listOfStrings.at(k);
					this->listOfBooleans[k] = true;
					PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
					const size_t nBooleans = this->listOfBooleans.size();
					for (size_t i = 0; i < nBooleans; i++)
					{
						if (i != k)
							this->listOfBooleans.at(i) = false;
						else
							this->currentIndex = static_cast<uint32_t>(i);
					}
				}
				if (this->separator)
					ImGui::Separator();
				k++;
			}
			ImGui::ListBoxFooter();
		}
		ImGui::PopStyleColor(9);
		if (iframeSkin->textListData.has_border)
		{
			ImGui::PopStyleVar();
			ImGui::PopStyleColor();
		}
		ImGui::PopFont();
	}
}
