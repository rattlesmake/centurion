/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iframe/imguiTextInput.h>

namespace gui
{
	class ImGuiTextInputMultiline : public ImGuiTextInput
	{
	public:
		explicit ImGuiTextInputMultiline(std::shared_ptr<Iframe> iframeSP);
		ImGuiTextInputMultiline(const ImGuiTextInputMultiline& other) = delete;
		ImGuiTextInputMultiline& operator=(const ImGuiTextInputMultiline& other) = delete;
		~ImGuiTextInputMultiline(void);

		void Render(void) override;

		[[nodiscard]] uint32_t GetCursorLine(void) const;
		[[nodiscard]] uint32_t GetCursorColumn(void) const;
		void UpdateCursorPos(void);
	private:
		uint32_t column = 0;
		uint32_t line = 0;
	};
}
