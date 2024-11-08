#include <iframe/imguiCheckBox.h>
#include <services/pyservice.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	ImGuiCheckBox::ImGuiCheckBox(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_checkBox, std::move(iframeSP))
	{
	}

	ImGuiCheckBox::~ImGuiCheckBox(void)
	{
	}

	void ImGuiCheckBox::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->checkBoxData.background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, iframeSkin->checkBoxData.hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, iframeSkin->checkBoxData.selected_background_color);
		ImGui::PushStyleColor(ImGuiCol_CheckMark, iframeSkin->checkBoxData.mark_color);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->checkBoxData.text_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->checkBoxData.font));
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !this->bIsEnabled);
		if (ImGui::Checkbox(this->text.c_str(), &this->bIsChecked))
		{
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
		}
		ImGui::PopItemFlag();
		ImGui::PopStyleColor(5);
		ImGui::PopFont();
	}

	#pragma region To scripts members:
	bool ImGuiCheckBox::IsChecked(void) const
	{
		return this->bIsChecked;
	}

	void ImGuiCheckBox::SetChecked(const bool status)
	{
		this->bIsChecked = status;
	}
	#pragma endregion
}
