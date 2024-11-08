/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <tinyxml2_utils.h> // it includes string and tinyxml2

namespace gui { class Iframe; }

namespace gui
{
	/// <summary>
	/// Abstract class for a generic GUI element
	/// </summary>
	class ImGuiElement
	{
	public:
		typedef uint8_t elementID_t;
		enum class ClassesTypes
		{
			// If you add something here, please give a look to the following methods: ImGuiElement::GetImGuiElementTypeByStr, ImGuiElement::GetImGuiElementTags
			// ImGuiElement::CreateElement

			e_unknown,
			// Valid elements list
			e_imageButton,
			e_image,
			e_text,
			e_textList,
			e_textWrapped,
			e_comboBox,
			e_textInput,
			e_textInputMultiline,
			e_inputInt,
			e_checkBox,
			e_colorPicker,
			e_slider,
			e_progressBar,
			e_bufferingBar,
			e_spinner,
			e_editorTree,
			e_multipleChoice,
		};

		ImGuiElement(const ImGuiElement& other) = delete;
		ImGuiElement& operator=(const ImGuiElement& other) = delete;
		virtual ~ImGuiElement(void);

		#pragma region Static members:
		[[nodiscard]] static std::shared_ptr<ImGuiElement> CreateElement(std::string tag, XMLElement* el, std::shared_ptr<Iframe> iframeOwner);
		[[nodiscard]] static ImGuiElement::ClassesTypes GetImGuiElementTypeByStr(const std::string& tag);
		[[nodiscard]] static const std::list<std::string>& GetImGuiElementArrayTags(void);
		#pragma endregion

		virtual void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0);
		virtual void Render(void) = 0;


		#pragma region To scripts members:
		[[nodiscard]] ImGuiElement::elementID_t GetId(void) const;
		virtual void SetPlaceholder(std::string new_placeholder, const bool bTranslate);
		[[nodiscard]] virtual std::string GetText(void) const;
		virtual void SetText(std::string new_text);
		virtual void SetTextWithTranslation(std::string new_text, const bool bTranslate);

		[[nodiscard]] bool IsActive(void) const;
		[[nodiscard]] bool IsEnabled(void) const;
		[[nodiscard]] bool IsHidden(void) const;

		void SetPosition_Py(const std::pair<float, float> pos);
		[[nodiscard]] std::pair<float, float> GetPosition_Py(void) const;
		#pragma endregion
		
		[[nodiscard]] bool IsClicked(void) const;

		[[nodiscard]] std::string GetPlaceholder(void) const;
		[[nodiscard]] ImVec2 GetPosition(void) const;
		[[nodiscard]] ImVec2 GetSize(void) const;
		[[nodiscard]] ImGuiElement::ClassesTypes GetClassType(void) const;

		void SetEnableValue(const bool bEnable);
		void SetHiddenValue(const bool bHidden);
		void SetPosition(ImVec2 _position);
		void SetSize(ImVec2 _size);
		void SetSizeX(const float xSize);
		void SetSizeY(const float ySize);

		// Alternativs to SetHiddenValue
		void Hide(void);
		void Show(void);
	protected:
		ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP); 
		ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size);
		ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, std::string&& text);
		ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& text, std::string&& onclickScript);

		elementID_t id = 0;
		ClassesTypes classType{ ClassesTypes::e_unknown };
		
		ImVec4 colors{ 0.f, 0.f, 0.f, 0.f };

		// The iframe owning this GUI element
		std::weak_ptr<Iframe> iframeWP;

		std::string label;
		int32_t minIntValue = 0;
		int32_t maxIntValue = 10;

		bool bIsActive = false;
		bool bIsBackground = false;
		bool bIsClicked = false;
		bool bIsCreated = false;
		bool bIsEnabled = true;
		bool bIsHidden = false;
		bool bIsHovered = false;
		bool separator = false;
		bool ascii = false;
		bool conditionResult = false;

		std::string text;
		std::string fontName;
		std::string icon;
		std::string onclick;
		std::string	type;
		std::string placeholder;

		int intValue = 1;
	private:
		ImVec2 position{ 0.f, 0.f };
		ImVec2 size{ 0.f, 0.f };

		static uint32_t nItems;
		#if CENTURION_DEBUG_MODE
		static uint32_t COUNTER_ELEMENTS_CREATIONS;
		static uint32_t COUNTER_ELEMENTS_CREATIONS_MAX;
		#endif
	};
}
