#include <iframe/imguiEditorTree.h>

#include <iframe/iframe.h>

#include <players/players_array.h>
#include <services/sqlservice.h>
#include <encode_utils.h>

namespace gui
{
	ImGuiEditorTree::ImGuiEditorTree(std::shared_ptr<Iframe> iframeSP) :
		ImGuiElement(ImGuiElement::ClassesTypes::e_editorTree, std::move(iframeSP))
	{
	}

	ImGuiEditorTree::EditorTreeItem::EditorTreeItem(ImGuiEditorTree* treePtr)
	{
		this->tree_ptr = treePtr;
	}

	bool ImGuiEditorTree::EditorTreeItem::CheckIfContains(std::string input)
	{
		if (this->bIsLeaf)
		{
			std::string _displayedText = this->displayedText;
			std::string _value = this->value;
			Encode::ToLowercase(&_displayedText);
			Encode::ToLowercase(&_value);
			return (_displayedText.find(input) != std::string::npos) || (_value.find(input) != std::string::npos);
		}
		else
		{
			for (auto const& i : this->children)
				if (i->CheckIfContains(input))
					return true;
			return false;
		}
	}

	void ImGuiEditorTree::EditorTreeItem::Close()
	{
		if (this->bIsLeaf)
			return;
		this->bClicked = false;
		for (auto const& i : this->children)
			i->Close();
	}

	void ImGuiEditorTree::EditorTreeItem::ClearSets()
	{
		for (size_t i = 0; i < this->children.size(); i++)
		{
			if (this->children[i].get() != nullptr)
				this->children[i]->ClearSets();
		}
		this->childrenSet.clear();
	}

	void ImGuiEditorTree::AddItemFromString(std::string path, std::string leaf)
	{
		auto _items = std::split_string(path, '/');
		_items.push_back(leaf);

		std::size_t n = 0;
		std::size_t N = _items.size();
		std::vector<std::shared_ptr<EditorTreeItem>> pointers = std::vector<std::shared_ptr<EditorTreeItem>>(N, nullptr);
		for (auto const& it : _items)
		{
			if (n == 0)
			{
				if (this->itemsSet.contains(it))
				{
					pointers[n] = this->itemsSet[it];
				}
				else
				{
					pointers[n] = std::shared_ptr<EditorTreeItem>(new EditorTreeItem(this));
					std::string strToBeTranslated = "etree_" + it;
					pointers[n]->displayedText = SqlService::GetInstance().GetTranslation(strToBeTranslated, false);
					pointers[n]->value = it;
					pointers[n]->bIsHeader = true;
					pointers[n]->deepness = (int)n;
					this->itemsSet[it] = pointers[n];
					this->items.push_back(pointers[n]);
				}
			}
			else
			{
				if (pointers[n - 1]->childrenSet.contains(it))
				{
					pointers[n] = pointers[n - 1]->childrenSet[it];
				}
				else
				{
					pointers[n] = std::shared_ptr<EditorTreeItem>(new EditorTreeItem(this));
					pointers[n]->bIsLeaf = (n == N - 1);
					std::string strToBeTranslated = pointers[n]->bIsLeaf ? "w_" + it : "etree_" + it;
					pointers[n]->displayedText = SqlService::GetInstance().GetTranslation(strToBeTranslated, false);
					pointers[n]->value = it;
					pointers[n]->deepness = static_cast<int>(n);
					pointers[n - 1]->childrenSet[it] = pointers[n];
					pointers[n - 1]->children.push_back(pointers[n]);
				}
			}
			n++;
		}
	}

	void ImGuiEditorTree::SetOnclickFunction(std::function<void(std::string par1, unsigned int par2)> onclick)
	{
		this->onclickFunction = onclick;
	}

	ImGuiEditorTree::~ImGuiEditorTree(void)
	{
	}

