#include <iframe/imguiComboBox.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <services/pyservice.h>

namespace gui
{
	ImGuiComboBox::ImGuiComboBox(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElementWithOptions(ImGuiElement::ClassesTypes::e_comboBox, std::move(iframeSP))
	{
	}

	ImGuiComboBox::~ImGuiComboBox(void)
	{
	}

	void ImGuiComboBox::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		const ImVec2 pos = this->GetPosition();
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();

		ImGui::SetCursorPos(pos);
		ImGui::PushItemWidth(this->GetSize().x);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->comboBoxData.background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, iframeSkin->comboBoxData.hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_Button, iframeSkin->comboBoxData.arrow_background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, iframeSkin->comboBoxData.arrow_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_PopupBg, iframeSkin->comboBoxData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Header, iframeSkin->comboBoxData.background_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, iframeSkin->comboBoxData.selectable_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, iframeSkin->comboBoxData.selectable_pressed_background_color);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->comboBoxData.text_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->comboBoxData.font));

		//Auto detect alignment: right if xoffset is more than windows size / 2; left otherwise.
		auto iframeSizeX = this->iframeWP.lock()->GetSize().x;
		if (pos.x >= iframeSizeX / 2)
		{
			if (ImGui::Combo(this->label.c_str(), reinterpret_cast<int*>(&this->currentIndex), this->listOfTranslatedStrings))
			{
				PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(pos.x - ImGui::CalcTextSize(this->text.c_str(), nullptr, true).x - 8, pos.y));
			ImGui::Text(this->text.c_str());
		}
		else
		{
			ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 2));
			ImGui::Text(this->text.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(pos.x + ImGui::CalcTextSize(this->text.c_str(), nullptr, true).x + 8, pos.y));
			if (ImGui::Combo(this->label.c_str(), reinterpret_cast<int*>(&this->currentIndex), this->listOfTranslatedStrings))
			{
				this->selectedOption = this->listOfStrings[this->currentIndex];
				PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
			}
		}
		ImGui::PopStyleColor(9);
		ImGui::PopFont();
		ImGui::PopItemWidth();
	}
}
