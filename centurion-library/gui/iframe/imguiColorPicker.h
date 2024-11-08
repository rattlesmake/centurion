/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iframe/imguiElement.h>
#include <players/color.h>

namespace gui
{
	class ImGuiColorPicker : public ImGuiElement
	{
	public:
		explicit ImGuiColorPicker(std::shared_ptr<Iframe> iframeSP);
		ImGuiColorPicker(const ImGuiColorPicker& other) = delete;
		ImGuiColorPicker& operator=(const ImGuiColorPicker& other) = delete;
		~ImGuiColorPicker(void);

		void Render(void) override;
	};
}
