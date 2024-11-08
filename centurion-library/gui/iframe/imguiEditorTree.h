/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <functional>
#include <iframe/imguiElement.h>
#include <iframeSkinCollection.h>

namespace gui
{
	class ImGuiEditorTree : public ImGuiElement
	{
	public:
		explicit ImGuiEditorTree(std::shared_ptr<Iframe> iframeSP);
		ImGuiEditorTree(const ImGuiEditorTree& other) = delete;
		ImGuiEditorTree& operator=(const ImGuiEditorTree& other) = delete;
		~ImGuiEditorTree(void);

		void Render(void) override;

		void AddItemFromString(std::string path, std::string leaf);
		void SetOnclickFunction(std::function<void(std::string par1, unsigned int par2)> onclick);
		void ClearSets();
		void Sort();
		void SetTranslations();
		void Show();
		void Hide();

		class EditorTreePlayerId
		{
		public:
			EditorTreePlayerId(void) {}
			EditorTreePlayerId(const EditorTreePlayerId& other) = delete;
			EditorTreePlayerId& operator=(const EditorTreePlayerId& other) = delete;

			void Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr);
			int playerID = 1; // default value = 1
			int maxPlayerID = 1;
			std::string label;
		};

		class EditorTreeSearchbox
		{
		public:
			EditorTreeSearchbox(void) {}
			EditorTreeSearchbox(const EditorTreeSearchbox& other) = delete;
			EditorTreeSearchbox& operator=(const EditorTreeSearchbox& other) = delete;

			void Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr);
			std::string text; // use this field to take the text inserted in search box
			bool bChanged = false; // true when you write/delete almost one character
			bool bActive = false; // true when the text is empty
			std::string label;
		private:
			char inputBuf[256] = { '\0' };
		};

		class EditorTreeItem
		{
		public:
			EditorTreeItem(void) {}
			EditorTreeItem(ImGuiEditorTree* treePtr);
			EditorTreeItem(const EditorTreeItem& other) = delete;
			EditorTreeItem& operator=(const EditorTreeItem& other) = delete;

			bool CheckIfContains(std::string input);
			void Close(); // close this item and all the children
			void ClearSets();
			void Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr);
			void Sort();
			bool bClicked = false;  // ~ clicked leaf or item (true only during click)
			bool bSelected = false; // ~ selected leaf (true after click)
			bool bIsLeaf = false;
			bool bIsHeader = false;
			bool bShown = true; // useful for search (we want to filter)
			int deepness = 0;
			std::string displayedText; // the text translated and displayed on the screen
			std::string value; // the className or the stringID to be translated

			std::vector<std::shared_ptr<EditorTreeItem>> children;
			std::unordered_map<std::string, std::shared_ptr<EditorTreeItem>> childrenSet; // useful during creation
			ImGuiEditorTree* tree_ptr = nullptr;
		};

		// objects
		EditorTreeItem* selectedItem = nullptr;
		EditorTreeSearchbox searchBox;
		EditorTreePlayerId playerID;
		std::vector<std::shared_ptr<EditorTreeItem>> items;

		std::unordered_map<std::string, std::shared_ptr<EditorTreeItem>> itemsSet; // useful during creation
		bool bCreated = false;
		bool bOpened = false;
		bool bStartInsertingObject = false;
		std::string title;
		std::function<void(std::string par1, unsigned int par2)> onclickFunction;
	};
}
