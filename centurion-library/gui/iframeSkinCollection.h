/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once


#include <memory>
#include <string>
#include <unordered_map>
#include <imgui.h>

namespace rattlesmake { namespace image { class png_data; }; };

namespace gui
{
	class Image;

	namespace IframeSkinCollection
	{
		class IframeSkin
		{
		public:
			~IframeSkin(void);

			static void Initialize(void);
			[[nodiscard]] static std::shared_ptr<IframeSkin> GetSkin(const std::string& skinName);

			void Create(void);
			void Render(ImDrawList* drawList, const ImVec2& _position, const ImVec2& _size);

			[[nodiscard]] float GetLeftWidth(void) const;
			[[nodiscard]] float GetRightWidth(void) const;
			[[nodiscard]] float GetTopHeight(void) const;
			[[nodiscard]] float GetBottomHeight(void) const;

			std::string back_name;
			std::string topleft_name;
			std::string topright_name;
			std::string bottomright_name;
			std::string bottomleft_name;
			std::string right_name;
			std::string left_name;
			std::string top_name;
			std::string bottom_name;
			std::string button_x_name;
			std::string name;
			std::shared_ptr<rattlesmake::image::png_data> back;
			std::shared_ptr<rattlesmake::image::png_data> topleft;
			std::shared_ptr<rattlesmake::image::png_data> topright;
			std::shared_ptr<rattlesmake::image::png_data> bottomright;
			std::shared_ptr<rattlesmake::image::png_data> bottomleft;
			std::shared_ptr<rattlesmake::image::png_data> right;
			std::shared_ptr<rattlesmake::image::png_data> left;
			std::shared_ptr<rattlesmake::image::png_data> top;
			std::shared_ptr<rattlesmake::image::png_data> bottom;
			std::shared_ptr<rattlesmake::image::png_data> button_x;

			// Title data
			struct GetTitleData
			{
				std::string font;
				ImVec4 text_color;
			};
			GetTitleData titleData;

			// Text Data
			struct GetTextData
			{
				std::string font;
				ImVec4 color;
			};
			GetTextData textData;

			// Tab Data
			struct GetTabData
			{
				std::string font, active_font;
				ImVec4 background_color, hover_background_color, selected_background_color, text_color;
			};
			GetTabData tabData;

			// Button data
			struct GetButtonData
			{
				std::string font, image_name, pressed_image_name, hover_image_name, disabled_image_name;
				ImVec4 text_color;
			};
			GetButtonData buttonData;

			// TextList Data
			struct GetTextListData
			{
				std::string font;
				ImVec4 background_color, hover_background_color, selected_background_color, pressed_background_color,
					slider_color, slider_background_color, slider_pressed_background_color, slider_hover_background_color,
					border_color, text_color;
				bool has_border = false;
			};
			GetTextListData textListData;

			// TextInput Data
			struct GetTextInputData
			{
				std::string font;
				ImVec4 background_color, border_color, text_color;
				bool has_background = false;
			};
			GetTextInputData textInputData;

			// TextInputMultiline Data
			struct GetTextInputMultilineData
			{
				std::string font;
				ImVec4 background_color, slider_color, slider_background_color, slider_pressed_background_color, slider_hover_background_color,
					border_color, text_color;
				bool has_background = false;
			};
			GetTextInputMultilineData textInputMultilineData;

			// ComboBox Data
			struct GetComboBoxData
			{
				std::string font;
				ImVec4 background_color, hover_background_color, arrow_background_color, arrow_hover_background_color,
					selectable_hover_background_color, selectable_pressed_background_color, text_color;
			};
			GetComboBoxData comboBoxData;

			// CheckBox Data
			struct GetCheckBoxData
			{
				std::string font;
				ImVec4 background_color, hover_background_color, selected_background_color, mark_color, text_color;
			};
			GetCheckBoxData checkBoxData;

			// InputInt Data
			struct GetInputIntData
			{
				std::string font;
				ImVec4 background_color, button_background_color, button_hover_background_color, button_selected_background_color, text_color;
			};
			GetInputIntData inputIntData;

			// Slider Data
			struct GetSliderData
			{
				std::string type, font;
				ImVec4 background_color, pressed_background_color, bar_background_color, bar_hover_background_color,
					bar_pressed_background_color, border_color, text_color;
			};
			GetSliderData sliderData;

			// ProgressBar Data
			struct GetProgressBarData
			{
				std::string font;
				ImVec4 background_color, bar_color, text_color;
			};
			GetProgressBarData progressBarData;

			// BufferingBar Data
			struct GetBufferingBarData
			{
				ImVec4 background_color, bar_color;
			};
			GetBufferingBarData bufferingBarData;

			// Spinner Data
			struct GetSpinnerData
			{
				ImVec4 color;
			};
			GetSpinnerData spinnerData;

			// Tree(nodes) Data
			struct GetTreeData
			{
				std::string font, font_big;
				ImVec4 node_background_color, node_hover_background_color, node_pressed_background_color, slider_color,
					slider_background_color, slider_pressed_background_color, slider_hover_background_color, resize_grip_color,
					resize_grip_hover_color, resize_grip_pressed_color, text_color;
			};
			GetTreeData treeData;

			// Multiple Choice Data
			struct GetMultipleChoiceData
			{
				std::string font;
				ImVec4 background_color, pressed_background_color, hover_background_color, option_background_color, border_color, text_color;
			};
			GetMultipleChoiceData multipleChoiceData;

			bool bCreated = false;
		private:
			IframeSkin(void);
			IframeSkin(const IframeSkin& other) = delete;
			IframeSkin& operator=(const IframeSkin& other) = delete;

			static std::unordered_map<std::string, std::shared_ptr<IframeSkin>> SKINS;
		};
	};
};
