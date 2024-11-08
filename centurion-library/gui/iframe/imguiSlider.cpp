#include <iframe/imguiSlider.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	ImGuiSlider::ImGuiSlider(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_slider, std::move(iframeSP))
	{
	}

	ImGuiSlider::~ImGuiSlider(void)
	{
	}

	void ImGuiSlider::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parents' method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		el->QueryBoolAttribute("displayText", &this->displayText);
		this->value = TryParseDynamicIntAttribute(el, "startingValue");
		this->minValue = TryParseDynamicIntAttribute(el, "minValue");
		this->maxValue = TryParseDynamicIntAttribute(el, "maxValue");
	}

	void ImGuiSlider::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		const ImVec2 size = this->GetSize();
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushItemWidth(size.x);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.f, size.y));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, iframeSkin->sliderData.background_color);
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, iframeSkin->sliderData.pressed_background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->sliderData.bar_background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, iframeSkin->sliderData.bar_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, iframeSkin->sliderData.bar_pressed_background_color);
		ImGui::PushStyleColor(ImGuiCol_Border, iframeSkin->sliderData.border_color);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->sliderData.text_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->sliderData.font));
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !this->bIsEnabled);
		ImGuiSliderFlags flags = ImGuiSliderFlags_None;
		if (this->displayText)
			flags |= ImGuiSliderFlags_DisplayText;
		if (iframeSkin->sliderData.type == "Rectangle" || iframeSkin->sliderData.type == "rectangle")
			flags |= ImGuiSliderFlags_Rectangle;

		ImGui::SliderInt(this->label.c_str(), &this->value, this->minValue, this->maxValue, "%d", flags);
		ImGui::PopItemFlag();
		ImGui::PopFont();
		ImGui::PopStyleColor(7);
		ImGui::PopStyleVar(2);
		ImGui::PopItemWidth();
	}

	#pragma region To scripts members:
	int ImGuiSlider::GetMinValue(void) const
	{
		return this->minValue;
	}

	void ImGuiSlider::SetMinValue(int minValue)
	{
		this->minValue = minValue;
		if (this->minValue > this->maxValue)
			this->maxValue = this->minValue;
	}

	int ImGuiSlider::GetMaxValue(void) const
	{
		return this->maxValue;
	}

	void ImGuiSlider::SetMaxValue(int maxValue)
	{
		this->maxValue = maxValue;
		if (this->maxValue < this->minValue)
			this->minValue = this->maxValue;
	}

	int ImGuiSlider::GetValue(void) const
	{
		return this->value;
	}

	void ImGuiSlider::SetValue(const int value)
	{
		this->value = value;
		if (this->value > this->maxValue)
			this->value = this->maxValue;
		if (this->value < this->minValue)
			this->value = this->minValue;
	}
	#pragma endregion
}
