#include "imgui_tooltip.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <png_shader.h>

namespace gui
{
	#pragma region Constructors and destructor:
	ImGuiTooltip::ImGuiTooltip(void)
	{
	}

	ImGuiTooltip::ImGuiTooltip(const uint8_t defaultR, const uint8_t defaultG, const uint8_t defaultB, const uint8_t defaultAlpha) :
		defaultColor(defaultR, defaultG, defaultB, defaultAlpha)
	{
	}

	ImGuiTooltip::~ImGuiTooltip(void)
	{
	}

	ImGuiTooltip::TooltipElementColor_s::TooltipElementColor_s(void)
	{
	}

	ImGuiTooltip::TooltipElementColor_s::TooltipElementColor_s(const uint8_t R, const uint8_t G, const uint8_t B, const uint8_t alpha) :
		r(R), g(G), b(B), a(alpha)
	{
	}

	ImGuiTooltip::TooltipElementImageSize_s::TooltipElementImageSize_s(void)
	{
	}
	ImGuiTooltip::TooltipElementImageSize_s::TooltipElementImageSize_s(const uint8_t x, const uint8_t y) :
		width(x), height(y)
	{
	}

	ImGuiTooltip::TooltipElement_s::TooltipElement_s(std::string _text) :
		text(std::move(_text))
	{
	}
	ImGuiTooltip::TooltipElement_s::TooltipElement_s(std::string _text, std::optional<TooltipElementColor_s> _textColor) :
		text(std::move(_text)), textColor(std::move(_textColor))
	{
	}

	ImGuiTooltip::TooltipElement_s::TooltipElement_s(
		std::string _text, 
		std::optional<TooltipElementColor_s> _textColor, 
		std::shared_ptr<rattlesmake::image::png_data> _image, 
		std::optional<TooltipElementImageSize_s> _imgCustomSize
	) :
		text(std::move(_text)),
		textColor(std::move(_textColor)),
		image(std::move(_image)),
		imageCustomSize(std::move(_imgCustomSize))
	{
	}

	ImGuiTooltip::TooltipElement_s::TooltipElement_s(TooltipElement_s&& other) noexcept :
		text(std::move(other.text)), textColor(std::move(other.textColor)), image(std::move(other.image)), imageCustomSize(std::move(other.imageCustomSize))
	{
	}

	ImGuiTooltip::TooltipElement_s& ImGuiTooltip::TooltipElement_s::operator=(TooltipElement_s&& other) noexcept
	{
		if (this != &other)
		{
			this->text = std::move(other.text);
			this->textColor = std::move(other.textColor);
			this->image = std::move(other.image);
			this->imageCustomSize = std::move(other.imageCustomSize);
		}
		return (*this);
	}
	#pragma endregion


	#pragma region Render functions:
	void ImGuiTooltip::Render(void)
	{
		if (this->functToGetTooltipInfo)  // If tooltip has a function to get tooltipinfo
		{
			// Get tooltip info using provided function
			tooltip_t tooltipInfo = this->functToGetTooltipInfo();

			ImGui::BeginTooltipEx(0, ImGuiTooltipFlags_OverridePreviousTooltip);

			while (tooltipInfo.empty() == false)
			{
				// Get first element to show
				auto& tooltipEl = tooltipInfo.front();

				// Decide if it should use custom or default color for text and set color components
				const bool bHasCustomColor = tooltipEl.textColor.has_value();
				// Check whether red, green, blue, and alpha channels have proper values; set to black color otherwise, as default value.
				const uint8_t r = (bHasCustomColor == true) ? tooltipEl.textColor.value().r : this->defaultColor.r;
				const uint8_t g = (bHasCustomColor == true) ? tooltipEl.textColor.value().g : this->defaultColor.g;
				const uint8_t b = (bHasCustomColor == true) ? tooltipEl.textColor.value().b : this->defaultColor.b;
				const uint8_t a = (bHasCustomColor == true) ? tooltipEl.textColor.value().a : this->defaultColor.a;

				if (tooltipEl.image)  // If tooltip element has both an image and a text
				{
					const bool bHasImgCustomSize = tooltipEl.imageCustomSize.has_value();
					const float width = (bHasImgCustomSize == true) ? (float)tooltipEl.imageCustomSize.value().width : (float)tooltipEl.image->get_image_width();
					const float height = (bHasImgCustomSize == true) ? (float)tooltipEl.imageCustomSize.value().height : (float)tooltipEl.image->get_image_height();
					ImGuiTooltip::ShowTooltipIcon(std::move(tooltipEl.image), width, height, std::move(tooltipEl.text), r, g, b, a);
				}
				else  // Tooltip element has only a text
				{
					ImGuiTooltip::ShowTooltipText(std::move(tooltipEl.text), r, g, b, a);
				}

				// Remove the element just shown from the list of elements to show
				tooltipInfo.pop_front();
			}

			ImGui::EndTooltip();
		}
	}
	#pragma endregion


	void ImGuiTooltip::SetFunctionToGetTooltipInfo(std::function<ImGuiTooltip::tooltip_t(void)>&& tooltipInfoFunct)
	{
		this->functToGetTooltipInfo = std::move(tooltipInfoFunct);
	}

	void ImGuiTooltip::ResetFunctionToGetTooltipInfo(void)
	{
		this->functToGetTooltipInfo = std::function<tooltip_t(void)>();
	}

	#pragma region Private static members:
	void ImGuiTooltip::ShowTooltipIcon(const std::shared_ptr<rattlesmake::image::png_data>&& imageSP, const float imgWidth, const float imgHeight, std::string&& text, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha)
	{
		if (imageSP)
		{
			ImTextureID user_texture_id = (void*)(intptr_t)imageSP->get_opengl_texture_id();
			ImGui::Image(user_texture_id, ImVec2(imgWidth, imgHeight));
			ImGui::SameLine();
			ImGuiTooltip::ShowTooltipText(std::move(text), r, g, b, alpha);
			ImGui::SameLine();
		}
	}

	void ImGuiTooltip::ShowTooltipText(std::string&& text, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha)
	{
		// TODO - @lezzo Leggere, comprendere e tradurre in inglese dopo aver fatto le prime due cose asd
		// La traduzione del testo deve essere fatta dalla funzione ESTERNA che fornisce le info del tooltip.
		// Queste informazioni sono ottenute quando si effettua l'istruzione this->functToGetTooltipInfo() in ImGuiTooltip::Render
		// this->functToGetTooltipInfo() deve essere settato esternamente (se si desidera che il pulsante abbia un tooltip) invocando la funzione ImGuiTooltip::SetFunctionToGetTooltipInfo
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(r, g, b, alpha));
		ImGui::Text(text.c_str());
		ImGui::PopStyleColor();
	}
	#pragma endregion
	
}

