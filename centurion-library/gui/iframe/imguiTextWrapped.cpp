#include <iframe/imguiTextWrapped.h>

namespace gui
{
	ImGuiTextWrapped::ImGuiTextWrapped(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_textWrapped, std::move(iframeSP))
	{
	}

	ImGuiTextWrapped::~ImGuiTextWrapped(void)
	{
	}

	void ImGuiTextWrapped::Render(void)
	{
		if (this->bIsHidden == true)
			return;
		
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::TextWrapped(this->text.c_str());
	}
}
