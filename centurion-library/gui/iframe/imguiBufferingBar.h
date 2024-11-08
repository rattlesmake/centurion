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
	class ImGuiBufferingBar : public ImGuiElement
	{
	public:
		explicit ImGuiBufferingBar(std::shared_ptr<Iframe> iframeSP);
		ImGuiBufferingBar(const ImGuiBufferingBar& other) = delete;
		ImGuiBufferingBar& operator=(const ImGuiBufferingBar& other) = delete;
		~ImGuiBufferingBar(void);

		void Render(void) override;

		#pragma region To scripts members:
		[[nodiscard]] float GetProgress(void) const;
		void SetProgress(const float progress);
		#pragma endregion
	private:
		float progress = 0.f; // Value between 0.f and 1.f
	};
}
