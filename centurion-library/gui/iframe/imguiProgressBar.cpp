#include <iframe/imguiProgressBar.h>

#include <iframe/iframe.h>
#include <iframeSkinCollection.h>

namespace gui
{
	#pragma region Constructors and destructor:
	ImGuiProgressBar::ImGuiProgressBar(std::shared_ptr<Iframe>&& iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_progressBar, std::move(iframeSP))
	{
	}

	ImGuiProgressBar::ImGuiProgressBar(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, const float progress, bool bInvertedColor) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_progressBar, std::move(iframeSP), id, std::move(pos), std::move(size)),
		invertedColors(bInvertedColor)
	{
		this->SetProgress(progress);
	}

	ImGuiProgressBar::~ImGuiProgressBar(void)
	{
	}
	#pragma endregion


	#pragma region Static members:
	std::shared_ptr<ImGuiProgressBar> ImGuiProgressBar::CreateProgressBar(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, const float progress, bool bInvertedColors)
	{
		return std::shared_ptr<ImGuiProgressBar>(new ImGuiProgressBar{ std::move(iframeSP), id, std::move(pos), std::move(size), progress, bInvertedColors });
	}

	std::shared_ptr<ImGuiProgressBar> ImGuiProgressBar::CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el)
	{
		std::shared_ptr<ImGuiProgressBar> progressBar{ new ImGuiProgressBar{std::move(iframeSP) } };
		progressBar->InitFromXmlElement(el);
		return progressBar;
	}
	#pragma endregion


	void ImGuiProgressBar::InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset)
	{
		// Call parent's method
		this->ImGuiElement::InitFromXmlElement(el, yoffset);

		this->invertedColors = TryParseBoolAttribute(el, "invertedColors");
	}

	void ImGuiProgressBar::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();
		ImGui::SetCursorPos(this->GetPosition());
		ImGui::PushStyleColor(ImGuiCol_Text, iframeSkin->progressBarData.text_color);
		if (this->invertedColors == false)
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->progressBarData.background_color);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, iframeSkin->progressBarData.bar_color);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_FrameBg, iframeSkin->progressBarData.bar_color);
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, iframeSkin->progressBarData.background_color);
		}
		ImGui::PushFont(ImGui::GetFontByName(iframeSkin->sliderData.font));
		ImGui::ProgressBar(this->progress, this->GetSize(), this->text.c_str());
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
	}

	#pragma region To scripts members:
	bool ImGuiProgressBar::GetInvertedColors(void) const
	{
		return this->invertedColors;
	}

	void ImGuiProgressBar::SetInvertedColors(const bool bInverted)
	{
		this->invertedColors = bInverted;
	}

	float ImGuiProgressBar::GetProgress(void) const
	{
		return this->progress;
	}

	void ImGuiProgressBar::SetProgress(float progress)
	{
		if (progress > 1)
			progress = progress / 100.0f;
		if (progress < 0)
			progress = 0;
		this->progress = progress;
	}
	#pragma endregion
}
