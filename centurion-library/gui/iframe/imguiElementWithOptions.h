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
	class ImGuiElementWithOptions : public ImGuiElement
	{
	public:
		ImGuiElementWithOptions(const ImGuiElementWithOptions& other) = delete;
		ImGuiElementWithOptions& operator=(const ImGuiElementWithOptions& other) = delete;
		virtual ~ImGuiElementWithOptions(void);

		#pragma region To scripts members:
		[[nodiscard]] uint32_t GetSelectedOptionIndex(void) const;
		[[nodiscard]] std::vector<std::string> GetListOfStrings(void) const;
		[[nodiscard]] std::string GetSelectedOption(void) const;
		void ResetOptions(void);
		void ResetSelectedOption(void);
		void UpdateOptions(std::vector<std::string> listOfStrings, const std::string prefix, const std::string current);
		#pragma endregion
	protected:
		ImGuiElementWithOptions(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP);

		std::vector<std::string> listOfStrings;
		std::vector<std::string> listOfTranslatedStrings;
		std::vector<bool> listOfBooleans;
		std::string selectedOption;
		uint32_t currentIndex = 0;
	};
}
