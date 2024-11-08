#include "dialogWindows.h"

#include <encode_utils.h>
#include <icons.h>
#include <engine.h>

#include <iframe/imguiImageButton.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

#include <services/pyservice.h>
#include <services/sqlservice.h>

#include <png.h>
#include <fileservice.h>
#include <mouse.h>

namespace gui
{
	#pragma region Static variables initialization:
	bool ImGuiImageButton::bIsThereButtonPressed = false;
	uint32_t ImGuiImageButton::lastPickingID = 0;
	#pragma endregion


	#pragma region Constructors and destructor:
	ImGuiImageButton::ImGuiImageButton(std::shared_ptr<Iframe>&& iframeSP) : 
		ImGuiElement(ImGuiElement::ClassesTypes::e_imageButton, std::move(iframeSP))
	{
		this->pickingID = ImGuiImageButton::lastPickingID;
		ImGuiImageButton::lastPickingID += 1;
	}

	ImGuiImageButton::ImGuiImageButton(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& text, std::string&& onclickScript) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_imageButton, std::move(iframeSP), id, std::move(pos), std::move(size), std::move(text), std::move(onclickScript)),
		imageName(std::move(imageName))
	{
		this->pickingID = ImGuiImageButton::lastPickingID;
		ImGuiImageButton::lastPickingID += 1;
	}

	ImGuiImageButton::~ImGuiImageButton(void)
	{
	}
	#pragma endregion


	#pragma region Static public members:
	std::shared_ptr<ImGuiImageButton> ImGuiImageButton::CreateButton(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, std::string text, std::string imageName, std::string onclickScript, const bool executeOnRelease)
	{
		std::shared_ptr<ImGuiImageButton> button{ new ImGuiImageButton{ std::move(iframeSP), id, std::move(pos), std::move(size), std::move(text), std::move(onclickScript) } };
		button->SetImage(std::move(imageName));
		// The button will execute its script immediately after clicking (i.e. not need to be released)
		if (!executeOnRelease)
			button->SetExecutionOnClick();
		return button;
	}

	std::shared_ptr<ImGuiImageButton> ImGuiImageButton::CreateCloseButton(std::shared_ptr<Iframe> iframeSP, std::optional<ImVec2> pos, std::string imageName)
	{
		std::shared_ptr<ImGuiImageButton> button{ new ImGuiImageButton{ std::move(iframeSP) } };
		if (pos.has_value() == true)
			button->SetPosition(std::move(pos.value()));
		button->SetImage(std::move(imageName));
		return button;
	}

	std::shared_ptr<ImGuiImageButton> ImGuiImageButton::CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el)
	{
		std::shared_ptr<ImGuiImageButton> button{ new ImGuiImageButton{ std::move(iframeSP) } };
		button->InitFromXmlElement(el);
		return button;
	}
	#pragma endregion


	void ImGuiImageButton::Render(void)
	{
		if (this->bSkinReadWithErrors == true)
			return;
		if (this->bIsHidden == true)
			return;
		if (this->bIsCreated == false)
			this->Create();

		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

		// TODO
		// a che serviva sta roba?
		//if (img->bResetTexParameter == true)
		//	img->ResetTexParameter();

		this->bIsClicked = ImGui::ImageButton(this->buttonStates_images[(size_t)currentButtonState], this->pickingID, this->GetSize(), ImVec2(0, 0), ImVec2(1, 1), ImGuiButtonFlags_Repeat);
		this->bIsHovered = ImGui::IsItemHovered();

		if (this->bIsEnabled == true)
		{
			if (this->bIsClicked == true)
			{
				if (!this->bExecuteOnRelease)
				{
					ImGuiImageButton::bIsThereButtonPressed = false;
					this->ManageClick();
				}
				else
				{
					this->SetPressedValue(true);
				}
			}

			if (this->bIsPressed && this->bExecuteOnRelease && (rattlesmake::peripherals::mouse::get_instance().Release || ImGui::IsMouseReleased(0)))
			{
				this->SetPressedValue(false);
				if (this->bIsHovered == true)
					this->ManageClick();
			}
		}

		if (this->bIsHovered == true)
		{
			if (ImGui::GetCurrentContext()->HoveredIdTimer > 1.f)
			{
				// Try to show a proper tooltip
				ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("IframeTitle")));
				this->tooltip.Render();
				ImGui::PopFont();
			}
		}

		ImGui::PopStyleColor(3);

		if (this->bHasText == true)
		{
			this->textColor = (this->bIsEnabled == true) ? this->iframeWP.lock()->GetSkin()->buttonData.text_color : ImVec4(0.5f, 0.5f, 0.5f, 1.f);
			if (this->icon != "")
			{
				ImVec2 iconSize = ImGuiText::CalculateTextSize(icon, FONT_ICON_FILE_NAME_FA);
				ImGui::SetCursorPos(ImVec2(this->textPos.x - iconSize.y, this->textPos.y));
				ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);
				ImGui::Text(reinterpret_cast<const char*>(GetIconUTF8Reference(this->icon)));
				ImGui::PopStyleColor();
				ImGui::PopFont();
				ImGui::SameLine();
			}
			else
			{
				ImGui::SetCursorPos(this->textPos);
			}
			ImGui::PushFont(ImGui::GetFontByName(fontName));
			ImGui::TextColored(this->textColor, this->text.c_str());
			ImGui::PopFont();
		}

		this->UpdateImageButton();
	}

	void ImGuiImageButton::SetImage(std::string imageName)
	{
		if (this->imageName == imageName)
			return;

		this->imageName = std::move(imageName);
		this->SetButtonStates();
	}

	void ImGuiImageButton::SetPressedValue(const bool bPressed)
	{
		if (bPressed == true)
		{
			if (ImGuiImageButton::bIsThereButtonPressed == false)
			{
				this->bIsPressed = true;
				ImGuiImageButton::bIsThereButtonPressed = true;
			}
		}
		else
		{
			if (ImGuiImageButton::bIsThereButtonPressed == true)
			{
				this->bIsPressed = false;
				ImGuiImageButton::bIsThereButtonPressed = false;
			}
		}
	}

	void ImGuiImageButton::SetExecutionOnClick(void)
	{
		this->bExecuteOnRelease = false;
	}

	void ImGuiImageButton::SetExecutionOnRelease(void)
	{
		this->bExecuteOnRelease = true;
	}

	void ImGuiImageButton::SetOnclickPyScript(std::string pyScript)
	{
		this->onclick = std::move(pyScript);
	}

	void ImGuiImageButton::ResetFunctionToRun(void)
	{
		this->functToRun = std::unique_ptr<ImGuiImageButtonFunctWrapperInterface>();
	}

	void ImGuiImageButton::SetFunctionToGetTooltipInfo(std::function<ImGuiTooltip::tooltip_t(void)>&& tooltipInfoFunct)
	{
		this->tooltip.SetFunctionToGetTooltipInfo(std::move(tooltipInfoFunct));
	}

	void ImGuiImageButton::ResetFunctionToGetTooltipInfo(void)
	{
		this->tooltip.ResetFunctionToGetTooltipInfo();
	}


	#pragma region Protected members:
	void ImGuiImageButton::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->imageName = Encode::FixImageName(TryParseStrAttribute(el, "image_name"));
	}
	#pragma endregion


	#pragma region Private members:
	void ImGuiImageButton::Create(void)
	{
		//ImageService& image_service = ImageService::GetInstance();
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();

		this->fontName = iframeSkin->buttonData.font == "" ? SqlService::GetInstance().GetFontByContext("DefaultButtonText") : iframeSkin->buttonData.font;
		this->bHasText = (this->text.empty() == false);

		this->textColor = this->bIsEnabled ? iframeSkin->buttonData.text_color : ImVec4(0.5f, 0.5f, 0.5f, 1.f);

		this->SetButtonStates();
		
		//todo
		//todo
		//todo
		// Remove this part?

		//if (this->cil_image)
		//{
		//	{
		//		auto _img = this->cil_image;
		//
		//		if (this->icon != "" || (this->bHasText && ImGuiText::CalculateTextSize(text, this->fontName).x >= _img->GetWidth() - 20))
		//		{
		//			this->imageName += "_huge";
		//			this->hoverName += "_huge";
		//			this->pressedName += "_huge";
		//			this->disabledName += "_huge";
		//
		//			this->cil_image.reset();
		//			this->cil_image = rattlesmake::image::png::create(zipFile, this->imageName, rattlesmake::image::png_flags_::png_flags_None, 1.f);
		//
		//			//this->image = image_service.GetImage(this->imageName, true);
		//		}
		//	}
		//	{
		//		auto _img = this->cil_image;
		//
		//		const ImVec2 oldSize = this->GetSize();
		//		const ImVec2 newSize{ 
		//			oldSize.x == 0 ? static_cast<float>(_img->GetWidth()) : oldSize.x,
		//			oldSize.y == 0 ? static_cast<float>(_img->GetHeight()) : oldSize.y
		//		};
		//		this->SetSize(newSize);
		//	}
		//}

		if (this->bHasText == true)
		{
			const ImVec2 size = this->GetSize();
			const ImVec2 pos = this->GetPosition();
			this->textSize = ImGuiText::CalculateTextSize(text, this->fontName);
			this->textPos = ImVec2(pos.x + size.x / 2 - this->textSize.x / 2, pos.y + size.y / 2 - this->textSize.y / 2 - 1);
		}

		this->bIsCreated = true;
	}

	void ImGuiImageButton::UpdateImageButton(void)
	{
		// Whether there is an opened dialog window, keeps the idle image
		if (gui::IsAnyDialogWindowActive() == true)
		{
			this->currentButtonState = ImGuiImageButtonState::Normal;
			return;
		}

		if (this->bIsEnabled == true)
		{
			if (this->bIsPressed == false)
			{
				if (this->bIsHovered == true && ImGuiImageButton::bIsThereButtonPressed == false)
				{
					this->currentButtonState = ImGuiImageButtonState::Hovered;
				}
				else if (this->bIsHovered == false)
				{
					this->currentButtonState = ImGuiImageButtonState::Normal;
				}
			}
			else
			{
				this->currentButtonState = ImGuiImageButtonState::Pressed;
			}
		}
		else 
		{
			this->currentButtonState = ImGuiImageButtonState::Disabled;
		}
	}

	void ImGuiImageButton::ManageClick(void)
	{
		if (gui::IsAnyDialogWindowActive() == true)
			return;

		// Run cpp function associated to the button
		if (this->functToRun)
			(*this->functToRun)();

		// Run python scripton associated to the button
		if (this->onclick.empty() == false)
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->onclick);
	}
	
	void ImGuiImageButton::SetButtonStateName(const ImGuiImageButtonState state, const std::string& name)
	{
		this->buttonState_names[(size_t)state] = name;
	}
	
	bool ImGuiImageButton::SetButtonStateImage(const ImGuiImageButtonState state)
	{
		auto img = rattlesmake::image::png::create(this->buttonState_names[(size_t)state], rattlesmake::image::png_flags_::png_flags_None, 1.f);
		if (img->is_png_read_correctly())
		{
			this->buttonStates_images[(size_t)state] = img;
			if (state == ImGuiImageButtonState::Normal && this->GetSize().x <= 0 && this->GetSize().y <= 0)
			{
				this->SetSize(ImVec2(img->get_width(), img->get_height()));
			}
		}
		else if (state != ImGuiImageButtonState::Normal)
		{
			this->buttonStates_images[(size_t)state] = this->buttonStates_images[(size_t)ImGuiImageButtonState::Normal];
		}
		else
		{
			this->buttonStates_images[(size_t)state] = nullptr;
			return false;
		}
		return true;
	}
	
	void ImGuiImageButton::SetButtonStates(void)
	{
		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		if (this->imageName.empty() == false)
		{
			this->SetButtonStateName(ImGuiImageButtonState::Normal, this->imageName);

			// check that normal image has been read correctly before proceed
			if (this->SetButtonStateImage(ImGuiImageButtonState::Normal))
			{
				this->SetButtonStateName(ImGuiImageButtonState::Hovered, this->imageName + "_hover");
				this->SetButtonStateImage(ImGuiImageButtonState::Hovered);

				this->SetButtonStateName(ImGuiImageButtonState::Pressed, this->imageName + "_pressed");
				this->SetButtonStateImage(ImGuiImageButtonState::Pressed);

				this->SetButtonStateName(ImGuiImageButtonState::Disabled, this->imageName + "_disabled");
				this->SetButtonStateImage(ImGuiImageButtonState::Disabled);
			}
			else
			{
				this->imageName = "";
			}
		}

		// apply skin images
		if (this->imageName.empty() == true)
		{
			this->SetButtonStateName(ImGuiImageButtonState::Normal, (iframeSkin->buttonData.image_name == "") ? "common/buttons/button_1" : iframeSkin->buttonData.image_name);
			if (this->SetButtonStateImage(ImGuiImageButtonState::Normal))
			{
				this->SetButtonStateName(ImGuiImageButtonState::Hovered, (iframeSkin->buttonData.hover_image_name == "") ? "common/buttons/button_1_hover" : iframeSkin->buttonData.hover_image_name);
				this->SetButtonStateImage(ImGuiImageButtonState::Hovered);

				this->SetButtonStateName(ImGuiImageButtonState::Pressed, (iframeSkin->buttonData.pressed_image_name == "") ? "common/buttons/button_1_pressed" : iframeSkin->buttonData.pressed_image_name);
				this->SetButtonStateImage(ImGuiImageButtonState::Pressed);

				this->SetButtonStateName(ImGuiImageButtonState::Disabled, (iframeSkin->buttonData.disabled_image_name == "") ? "common/buttons/button_1_disabled" : iframeSkin->buttonData.disabled_image_name);
				this->SetButtonStateImage(ImGuiImageButtonState::Disabled);
			}
			else
			{
				std::cout << "[DEBUG] Skin read with errors" << std::endl;
				this->bSkinReadWithErrors = true;
			}
		}
	}
	#pragma endregion
}
