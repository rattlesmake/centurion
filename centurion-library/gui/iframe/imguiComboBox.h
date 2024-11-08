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
	class ImGuiComboBox : public ImGuiElementWithOptions
	{
	public:
		explicit ImGuiComboBox(std::shared_ptr<Iframe> iframeSP);
		ImGuiComboBox(const ImGuiComboBox& other) = delete;
		ImGuiComboBox& operator=(const ImGuiComboBox& other) = delete;
		~ImGuiComboBox(void);

		void Render(void) override;
	};
}
