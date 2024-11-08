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
#include <set>
#include <string>
#include <unordered_map>
#include <utility>  // includes std::pair
#include <vector>

#include <tinyxml2_utils.h>

#include <environments/ienvironment.h>

// ImGui Iframe elements
#include <iframe/imgui_elements_include.h>

namespace gui
{
	namespace IframeSkinCollection { class IframeSkin; }

	class IframeTab;
	class IframeEditorTree;

	class Iframe
	{
		friend class IframeTab;

	public:
		[[nodiscard]] static std::shared_ptr<Iframe> CreateIframe(IEnvironment::Environments env, tinyxml2::XMLElement* el);
		[[nodiscard]] static Iframe* GetCurrentIframe(void);

		Iframe(const Iframe& other) = delete;
		Iframe& operator=(const Iframe& other) = delete;
		virtual ~Iframe(void);

		virtual void Render(void);

		virtual ImVec2 GetAbsolutePosition(void) const;

#pragma region To scripts members:
		/*
		 * A slightly slower but stylistically clearer alternative to GetElementByTagAndId
		 */
		[[nodiscard]] std::shared_ptr<ImGuiElement> operator[](const std::string& elementIdentificator) const;

		[[nodiscard]] bool CheckIfElementExistsByTagAndId_Py(const std::string& tag, const ImGuiElement::elementID_t id) const;
		void Close(void);
		[[nodiscard]] ImGuiElement::elementID_t GetCurrentElementId(void) const;
		[[nodiscard]] std::shared_ptr<ImGuiElement> GetElementByTagAndId_Py(const std::string& tag, const ImGuiElement::elementID_t id);
		[[nodiscard]] std::string GetId(void) const;
		[[nodiscard]] std::pair<float, float> GetPosition_Py(void) const;
		[[nodiscard]] std::pair<float, float> GetSize_Py(void) const;
		[[nodiscard]] std::shared_ptr<Iframe> GetTabByIndex(const uint8_t index) const;
		[[nodiscard]] bool IsOpened(void) const;
		void Open(void);
		void SetPosition_Py(std::pair<float, float> pos);
		void SetSize_Py(std::pair<float, float> value);
#pragma endregion


