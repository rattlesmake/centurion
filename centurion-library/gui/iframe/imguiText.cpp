#include <iframe/imguiText.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	#pragma region Constructors and destructor:
	ImGuiText::ImGuiText(std::shared_ptr<Iframe>&& iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_text, std::move(iframeSP))
	{
	}

	ImGuiText::ImGuiText(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, std::string&& text) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_text, std::move(iframeSP), id, std::move(pos), std::move(text))
	{
	}

	ImGuiText::~ImGuiText(void)
	{
	}
	#pragma endregion


	#pragma region Static members:
	ImVec2 ImGuiText::CalculateTextSize(const std::string& text, const std::string& font)
	{
		ImGui::PushFont(ImGui::GetFontByName(font));
		ImVec2 txtSize = ImGui::CalcTextSize(text.c_str());
		ImGui::PopFont();
		return txtSize;
	}

	std::shared_ptr<ImGuiText> ImGuiText::CreateText(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, std::string text)
	{
		std::shared_ptr<ImGuiText> newText{ new ImGuiText{ std::move(iframeSP), id, std::move(pos), std::move(text) } };
		newText->SetSize(ImGuiText::CalculateTextSize(newText->text, newText->fontName));
		return newText;
	}

	std::shared_ptr<ImGuiText> ImGuiText::CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el)
	{
		std::shared_ptr<ImGuiText> text{ new ImGuiText{ std::move(iframeSP) } };
		text->InitFromXmlElement(el);
		return text;
	}
	#pragma endregion


	void ImGuiText::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->align = TryParseStrAttribute(el, "align");
	}

	void ImGuiText::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->textData.color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->textData.font));
		if (this->align == "right")
			this->xOffset = static_cast<uint16_t>(ImGuiText::CalculateTextSize(this->text, iframeSkin->textData.font).x);
		else if (this->align == "center")
			this->xOffset = static_cast<uint16_t>(ImGuiText::CalculateTextSize(this->text, iframeSkin->textData.font).x) >> 1;
		else
			this->xOffset = 0;

		const ImVec2 pos = this->GetPosition();
		ImGui::SetCursorPos(ImVec2(pos.x - xOffset, pos.y));
		ImGui::Text(this->text.c_str());
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}
}
