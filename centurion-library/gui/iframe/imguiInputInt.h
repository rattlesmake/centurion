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
	class ImGuiInputInt : public ImGuiElement
	{
	public:
		explicit ImGuiInputInt(std::shared_ptr<Iframe> iframeSP);
		ImGuiInputInt(const ImGuiInputInt& other) = delete;
		ImGuiInputInt& operator=(const ImGuiInputInt& other) = delete;
		~ImGuiInputInt(void);

		void Render(void) override;

		[[nodiscard]] int32_t GetValue(void) const;
		void SetValue(const int32_t new_value);

		void SetMinMax(const int32_t min, const int32_t max);
	};
}
