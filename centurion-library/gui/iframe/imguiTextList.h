/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <iframe/imguiElementWithOptions.h>

namespace gui
{
	class ImGuiTextList : public ImGuiElementWithOptions
	{
	public:
		explicit ImGuiTextList(std::shared_ptr<Iframe> iframeSP);
		ImGuiTextList(const ImGuiTextList& other) = delete;
		ImGuiTextList& operator=(const ImGuiTextList& other) = delete;
		~ImGuiTextList(void);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
		void Render(void) override;
	};
}
