#include "imguiElement.h"

#include <services/sqlservice.h>
#include <services/pyservice.h>

#include <iframe/iframe.h>
//ImGui Iframe elements
#include <iframe/imgui_elements_include.h>

namespace gui
{
	#if CENTURION_DEBUG_MODE
	// Static attributes initialization
	uint32_t ImGuiElement::nItems = 0;
	uint32_t ImGuiElement::COUNTER_ELEMENTS_CREATIONS = 0;
	uint32_t ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX = 0;
	#endif


	#pragma region Constructors and destructor:
	ImGuiElement::ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP) : 
		classType(type), iframeWP(std::move(iframeSP))
	{
		this->label = "##item" + std::to_string(ImGuiElement::nItems);
		#if CENTURION_DEBUG_MODE
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS++;
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX++;
		#endif
	}

	ImGuiElement::ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size) :
		classType(type), iframeWP(std::move(iframeSP)), id(id)
	{
		this->label = "##item" + std::to_string(ImGuiElement::nItems);
		this->SetPosition(std::move(pos));
		this->SetSize(std::move(size));

		#if CENTURION_DEBUG_MODE
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS++;
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX++;
		#endif
	}

	ImGuiElement::ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, std::string&& text) :
		classType(type), iframeWP(std::move(iframeSP)), id(id), text(std::move(text))
	{
		this->label = "##item" + std::to_string(ImGuiElement::nItems);
		this->SetPosition(std::move(pos));

		#if CENTURION_DEBUG_MODE
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS++;
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX++;
		#endif
	}

	ImGuiElement::ImGuiElement(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& text, std::string&& onclickScript) :
		classType(type), iframeWP(std::move(iframeSP)), id(id), text(std::move(text)), onclick(std::move(onclickScript))
	{
		this->label = "##item" + std::to_string(ImGuiElement::nItems);
		this->SetPosition(std::move(pos));
		this->SetSize(std::move(size));

		#if CENTURION_DEBUG_MODE
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS++;
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX++;
		#endif
	}

	ImGuiElement::~ImGuiElement(void)
	{
		#if CENTURION_DEBUG_MODE
		ImGuiElement::COUNTER_ELEMENTS_CREATIONS--;
		if (ImGuiElement::COUNTER_ELEMENTS_CREATIONS == 0)
			std::cout << "[DEBUG] All " << ImGuiElement::COUNTER_ELEMENTS_CREATIONS_MAX << " instances of elements has been correctly cleaned" << std::endl;
		#endif
	}
	#pragma endregion


	#pragma region Static members:
	std::shared_ptr<ImGuiElement> ImGuiElement::CreateElement(std::string tag, XMLElement* el, std::shared_ptr<Iframe> iframeOwner)
	{
		// ASSERTION: a GUI element must have an existing iframe that owns it
		assert(iframeOwner);

		std::shared_ptr<ImGuiElement> new_el;
		if (tag == "buttonArray")
			new_el = ImGuiImageButton::CreateFromXml(std::move(iframeOwner), el);
		else if (tag == "imageArray")
			new_el = ImGuiImage::CreateFromXml(std::move(iframeOwner), el);
		else if (tag == "textListArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiTextList(std::move(iframeOwner)));
		else if (tag == "comboBoxArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiComboBox(std::move(iframeOwner)));
		else if (tag == "textInputArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiTextInput(std::move(iframeOwner)));
		else if (tag == "textInputMultilineArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiTextInputMultiline(std::move(iframeOwner)));
		else if (tag == "textArray")
			new_el = ImGuiText::CreateFromXml(std::move(iframeOwner), el);
		else if (tag == "textWrappedArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiTextWrapped(std::move(iframeOwner)));
		else if (tag == "inputIntArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiInputInt(std::move(iframeOwner)));
		else if (tag == "checkBoxArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiCheckBox(std::move(iframeOwner)));
		else if (tag == "colorPickerArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiColorPicker(std::move(iframeOwner)));
		else if (tag == "sliderArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiSlider(std::move(iframeOwner)));
		else if (tag == "progressBarArray")
			new_el = ImGuiProgressBar::CreateFromXml(std::move(iframeOwner), el);
		else if (tag == "bufferingBarArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiBufferingBar(std::move(iframeOwner)));
		else if (tag == "spinnerArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiSpinner(std::move(iframeOwner)));
		else if (tag == "multipleChoiceArray")
			new_el = std::shared_ptr<ImGuiElement>(new ImGuiMultipleChoice(std::move(iframeOwner)));
		else
			throw std::runtime_error("Invalid GUI element"); // TODO exception - Creare apposita eccezione

		static std::set<std::string> tmpSet{
			"progressBarArray",
			"imageArray",
			"textArray",
			"buttonArray"
		};
		if (tmpSet.contains(tag) == false)  //TMP if
		{
			new_el->InitFromXmlElement(el);
		}
		return new_el;
	}

	ImGuiElement::ClassesTypes ImGuiElement::GetImGuiElementTypeByStr(const std::string& tag)
	{
		static std::unordered_map<std::string, ImGuiElement::ClassesTypes> typeByTag{
			{ "button", ImGuiElement::ClassesTypes::e_imageButton },
			{ "image", ImGuiElement::ClassesTypes::e_image },
			{ "text", ImGuiElement::ClassesTypes::e_text },
			{ "textWrapped", ImGuiElement::ClassesTypes::e_textWrapped },
			{ "textList", ImGuiElement::ClassesTypes::e_textList },
			{ "textInput", ImGuiElement::ClassesTypes::e_textInput },
			{ "textInputMultiline", ImGuiElement::ClassesTypes::e_textInputMultiline },
			{ "comboBox", ImGuiElement::ClassesTypes::e_comboBox },
			{ "inputInt", ImGuiElement::ClassesTypes::e_inputInt },
			{ "checkBox", ImGuiElement::ClassesTypes::e_checkBox },
			{ "colorPicker", ImGuiElement::ClassesTypes::e_colorPicker },
			{ "slider", ImGuiElement::ClassesTypes::e_slider },
			{ "progressBar", ImGuiElement::ClassesTypes::e_progressBar },
			{ "bufferingBar", ImGuiElement::ClassesTypes::e_bufferingBar },
			{ "spinner", ImGuiElement::ClassesTypes::e_spinner },
			{ "editorTree", ImGuiElement::ClassesTypes::e_editorTree },
			{ "multipleChoice", ImGuiElement::ClassesTypes::e_multipleChoice },
		};
		return typeByTag.contains(tag) ? typeByTag.at(tag) : ImGuiElement::ClassesTypes::e_unknown;
	}

	const std::list<std::string>& ImGuiElement::GetImGuiElementArrayTags(void)
	{
		// EditorTreeArray doesn't exist because it is a special class and there should be only one present for each environment
		// and it should be called only by a function and not in by reading it a XML file.
		static const std::list<std::string> tags{
			"buttonArray", 
			"imageArray", 
			"textArray", 
			"textWrappedArray", 
			"textListArray", 
			"textInputArray", 
			"textInputMultilineArray", 
			"comboBoxArray", 
			"inputIntArray", 
			"checkBoxArray", 
			"colorPickerArray", 
			"sliderArray", 
			"progressBarArray", 
			"bufferingBarArray", 
			"spinnerArray", 
			"multipleChoiceArray"
		};
		return tags;
	}
	#pragma endregion


	void ImGuiElement::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		this->id = static_cast<elementID_t>(TryParseIntAttribute(el, "id"));

		this->onclick = TryParseStrAttribute(el, "onclick");
		if (this->onclick.empty() == false)
			this->onclick = this->onclick + PyService::GetInstance().GetArgs(this->iframeWP.lock()->GetEnv());  // It depends on the environment

		std::string stringName = TryParseStrAttribute(el, "stringName");
		std::string placeHolder = TryParseStrAttribute(el, "placeholder");
		std::list<dbWord_t> translationsToGet{ stringName, placeHolder };

		auto translations = SqlService::GetInstance().GetTranslations(std::move(translationsToGet), false);

		if (translations.contains(stringName) && translations.empty() == false)
			this->text = translations.at(stringName);
		else
			this->text = std::move(stringName);
		this->icon = TryParseStrAttribute(el, "icon");

		const float sizeX =  TryParseDynamicFloatAttribute(el, "width");
		const float sizeY = TryParseDynamicFloatAttribute(el, "height");
		this->SetSize(ImVec2(sizeX, sizeY));

		auto iframeSizeY = this->iframeWP.lock()->GetSize().y;
		this->SetPosition(ImVec2(TryParseDynamicFloatAttribute(el, "xOffset"), iframeSizeY - TryParseDynamicFloatAttribute(el, "yOffset")));

		this->type = TryParseStrAttribute(el, "type");
		this->bIsBackground = TryParseBoolAttribute(el, "background");
		this->colors = ImColor(TryParseDynamicFloatAttribute(el, "r") / 255.f, TryParseDynamicFloatAttribute(el, "g") / 255.f, TryParseDynamicFloatAttribute(el, "b") / 255.f, 1.f);
		
		ImGuiElement::nItems++;
		if (translations.contains(placeHolder) == true)
			this->placeholder = translations.at(placeHolder);
		else
			this->placeholder = std::move(placeHolder);
		el->QueryBoolAttribute("ascii", &this->ascii);
		el->QueryIntAttribute("defaultValue", &this->intValue);
		el->QueryIntAttribute("minValue", &this->minIntValue);
		this->maxIntValue = TryParseDynamicIntAttribute(el, "maxValue");
		el->QueryBoolAttribute("isEnabled", &this->bIsEnabled);
	}


	#pragma region To scripts members:
	ImGuiElement::elementID_t ImGuiElement::GetId(void) const
	{
		return this->id;
	}

	void ImGuiElement::SetPlaceholder(std::string new_placeholder, const bool bTranslate)
	{
		this->placeholder = bTranslate ? SqlService::GetInstance().GetTranslation(new_placeholder, false) : std::move(new_placeholder);
	}

	std::string ImGuiElement::GetText(void) const
	{
		return this->text;
	}

	void ImGuiElement::SetText(std::string new_text)
	{
		this->text = std::move(new_text);
	}

	void ImGuiElement::SetTextWithTranslation(std::string new_text, const bool bTranslate)
	{
		this->text = bTranslate ? SqlService::GetInstance().GetTranslation(new_text, false) : std::move(new_text);
	}

	bool ImGuiElement::IsActive(void) const
	{
		return this->bIsActive;
	}

	bool ImGuiElement::IsEnabled(void) const
	{
		return this->bIsEnabled;
	}

	bool ImGuiElement::IsHidden(void) const
	{
		return this->bIsHidden;
	}

	void ImGuiElement::SetPosition_Py(const std::pair<float, float> pos)
	{
		this->SetPosition(ImVec2(pos.first, pos.second));
	}

	std::pair<float, float> ImGuiElement::GetPosition_Py(void) const
	{
		return std::pair<float, float>(this->position.x, this->position.y);
	}
	#pragma endregion


	std::string ImGuiElement::GetPlaceholder(void) const
	{
		return this->placeholder;
	}

	ImVec2 ImGuiElement::GetPosition(void) const
	{
		return this->position;
	}

	ImVec2 ImGuiElement::GetSize(void) const
	{
		return this->size;
	}

	ImGuiElement::ClassesTypes ImGuiElement::GetClassType(void) const
	{
		return this->classType;
	}

	bool ImGuiElement::IsClicked(void) const
	{
		return this->bIsClicked;
	}

	void ImGuiElement::SetPosition(ImVec2 _position)
	{
		this->position = std::move(_position);
		auto iframeSP = this->iframeWP.lock();
		auto iframePos = iframeSP->GetPosition();
		auto iframeSize = iframeSP->GetSize();
		if (this->position.x < 0)
			this->position.x = iframePos.x + iframeSize.x + this->position.x;
		if (this->position.y < 0)
			this->position.y = iframePos.y + iframeSize.y + this->position.y;
	}

	void ImGuiElement::SetSize(ImVec2 _size)
	{
		this->size = std::move(_size);
		auto iframeSize = this->iframeWP.lock()->GetSize();
		if (this->size.x < 0)
			this->size.x = 100;
		if (this->size.y < 0)
			this->size.y = 100;
		if (this->size.x > iframeSize.x)
			this->size.x = iframeSize.x;
		if (this->size.y > iframeSize.y)
			this->size.y = iframeSize.y;
	}

	void ImGuiElement::SetSizeX(const float xSize)
	{
		this->SetSize(ImVec2(xSize, this->size.y));
	}

	void ImGuiElement::SetSizeY(const float ySize)
	{
		this->SetSize(ImVec2(this->size.x, ySize));
	}

	void ImGuiElement::Hide(void)
	{
		this->bIsHidden = true;
	}

	void ImGuiElement::Show(void)
	{
		this->bIsHidden = false;
	}

	void ImGuiElement::SetEnableValue(const bool bEnable)
	{
		this->bIsEnabled = bEnable;
	}

	void ImGuiElement::SetHiddenValue(const bool bHidden)
	{
		this->bIsHidden = bHidden;
	}
}
