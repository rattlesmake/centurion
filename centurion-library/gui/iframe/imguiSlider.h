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
	class ImGuiSlider : public ImGuiElement
	{
	public:
		explicit ImGuiSlider(std::shared_ptr<Iframe> iframeSP);
		ImGuiSlider(const ImGuiSlider& other) = delete;
		ImGuiSlider& operator=(const ImGuiSlider& other) = delete;
		~ImGuiSlider(void);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
		void Render(void) override;

		#pragma region To scripts members:
		[[nodiscard]] int GetMinValue(void) const;
		void SetMinValue(const int minValue);

		[[nodiscard]] int GetMaxValue(void) const;
		void SetMaxValue(const int maxValue);

		[[nodiscard]] int GetValue(void) const;
		void SetValue(const int value);
		#pragma endregion
	private:
		bool displayText = false;
		int value = 0;
		int minValue = 0;
		int maxValue = 0;
	};
}
