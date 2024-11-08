#include <iframe/imguiTextInputMultiline.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <services/pyservice.h>

#include <engine.h>
#include <GLFW/glfw3.h>
#include <keyboard.h>

namespace gui
{
	#pragma region Constructor and destructor:
	ImGuiTextInputMultiline::ImGuiTextInputMultiline(std::shared_ptr<Iframe> iframeSP) :
		ImGuiTextInput(ImGuiElement::ClassesTypes::e_textInputMultiline, std::move(iframeSP))
	{
	}

	ImGuiTextInputMultiline::~ImGuiTextInputMultiline(void)
	{
	}
	#pragma endregion

	void ImGuiTextInputMultiline::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::PushItemWidth(this->GetSize().x);
		ImGui::SetCursorPos(this->GetPosition());
		if (iframeSkin->textInputMultilineData.has_background)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->textInputMultilineData.background_color);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->textInputMultilineData.text_color);
		ImGui::PushStyleColor(ImGuiCol_Border, iframeSkin->textInputMultilineData.border_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, iframeSkin->textInputMultilineData.slider_background_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, iframeSkin->textInputMultilineData.slider_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, iframeSkin->textInputMultilineData.slider_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, iframeSkin->textInputMultilineData.slider_pressed_background_color);
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->textInputMultilineData.font));
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AllowTabInput;
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !this->bIsEnabled);
		ImGui::InputTextMultiline(this->label.c_str(), this->inputBuf, IM_ARRAYSIZE(this->inputBuf), this->GetSize(), flags);
		if (ImGui::IsItemHovered() == true && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
			this->UpdateCursorPos();
		}
		if (this->text != this->inputBuf || rattlesmake::peripherals::keyboard::get_instance().IsAnyDirectionalButtonNotReleased() ||
			rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_PAGE_UP) || rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_PAGE_DOWN))
			this->UpdateCursorPos();
		this->text = this->inputBuf;
		ImGui::PopItemWidth();
		if (iframeSkin->textInputMultilineData.has_background)
			ImGui::PopStyleColor();
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(6);
		ImGui::PopFont();
	}

	uint32_t ImGuiTextInputMultiline::GetCursorLine(void) const
	{
		return this->line;
	}

	uint32_t ImGuiTextInputMultiline::GetCursorColumn(void) const
	{
		return this->column;
	}

	void ImGuiTextInputMultiline::UpdateCursorPos(void)
	{
		uint32_t ln = 1;
		uint32_t lastNewLine = 1;

		for (uint16_t i = 0; i < ImGui::GetStateCursor(this->label.c_str()); i++)
		{
			char current = this->inputBuf[i];
			if (current == '\n')
			{
				lastNewLine = i;
				ln++;
			}
		}
		this->line = ln;
		this->column = (lastNewLine > 1) ? static_cast<uint32_t>(ImGui::GetStateCursor(this->label.c_str())) - lastNewLine : ImGui::GetStateCursor(this->label.c_str()) + 1;
	}
}
