/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <glm.hpp>

namespace rattlesmake { namespace image { class png_data; }; };

namespace gui
{
	class ImGuiTooltip
	{
	public:
		struct TooltipElementColor_s
		{
			TooltipElementColor_s(void);
			TooltipElementColor_s(const uint8_t R, const uint8_t G, const uint8_t B, const uint8_t alpha);
			// Color components (RGBa)
			uint8_t r = 0;
			uint8_t g = 0;
			uint8_t b = 0;
			uint8_t a = 255;
		};
		struct TooltipElementImageSize_s
		{
			TooltipElementImageSize_s(void);
			TooltipElementImageSize_s(const uint8_t x, const uint8_t y);
			uint8_t width = 20;
			uint8_t height = 20;
		};

		struct TooltipElementImage_s
		{

		};

		struct TooltipElement_s
		{
			// Each tooltip has its own text, that must be always specified, even if empty
			// Afterwards the user can optionally choose a RGBA color to apply to the same text; if not declared, it will be used the default one
			// The user can eventually set an image, that will be placed automatically aligned to the text

			explicit TooltipElement_s(std::string _text);
			TooltipElement_s(std::string _text, std::optional<TooltipElementColor_s> _textColor);
			TooltipElement_s(std::string _text, std::optional<TooltipElementColor_s> _textColor, std::shared_ptr<rattlesmake::image::png_data> _image, std::optional<TooltipElementImageSize_s> _imgCustomSize);

			TooltipElement_s(TooltipElement_s&& other) noexcept;
			[[nodiscard]] TooltipElement_s& operator=(TooltipElement_s&& other) noexcept;

			std::string text;
			std::optional<TooltipElementColor_s> textColor;
			std::shared_ptr<rattlesmake::image::png_data> image;
			std::optional<TooltipElementImageSize_s> imageCustomSize;
		};

		// A tooltip is composed by a series of elements shown consecutively in sequence
		typedef std::list<TooltipElement_s> tooltip_t;

		ImGuiTooltip(void);
		ImGuiTooltip(const uint8_t defaultR, const uint8_t defaultG, const uint8_t defaultB, const uint8_t defaultAlpha);
		ImGuiTooltip(const ImGuiTooltip& other) = delete;
		ImGuiTooltip& operator=(const ImGuiTooltip& other) = delete;
		~ImGuiTooltip(void);

		#pragma region Render functions
		void Render(void);
		#pragma endregion

		void SetFunctionToGetTooltipInfo(std::function<ImGuiTooltip::tooltip_t(void)>&& tooltipInfoFunct);
		void ResetFunctionToGetTooltipInfo(void);
	private:
		static void ShowTooltipIcon(const std::shared_ptr<rattlesmake::image::png_data>&& imageSP, const float imgWidth, const float imgHeight, std::string&& text, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha);
		static void ShowTooltipText(std::string&& text, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t alpha);

		// Pointer to a function (that must have a specific signature) to run in order to get tooltip info when button is hovered
		std::function<tooltip_t(void)> functToGetTooltipInfo;

		// Default components RGBa for texts
		TooltipElementColor_s defaultColor{ 255, 255, 255, 255 };
	};
}
