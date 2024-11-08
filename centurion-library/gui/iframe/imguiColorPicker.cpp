#include <iframe/imguiColorPicker.h>

namespace gui
{
	ImGuiColorPicker::ImGuiColorPicker(std::shared_ptr<Iframe> iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_colorPicker, std::move(iframeSP))
	{
	}

	ImGuiColorPicker::~ImGuiColorPicker(void)
	{
	}

	void ImGuiColorPicker::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		ImGui::SetCursorPos(this->GetPosition());
		ImGui::ColorEdit4(this->label.c_str(), reinterpret_cast<float*>(&colors), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha);
	}
}
