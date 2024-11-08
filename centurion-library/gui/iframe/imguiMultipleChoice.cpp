#include <iframe/imguiMultipleChoice.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <services/sqlservice.h>

namespace gui
{
	ImGuiMultipleChoice::ImGuiMultipleChoice(std::shared_ptr<Iframe> iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_multipleChoice, std::move(iframeSP))
	{
	}

	ImGuiMultipleChoice::~ImGuiMultipleChoice(void)
	{
	}

	void gui::ImGuiMultipleChoice::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->alignment = TryParseStrAttribute(el, "alignment");
		for (XMLElement* i = el->FirstChildElement(); i != nullptr; i = i->NextSiblingElement())
		{
			assert(i->GetText() != nullptr);
			std::string option = i->GetText();
			assert(option.empty() == false);
			this->options.push_back(std::move(option));
		}
	}

	void ImGuiMultipleChoice::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		const ImVec2 pos = this->GetPosition();
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();

		ImGui::SetCursorPos(pos);
		const size_t n_options = this->options.size();
		for (size_t i = 0; i < n_options; i++)
		{
			if (this->alignment != "horizontal")
				ImGui::SetCursorPosX(pos.x);

			ImGui::PushStyleColor(ImGuiCol_Border, iframeSkin->multipleChoiceData.border_color);
			ImGui::PushStyleColor(ImGuiCol_CheckMark, iframeSkin->multipleChoiceData.option_background_color);
			ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->multipleChoiceData.background_color);
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, iframeSkin->multipleChoiceData.hover_background_color);
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, iframeSkin->multipleChoiceData.pressed_background_color);
			ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->multipleChoiceData.text_color);
			ImGui::PushFont(ImGui::GetFontByName(iframeSkin->multipleChoiceData.font));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.75f, 1.75f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
			ImGui::RadioButton(SqlService::GetInstance().GetTranslation(options[i], false).c_str(), &this->selected, static_cast<int>(i));
			if (this->alignment == "horizontal")
				ImGui::SameLine();
			ImGui::PopStyleVar(2);
			ImGui::PopFont();
			ImGui::PopStyleColor(6);
		}
	}

	void ImGuiMultipleChoice::SetSelectedIndex(const uint8_t val)
	{
		this->selected = val;
	}

	uint8_t ImGuiMultipleChoice::GetSelectedIndex(void) const
	{
		return this->selected;
	}
}
