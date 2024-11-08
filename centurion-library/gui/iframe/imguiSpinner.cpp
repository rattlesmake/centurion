#include <iframe/imguiSpinner.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	ImGuiSpinner::ImGuiSpinner(std::shared_ptr<Iframe> iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_spinner, std::move(iframeSP))
	{
	}

	ImGuiSpinner::~ImGuiSpinner(void)
	{
	}

	void ImGuiSpinner::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->radius = TryParseDynamicFloatAttribute(el, "radius");
		this->thickness = TryParseDynamicIntAttribute(el, "thickness");
	}

	void ImGuiSpinner::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushStyleColor(ImGuiCol_Button, iframeSkin->spinnerData.color);
		ImGui::Spinner(this->label.c_str(), this->radius, this->thickness);
		ImGui::PopStyleColor();
	}
}
