#include <iframe/imguiImage.h>

#include <iframe/iframe.h>

#include <services/sqlservice.h>
#include <fileservice.h>

#include <encode_utils.h>
#include <engine.h>

#include <viewport.h>
#include <mouse.h>

#include <png.h>

namespace gui
{
	#pragma region Constructors and destructor:
	ImGuiImage::ImGuiImage(std::shared_ptr<Iframe>&& iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_image, std::move(iframeSP))
	{
	}

	ImGuiImage::ImGuiImage(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& imageName, std::string&& tooltip) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_image, std::move(iframeSP), id, std::move(pos), std::move(size)),
		imageName(std::move(imageName)), tooltip(std::move(tooltip))
	{
	}

	ImGuiImage::~ImGuiImage(void)
	{
	}
	#pragma endregion

	#pragma region Static members:
	std::shared_ptr<ImGuiImage> ImGuiImage::CreateImage(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, std::string imageName, std::string tooltip)
	{
		return std::shared_ptr<ImGuiImage>(new ImGuiImage{ std::move(iframeSP), id, std::move(pos), std::move(size), std::move(imageName), std::move(tooltip) });
	}

	std::shared_ptr<ImGuiImage> ImGuiImage::CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el)
	{
		std::shared_ptr<ImGuiImage> image{ new ImGuiImage{std::move(iframeSP) } };
		image->InitFromXmlElement(el);
		return image;
	}
	#pragma endregion

	void ImGuiImage::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->imageName = Encode::FixImageName(TryParseStrAttribute(el, "image_name"));
		this->tooltip = TryParseStrAttribute(el, "tooltip");
	}

	void ImGuiImage::Render(void)
	{
		if (this->bIsHidden == true)
			return;
		if (this->bIsCreated == false)
			this->Create();

		const ImVec2 pos = this->GetPosition(); //Offset from iframe, got from XML
		const ImVec2 size = this->GetSize();

		ImVec2 iframePos = this->iframeWP.lock()->GetAbsolutePosition();
		if (this->bIsBackground == false)
		{
			xPos = static_cast<int>(pos.x + iframePos.x);
			yPos = static_cast<int>(pos.y + iframePos.y - size.y);
			ImGui::cil_RenderImage(this->cil_image, ImGui::GetWindowDrawList(), xPos, yPos, this->Flags, static_cast<int>(size.x), static_cast<int>(size.y));
		}
		else
		{
			xPos = static_cast<int>(pos.x + iframePos.x);
			yPos = static_cast<int>(rattlesmake::peripherals::viewport::get_instance().GetHeight()) - static_cast<int>(iframePos.y) - static_cast<int>(pos.y);
			this->cil_image->render(xPos, yPos, this->Flags, static_cast<int>(size.x), static_cast<int>(size.y));
		}

		if (this->IsHovered(xPos, yPos, ImVec2(size.x, size.y)) == true && this->tooltip != "")
		{
			this->hoveringTime++;
			if (static_cast<float>(hoveringTime) / 60.f > 1.f)
			{
				ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("IframeTitle")));
				ImGui::SetTooltip(this->tooltip.c_str());
				ImGui::PopFont();
			}
		}
		else
			this->hoveringTime = 0;
	}

	bool ImGuiImage::IsHovered(const int16_t x, const int16_t y, const ImVec2& size) const
	{
		rattlesmake::peripherals::mouse& mouse_instance = rattlesmake::peripherals::mouse::get_instance();
		const double mouse_x_pos = mouse_instance.GetXPosition();
		const double mouse_y_pos = mouse_instance.GetYPosition();
		return (mouse_x_pos >= x && mouse_x_pos <= x + static_cast<double>(size.x) && mouse_y_pos >= y && mouse_y_pos <= y + static_cast<double>(size.y));
	}

	void ImGuiImage::SetImage(std::string image)
	{
		if (this->imageName != image)
		{
			this->imageName = std::move(image);
			this->bIsCreated = false;
		}
	}

	void ImGuiImage::Create(void)
	{
		this->cil_image = rattlesmake::image::png::create(this->imageName, rattlesmake::image::png_flags_::png_flags_None, 1.f);
		this->Flags |= rattlesmake::image::png::get_image_flag(this->type);

		if (this->type == "background")
		{
			this->SetSize(this->iframeWP.lock()->GetSize());
		}
		
		// check on size
		{
			const ImVec2 size = this->GetSize();
			if (size.x <= 0)
				this->SetSizeX(RIL_IMAGE_DEFAULT_VAL);
			if (size.y <= 0)
				this->SetSizeY(RIL_IMAGE_DEFAULT_VAL);
		}

		this->bIsCreated = true;
		this->type.clear();
	}
}