	void ImGuiEditorTree::Render(void)
	{
		if (this->bIsHidden == true)
			return;

		std::shared_ptr<IframeSkinCollection::IframeSkin> iframeSkin = this->iframeWP.lock()->GetSkin();

		// render the search box
		this->searchBox.Render(iframeSkin);

		// render the player id input
		this->playerID.Render(iframeSkin);

		// render the tree
		const size_t nItems = this->items.size();
		for (size_t i = 0; i < nItems; i++)
			this->items[i]->Render(iframeSkin);

		// if you click on a leaf, start inserting an object ! 
		if (this->bStartInsertingObject)
			if (this->onclickFunction)
				this->onclickFunction(this->selectedItem->value, this->playerID.playerID);
	}

	void ImGuiEditorTree::EditorTreeSearchbox::Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr)
	{
		assert(skinptr);
		auto leftPadding = skinptr->GetLeftWidth();
		ImGui::SetCursorPosX(7 + leftPadding);

		std::string oldText = inputBuf;
		if (skinptr->textInputData.has_background)
			ImGui::PushStyleColor(ImGuiCol_FrameBg, skinptr->textInputData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Text, skinptr->textInputData.text_color);
		ImGui::PushStyleColor(ImGuiCol_Border, skinptr->textInputData.border_color);
		ImGui::PushFont(ImGui::GetFontByName(skinptr->inputIntData.font));
		ImGui::Text(this->label.c_str());
		ImGui::SameLine(0.0f, 10.0f);
		ImGui::InputText("##EditorTreeSearchBox", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::PopFont();
		ImGui::PopStyleColor(2);
		if (skinptr->textInputData.has_background)
			ImGui::PopStyleColor();
		text = inputBuf;
		this->bChanged = oldText != text;
		this->bActive = text.empty() == false;
	}

	void ImGuiEditorTree::EditorTreePlayerId::Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr)
	{
		assert(skinptr);
		auto leftPadding = skinptr->GetLeftWidth();
		ImGui::SetCursorPosX(7 + leftPadding);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, skinptr->inputIntData.background_color);
		ImGui::PushStyleColor(ImGuiCol_Button, skinptr->inputIntData.button_background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, skinptr->inputIntData.button_selected_background_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, skinptr->inputIntData.button_hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_Text, skinptr->inputIntData.text_color);
		ImGui::PushFont(ImGui::GetFontByName(skinptr->inputIntData.font));
		ImGui::Text(this->label.c_str());
		ImGui::SameLine(0.f, 10.f);
		ImGui::InputInt("##EditorTreePlayerInput", &playerID);
		ImGui::PopFont();
		ImGui::PopStyleColor(5);

		if (this->maxPlayerID != PlayersArray::NUMBER_OF_PLAYERS)
			this->maxPlayerID = PlayersArray::NUMBER_OF_PLAYERS;

		if (this->playerID < 1) this->playerID = 1;
		if (this->playerID > this->maxPlayerID) this->playerID = this->maxPlayerID;
	}

	void ImGuiEditorTree::EditorTreeItem::Render(const std::shared_ptr<IframeSkinCollection::IframeSkin>& skinptr)
	{
		assert(skinptr);

		auto& searchBox = this->tree_ptr->searchBox;

		if (searchBox.bActive)
		{
			if (searchBox.bChanged)
			{
				std::string filter{ searchBox.text };
				Encode::ToLowercase(&filter);
				this->bShown = this->CheckIfContains(filter);
			}
		}
		else
		{
			if (searchBox.bChanged && searchBox.text.empty())
				this->Close();

			this->bShown = true;
		}

		if (this->bShown == false)
			return;

		if (this->bIsLeaf == false)
		{
			if (searchBox.bActive && searchBox.bChanged)
				ImGui::SetNextItemOpen(this->bShown);
			else
				ImGui::SetNextItemOpen(this->bClicked);

			ImGui::PushStyleColor(ImGuiCol_Text, skinptr->treeData.text_color);
			ImGui::PushStyleColor(ImGuiCol_Header, skinptr->treeData.node_background_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, skinptr->treeData.node_hover_background_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, skinptr->treeData.node_pressed_background_color);
			ImGui::PushFont(ImGui::GetFontByName(skinptr->treeData.font_big));
			this->bClicked = this->bIsHeader ? ImGui::CollapsingHeader(this->displayedText.c_str()) : ImGui::TreeNode(this->displayedText.c_str());
			ImGui::PopFont();
			ImGui::PopStyleColor(4);
			if (this->bClicked)
			{
				for (size_t i = 0; i < this->children.size(); i++)
				{
					this->children[i]->Render(skinptr);
				}

				if (this->bIsHeader == false && this->bClicked) ImGui::TreePop();
			}
		}
		else
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (this->bSelected)
				node_flags |= ImGuiTreeNodeFlags_Selected;
			ImGui::PushStyleColor(ImGuiCol_Text, skinptr->treeData.text_color);
			ImGui::PushStyleColor(ImGuiCol_Header, skinptr->treeData.node_background_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, skinptr->treeData.node_hover_background_color);
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, skinptr->treeData.node_pressed_background_color);
			ImGui::PushFont(ImGui::GetFontByName(skinptr->treeData.font));
			ImGui::TreeNodeEx((void*)(intptr_t)this->deepness, node_flags, this->displayedText.c_str(), this->deepness);
			ImGui::PopFont();
			ImGui::PopStyleColor(4);
			this->bClicked = ImGui::IsItemClicked();
			if (bClicked)
			{
				auto selectedItem = this->tree_ptr->selectedItem;
				if (selectedItem != this)
				{
					if (selectedItem != nullptr)
						this->tree_ptr->selectedItem->bSelected = false;
					this->tree_ptr->selectedItem = this;
				}
				this->bSelected = true;
				this->tree_ptr->bStartInsertingObject = true;
			}
		}
	}

	void ImGuiEditorTree::EditorTreeItem::Sort(void)
	{
		std::sort(this->children.begin(), this->children.end(), [](std::shared_ptr<EditorTreeItem>& left, std::shared_ptr<EditorTreeItem>& right) {
			return (left->displayedText < right->displayedText);
		});
		for (auto& i : this->children)
		{
			i->Sort();
		}
	}

	void ImGuiEditorTree::ClearSets(void)
	{
		for (size_t i = 0; i < this->items.size(); i++)
		{
			if (this->items[i] != nullptr)
				this->items[i]->ClearSets();
		}
		this->itemsSet.clear();
		this->bCreated = true;
	}

	void ImGuiEditorTree::Sort(void)
	{
		std::sort(this->items.begin(), this->items.end(), [](std::shared_ptr<EditorTreeItem>& left, std::shared_ptr<EditorTreeItem>& right) {
			return (left->displayedText < right->displayedText);
		});
		for (auto const& i : this->items)
		{
			i->Sort();
		}
	}

	void ImGuiEditorTree::SetTranslations(void)
	{
		std::list<dbWord_t> translationsToGet{ "e_text_add_object", "e_text_search", "w_player" };
		auto translations = SqlService::GetInstance().GetTranslations(std::move(translationsToGet), false);
		this->title = translations.at("e_text_add_object");
		this->searchBox.label = translations.at("e_text_search");
		this->playerID.label = translations.at("w_player") + ": ";

		// if strings are empty set default values
		if (this->title.empty())
			this->title = "Add object";
		if (this->searchBox.label.empty())
			this->searchBox.label = "Search: ";
		if (this->playerID.label.empty())
			this->playerID.label = "Player: ";
	}

	void ImGuiEditorTree::Show(void)
	{
		this->bOpened = true;
	}

	void ImGuiEditorTree::Hide(void)
	{
		this->bOpened = false;
	}
}
