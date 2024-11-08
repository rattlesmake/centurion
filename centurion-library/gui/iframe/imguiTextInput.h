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
	class ImGuiTextInput : public ImGuiElement
	{
	public:
		explicit ImGuiTextInput(std::shared_ptr<Iframe> iframeSP);
		ImGuiTextInput(const ImGuiTextInput& other) = delete;
		ImGuiTextInput& operator=(const ImGuiTextInput& other) = delete;
		virtual ~ImGuiTextInput(void);

		virtual void Render(void) override;

		void SetPlaceholder(std::string new_text, const bool bTranslate) override;
		void SetText(std::string new_text) override;

		void Reset(void);
	protected:
		ImGuiTextInput(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP);

		char inputBuf[256] { '\0' };
	};
}
