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
	class ImGuiSpinner : public ImGuiElement
	{
	public:
		explicit ImGuiSpinner(std::shared_ptr<Iframe> iframeSP);
		ImGuiSpinner(const ImGuiSpinner& other) = delete;
		ImGuiSpinner& operator=(const ImGuiSpinner& other) = delete;
		~ImGuiSpinner(void);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
		void Render(void) override;
	
		uint16_t thickness = 0;
		float radius = 0.f;
	};
}