		void ClearElementsByTag(const ImGuiElement::ClassesTypes classType);
		bool ClearElementByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id);
		[[nodiscard]] bool CheckIfElementExistsByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id) const;
		[[nodiscard]] std::shared_ptr<ImGuiElement> GetElementByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id) const;


		[[nodiscard]] IEnvironment::Environments GetEnv(void) const;
		[[nodiscard]] ImVec2 GetPosition(void) const;
		void SetPosition(ImVec2 pos);
		[[nodiscard]] ImVec2 GetSize(void) const;
		void SetSize(ImVec2 size);

		[[nodiscard]] bool IsFocused(void) const;
		[[nodiscard]] bool IsEmpty(void) const;

		[[nodiscard]] bool HasFlag(const std::string& flag) const;
		void SetFlag(const std::string& key, const uint64_t value);
		[[nodiscard]] std::optional<uint64_t> GetFlag(const std::string& key) const;
		void ClearFlag(const std::string& key);

		void SetOpeningFunction(std::function<void(bool& empty)> _openingFunction);
		void SetClosingFunction(std::function<void(bool& empty)> _closingFunction);

		void SetSkin(std::shared_ptr<IframeSkinCollection::IframeSkin> _skin);
		[[nodiscard]] std::shared_ptr<IframeSkinCollection::IframeSkin> GetSkin(void) const;
		[[nodiscard]] std::string GetSkinName(void) const;

		void CloseAllTabs(IframeTab* exceptThis = nullptr);

		void AddElement(std::shared_ptr<ImGuiElement> el);
		std::shared_ptr<ImGuiImageButton> AddButton(const float xPos, const float yPos, const float width, const float height, std::string text, std::string imageName, std::string onclickScript, const bool executeOnRelease = true);
		std::shared_ptr<ImGuiImage> AddImage(const float xPos, const float yPos, const float width, const float height, std::string imageName, std::string tooltip = "");
		std::shared_ptr<ImGuiProgressBar> AddProgressBar(const float xPos, const float yPos, const float width, const float height, const float progress, const bool inverted = false);
		std::shared_ptr<ImGuiText> AddText(const float xPos, const float yPos, std::string text);

		void AddEditorTree(const std::vector<std::pair<std::string, std::string>>& source, std::function<void(std::string par1, unsigned int par2)> onclick);
	protected:
		explicit Iframe(IEnvironment::Environments _env);

		void InitTabs(void);
		void RenderElements(const gui::Iframe* iframe);

		[[nodiscard]] IframeTab* AsTab(void);
		[[nodiscard]] IframeEditorTree* AsEditorTree(void);

		ImVec2 position{ 0.f, 0.f };
		ImVec2 size{ 0.f, 0.f };

		std::string icon;
		std::string title;
		std::string openingFun;
		std::string closingFun;
		std::string runtimeFun;

		typedef std::unordered_map<ImGuiElement::elementID_t, std::shared_ptr<ImGuiElement>> elementsByIdMap_t;
		typedef struct classTypeInfo_s
		{
			explicit classTypeInfo_s(elementsByIdMap_t&& elementsMap);
			classTypeInfo_s(const classTypeInfo_s& other) = delete;
			classTypeInfo_s& operator=(const classTypeInfo_s& other) = delete;
			classTypeInfo_s(classTypeInfo_s&& other) noexcept;

			elementsByIdMap_t elements;
			std::set<ImGuiElement::elementID_t> availableIDs;

		}	classTypeInfo_t;
		typedef std::unordered_map<ImGuiElement::ClassesTypes, classTypeInfo_t> elementsByTypeMap_t;
		elementsByTypeMap_t elementsMap; // main container (by class and by id)
		std::list<std::weak_ptr<ImGuiElement>> elementsList; // render list
		size_t elementsNumber = 0;

		bool bIsOpenedDefault_xml = false;
		bool bCanBeMoved_xml = true;
		bool bCanBeClosed_xml = true;
		bool bIsEmpty_xml = false;
		bool bInteractable_xml = true;
		bool bTabsInitialized = false;
		bool bHasTabs = false;
		bool bIsOpened = false;
		bool bIsClosed = true;
		bool bIsTab = false;
		bool bIsEditorTree = false;
		bool bIsFocused = false;  // for internal use

		std::shared_ptr<ImGuiElement> editorTree;
		std::weak_ptr<ImGuiElement> currentElement;
		std::string skinName;
		std::shared_ptr<IframeSkinCollection::IframeSkin> skin;
		IEnvironment::Environments env = IEnvironment::Environments::e_unknown;

		std::function<void(bool& empty)> openingFunction;
		std::function<void(bool& empty)> closingFunction;
		std::shared_ptr<ImGuiImageButton> closeButton;

		std::string id;
		std::vector<std::shared_ptr<Iframe>> listOfTabs;
		std::unordered_map<std::string, uint64_t> intFlags;

		static Iframe* currentIframe;
	private:
		void CreateFromXmlElement(tinyxml2::XMLElement* el);
		void ReadImGuiElementsFromXmlArray(tinyxml2::XMLElement* el, const std::string& xmlArrayTag);
		[[nodiscard]] ImGuiElement::elementID_t GetAvailableIdForType(const ImGuiElement::ClassesTypes type) const;

		std::weak_ptr<Iframe> me;
	};

	class IframeEmpty : public Iframe
	{
		friend class Iframe;
	public:
		void Render() override;

	protected:
		explicit IframeEmpty(IEnvironment::Environments _env);
	};

	class IframeEditorTree : public Iframe
	{
		friend class Iframe;
	public:
		void Render() override;
	protected:
		explicit IframeEditorTree(IEnvironment::Environments _env);
	};

	class IframeTab : public Iframe
	{
		friend class Iframe;
	public:
		void Render() override;
		bool closeOtherTabs = false;
		Iframe* iframe = nullptr;

		//This function allows the game to read the iframe position from an element, even is inside a tab
		virtual ImVec2 GetAbsolutePosition(void) const override;

	protected:
		IframeTab(IEnvironment::Environments env, ImVec2 size, ImVec2 position);
	};
};