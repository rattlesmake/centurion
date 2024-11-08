#include <iframe/imguiInputInt.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	ImGuiInputInt::ImGuiInputInt(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_inputInt, std::move(iframeSP))
	{
	}

	ImGuiInputInt::~ImGuiInputInt(void)
	{
	}

	void ImGuiInputInt::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		if (this->intValue < this->minIntValue)
			this->intValue = this->minIntValue;
		if (this->intValue > this->maxIntValue) 
			this->intValue = this->maxIntValue;

		const ImVec2 pos = this->GetPosition();
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::PushItemWidth(this->GetSize().x);
		ImGui::SetCursorPos(pos);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->inputIntData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Button, iframeSkin->inputIntData.button_background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, iframeSkin->inputIntData.button_selected_background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, iframeSkin->inputIntData.button_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->inputIntData.text_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->inputIntData.font));
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !this->bIsEnabled);

		// Auto detect alignment: right if xoffset is more than windows size / 2; left otherwise.
		if (pos.x >= this->iframeWP.lock()->GetSize().x / 2)
		{
			ImGui::InputInt(this->label.c_str(), &this->intValue);
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(pos.x - ImGui::CalcTextSize(this->text.c_str(), nullptr, true).x - 8, pos.y));
			ImGui::Text(this->text.c_str());
		}
		else
		{
			const ImVec2 pos = this->GetPosition();
			ImGui::SetCursorPos(ImVec2(pos.x, pos.y + 2));
			ImGui::Text(this->text.c_str());
			ImGui::SameLine();
			ImGui::SetCursorPos(ImVec2(pos.x + ImGui::CalcTextSize(this->text.c_str(), nullptr, true).x + 8, pos.y));
			ImGui::InputInt(this->label.c_str(), &this->intValue);
		}
		this->bIsActive = ImGui::IsItemActive();
		ImGui::PopItemFlag();
		ImGui::PopFont();
		ImGui::PopStyleColor(5);
		ImGui::PopItemWidth();
	}

	int32_t ImGuiInputInt::GetValue(void) const
	{
		return this->intValue;
	}

	void ImGuiInputInt::SetValue(const int32_t new_value)
	{
		this->intValue = new_value;
	}

	void ImGuiInputInt::SetMinMax(const int32_t min, const int32_t max)
	{
		this->minIntValue = min; 
		this->maxIntValue = max;
	}
}
