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
	class ImGuiTextWrapped : public ImGuiElement
	{
	public:
		explicit ImGuiTextWrapped(std::shared_ptr<Iframe> iframeSP);
		ImGuiTextWrapped(const ImGuiTextWrapped& other) = delete;
		ImGuiTextWrapped& operator=(const ImGuiTextWrapped& other) = delete;
		~ImGuiTextWrapped(void);

		void Render(void) override;
	};
}
