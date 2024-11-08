/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <tuple>

#include <iframe/imgui_tooltip.h>
#include <iframe/imguiElement.h>

namespace rattlesmake { namespace image { class png; }; };

namespace gui
{
	template <typename F>
	class FunctionWrapper 
	{
	private:
		F fun;
		using R = decltype(fun());
	public:
		explicit FunctionWrapper(F&& f) : fun(std::move(f)) { }
		~FunctionWrapper(void) { }

		R operator()(void)
		{
			return this->fun();
		}
	};

	class ImGuiImageButtonFunctWrapperInterface
	{
	public:
		virtual ~ImGuiImageButtonFunctWrapperInterface(void) { }
		virtual void operator()(void) = 0;
	};

	template <typename F>
	class ImGuiImageButtonFunctWrapper : public ImGuiImageButtonFunctWrapperInterface
	{
	private:
		std::unique_ptr<FunctionWrapper<F>> functWrapperSP;
	public:
		explicit ImGuiImageButtonFunctWrapper(F&& f) : functWrapperSP(new FunctionWrapper<F>(std::move(f))) { }
		~ImGuiImageButtonFunctWrapper(void) { }

		[[nodiscard]] FunctionWrapper<F>* GetFunctionWrapper(void) const
		{
			return (this->functWrapperSP) ? this->functWrapperSP.get() : nullptr;
		}

		void operator()(void)
		{
			if (this->functWrapperSP)
				(*this->functWrapperSP)();
		}
	};

	template <typename F>
	[[nodiscard]] std::unique_ptr<ImGuiImageButtonFunctWrapper<F>> CreateImGuiImageButtonFunctWrapper(F&& f)
	{
		return std::unique_ptr<ImGuiImageButtonFunctWrapper<F>>(new ImGuiImageButtonFunctWrapper<F>(std::move(f)));
	}

	enum class ImGuiImageButtonState : int
	{
		Normal = 0,
		Hovered = 1,
		Pressed = 2,
		Disabled = 3
	};

	class ImGuiImageButton : public ImGuiElement
	{
	public:
		ImGuiImageButton(const ImGuiImageButton& other) = delete;
		ImGuiImageButton& operator=(const ImGuiImageButton& other) = delete;
		virtual ~ImGuiImageButton(void);

		#pragma region Static members:
		[[nodiscard]] static std::shared_ptr<ImGuiImageButton> CreateButton(std::shared_ptr<Iframe> iframeSP, ImGuiElement::elementID_t id, ImVec2 pos, ImVec2 size, std::string text, std::string imageName, std::string onclickScript, const bool executeOnRelease = true);
		[[nodiscard]] static std::shared_ptr<ImGuiImageButton> CreateCloseButton(std::shared_ptr<Iframe> iframeSP, std::optional<ImVec2> pos, std::string imageName);
		[[nodiscard]] static std::shared_ptr<ImGuiImageButton> CreateFromXml(std::shared_ptr<Iframe> iframeSP, tinyxml2::XMLElement* el);
		#pragma endregion

		void Render(void) override;

		void SetImage(std::string imageName);

		void SetPressedValue(const bool bPressed);
		void SetExecutionOnClick(void);
		void SetExecutionOnRelease(void);
		void SetOnclickPyScript(std::string pyScript);
		
		template <typename F>
		void SetFunctionToRun(std::unique_ptr<ImGuiImageButtonFunctWrapper<F>>&& _functToRun)
		{
			this->functToRun = std::move(_functToRun);
		}
		void ResetFunctionToRun(void);

		void SetFunctionToGetTooltipInfo(std::function<ImGuiTooltip::tooltip_t(void)>&& funct);
		void ResetFunctionToGetTooltipInfo(void);
	protected:
		explicit ImGuiImageButton(std::shared_ptr<Iframe>&& iframeSP);
		ImGuiImageButton(std::shared_ptr<Iframe>&& iframeSP, ImGuiElement::elementID_t id, ImVec2&& pos, ImVec2&& size, std::string&& text, std::string&& onclickScript);

		void InitFromXmlElement(tinyxml2::XMLElement* el, const int yoffset = 0) override;
	private:
		void Create(void);
		void UpdateImageButton(void);
		void ManageClick(void);

		uint32_t pickingID = 0;

		bool bHasText = false;
		bool bIsPressed = false;
		bool bExecuteOnRelease = true;

		// Pointer to a GENERIC function to run when button is clicked
		std::unique_ptr<ImGuiImageButtonFunctWrapperInterface> functToRun;

		ImGuiTooltip tooltip;
		
		std::string imageName;
		std::string hoverName;
		std::string pressedName;
		std::string disabledName;
		
		
		std::shared_ptr<rattlesmake::image::png> buttonStates_images[4];
		std::string buttonState_names[4];
		ImGuiImageButtonState currentButtonState = ImGuiImageButtonState::Normal;
		void SetButtonStateName(const ImGuiImageButtonState state, const std::string& name);
		bool SetButtonStateImage(const ImGuiImageButtonState state);
		void SetButtonStates(void);
		bool bSkinReadWithErrors = false;

		//std::weak_ptr<gui::Image> image;
		std::shared_ptr<rattlesmake::image::png> cil_image;

		ImVec2 textPos{ 0.f, 0.f };
		ImVec2 textSize{ 0.f, 0.f };
		ImVec4 textColor{ 0.f, 0.f, 0.f, 0.f };

		static bool bIsThereButtonPressed;
		static uint32_t lastPickingID;
	};
}
