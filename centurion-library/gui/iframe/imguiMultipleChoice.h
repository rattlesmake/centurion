/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iframe/imguiElement.h>

namespace gui
{ 
	class ImGuiMultipleChoice : public ImGuiElement
	{
	public:
		explicit ImGuiMultipleChoice(std::shared_ptr<Iframe> iframeSP);
		ImGuiMultipleChoice(const ImGuiMultipleChoice& other) = delete;
		ImGuiMultipleChoice& operator=(const ImGuiMultipleChoice& other) = delete;
		~ImGuiMultipleChoice(void);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
		void Render(void) override;

		void SetSelectedIndex(const uint8_t val);
		[[nodiscard]] uint8_t GetSelectedIndex(void) const;

		int selected = 0;
		std::string alignment{ "horizontal" };
		std::vector<std::string> options;
	};
}
