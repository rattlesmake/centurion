#include <iframe/imguiTextInput.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <services/sqlservice.h>
#include <services/pyservice.h>

namespace gui
{
	#pragma region Constructors and destructor:
	ImGuiTextInput::ImGuiTextInput(std::shared_ptr<Iframe> iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_textInput, std::move(iframeSP))
	{
	}

	ImGuiTextInput::ImGuiTextInput(ClassesTypes type, std::shared_ptr<Iframe>&& iframeSP) :
		ImGuiElement(type, std::move(iframeSP))
	{
	}

	ImGuiTextInput::~ImGuiTextInput(void)
	{
	}
	#pragma endregion

	void ImGuiTextInput::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::PushItemWidth(this->GetSize().x);
		ImGui::SetCursorPos(this->GetPosition());
		if (iframeSkin->textInputData.has_background)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->textInputData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->textInputData.text_color);
		ImGui::PushStyleColor(ImGuiCol_Border, iframeSkin->textInputData.border_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->textInputData.font));
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_AnyInputReturnsTrue;
		if (this->ascii)
			flags |= ImGuiInputTextFlags_BlockSpecials;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !this->bIsEnabled);
		ImGui::InputText(this->label.c_str(), this->inputBuf, IM_ARRAYSIZE(this->inputBuf), flags);
		if (ImGui::IsItemHovered() == true && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
		}
		this->text = inputBuf;
		ImGui::PopItemWidth();
		if (iframeSkin->textInputData.has_background)
			ImGui::PopStyleColor();
		ImGui::PopItemFlag();
		ImGui::PopStyleColor(2);
		ImGui::PopFont();
	}

	void ImGuiTextInput::SetPlaceholder(std::string new_text, const bool bTranslate)
	{
		// Firstly, call parent method to set placeholder and eventually to translate it
		this->ImGuiElement::SetPlaceholder(std::move(new_text), bTranslate);

		this->Reset();
	}

	void ImGuiTextInput::SetText(std::string new_text)
	{
		this->text = std::move(new_text);
		for (size_t i = 0; i < 256; i++)
			this->inputBuf[i] = '\0';
		const size_t nTexts = this->text.size();
		for (size_t i = 0; i < nTexts; i++)
			this->inputBuf[i] = this->text[i];
	}

	void ImGuiTextInput::Reset(void)
	{
		this->SetText(this->placeholder);
	}
}
