/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iframe/imguiElement.h>

namespace rattlesmake { namespace image { class png; }; };

namespace gui
{
	class ImGuiImage : public ImGuiElement
	{
	public:
		ImGuiImage(const ImGuiImage& other) = delete;
		ImGuiImage& operator=(const ImGuiImage& other) = delete;
		~ImGuiImage(void);

		#pragma region Static members:
		[[nodiscard]] static std::shared_ptr<ImGuiImage> CreateImage(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, std::string imageName, std::string tooltip);
		[[nodiscard]] static std::shared_ptr<ImGuiImage> CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el);
		#pragma endregion

		void Render(void) override;

		[[nodiscard]] bool IsHovered(const int16_t x, const int16_t y, const ImVec2& size) const;
		void SetImage(std::string image);
	protected:
		explicit ImGuiImage(std::shared_ptr<Iframe>&& iframeSP);
		ImGuiImage(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& imageName, std::string&& tooltip);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
	private:
		void Create(void);

		uint16_t hoveringTime = 0;
		std::string tooltip;
		std::string imageName;
		std::shared_ptr<rattlesmake::image::png> cil_image;
		int Flags = 0;
		int xPos = 0, yPos = 0;
	};
}
