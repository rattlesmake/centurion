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
	class ImGuiCheckBox : public ImGuiElement
	{
	public:
		explicit ImGuiCheckBox(std::shared_ptr<Iframe> iframeSP);
		ImGuiCheckBox(const ImGuiCheckBox& other) = delete;
		ImGuiCheckBox& operator=(const ImGuiCheckBox& other) = delete;
		~ImGuiCheckBox(void);

		void Render(void) override;

		#pragma region To scripts members:
		[[nodiscard]] bool IsChecked(void) const;
		void SetChecked(const bool status);
		#pragma endregion
	private:
		bool bIsChecked = false;
	};
}
