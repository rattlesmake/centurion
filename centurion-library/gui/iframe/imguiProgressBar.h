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
	class ImGuiProgressBar : public ImGuiElement
	{
	public:
		ImGuiProgressBar(const ImGuiProgressBar& other) = delete;
		ImGuiProgressBar& operator=(const ImGuiProgressBar& other) = delete;
		~ImGuiProgressBar(void);

		#pragma region Static members:
		[[nodiscard]] static std::shared_ptr<ImGuiProgressBar> CreateProgressBar(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, const float progress, bool bInvertedColors);
		[[nodiscard]] static std::shared_ptr<ImGuiProgressBar> CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el);
		#pragma endregion

		void Render(void) override;

		#pragma region To scripts members:
		[[nodiscard]] bool GetInvertedColors(void) const;
		void SetInvertedColors(const bool bInverted);
		[[nodiscard]] float GetProgress(void) const;
		void SetProgress(float progress);
		#pragma endregion
	protected:
		explicit ImGuiProgressBar(std::shared_ptr<Iframe>&& iframeSP);
		ImGuiProgressBar(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, const float progress, bool bInvertedColor);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
	private:
		bool invertedColors = false;
		// A number between 0 and 1 (i.e. it's a percentange)
		float progress = 0.f;
	};
}
