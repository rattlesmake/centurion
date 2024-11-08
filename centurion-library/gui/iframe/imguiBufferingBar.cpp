#include <iframe/imguiBufferingBar.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	ImGuiBufferingBar::ImGuiBufferingBar(std::shared_ptr<Iframe> iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_bufferingBar, std::move(iframeSP))
	{
	}

	ImGuiBufferingBar::~ImGuiBufferingBar(void)
	{
	}

	void ImGuiBufferingBar::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushStyleColor(ImGuiCol_Button, iframeSkin->bufferingBarData.background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, iframeSkin->bufferingBarData.bar_color);
		ImGui::BufferingBar(this->label.c_str(), progress, this->GetSize());
		ImGui::PopStyleColor(2);
	}

	#pragma region To scripts members:
	float ImGuiBufferingBar::GetProgress(void) const
	{
		return this->progress;
	}

	void ImGuiBufferingBar::SetProgress(const float progress)
	{
		if (this->progress > 1.f)
			this->progress = 1.f;
		if (this->progress < 0.f)
			this->progress = 0.f;
		this->progress = progress;
	}
	#pragma endregion
}
