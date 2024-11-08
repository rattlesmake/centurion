/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iframe/ImGuiElement.h>

namespace gui
{
	class ImGuiText : public ImGuiElement
	{
	public:
		ImGuiText(const ImGuiText& other) = delete;
		ImGuiText& operator=(const ImGuiText& other) = delete;
		~ImGuiText(void);

		#pragma region Static members:
		[[nodiscard]] static ImVec2 CalculateTextSize(const std::string& text, const std::string& font);
		[[nodiscard]] static std::shared_ptr<ImGuiText> CreateText(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, std::string text);
		[[nodiscard]] static std::shared_ptr<ImGuiText> CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el);
		#pragma endregion

		void Render(void) override;
	protected:
		explicit ImGuiText(std::shared_ptr<Iframe>&& iframeSP);
		ImGuiText(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, std::string&& text);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;

		uint16_t xOffset = 0;
		std::string align{ "left" };
	};
}
