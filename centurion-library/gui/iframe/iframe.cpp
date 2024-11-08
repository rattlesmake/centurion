#include "iframe.h"

#include <climits>
#include <stdexcept>

#include <engine.h>
#include <icons.h>
#include <imgui_internal.h>
#include <services/sqlservice.h>
#include <services/pyservice.h>

#include <iframeSkinCollection.h>
#include <iframe/imgui_elements_include.h>

#include <png_shader.h>

#include <viewport.h>
#include <keyboard.h>

#include <GLFW/glfw3.h>

using namespace tinyxml2;

namespace gui
{
#if CENTURION_DEBUG_MODE
	namespace
	{
		uint16_t nItems = 0;
		uint16_t emptyCounter = 0;
		uint16_t scriptCounter = 0;
		IEnvironment::Environments lastEnvironment{ IEnvironment::Environments::e_unknown };
		uint32_t COUNTER_ELEMENTS_CREATIONS = 0;
		uint32_t COUNTER_ELEMENTS_CREATIONS_MAX = 0;
		uint32_t COUNTER_IFRAMES_CREATIONS = 0;
		uint32_t COUNTER_IFRAMES_CREATIONS_MAX = 0;

		void ResetCounters(void)
		{
			if (lastEnvironment != Engine::GetInstance().GetEnvironmentId())
			{
				scriptCounter = 0;
				nItems = 0;
				COUNTER_ELEMENTS_CREATIONS_MAX = 0;
				COUNTER_IFRAMES_CREATIONS_MAX = 1;
				lastEnvironment = Engine::GetInstance().GetEnvironmentId();
			}
		}
	};
#endif


#pragma region Constructor and destructor:
	Iframe::Iframe(IEnvironment::Environments _env) : env(_env)
	{
		this->bHasTabs = false;
		this->bIsTab = false;
		this->bIsEmpty_xml = false;
#if CENTURION_DEBUG_MODE
		COUNTER_IFRAMES_CREATIONS++;
		COUNTER_IFRAMES_CREATIONS_MAX++;
#endif
	}

	Iframe::~Iframe(void)
	{
#if CENTURION_DEBUG_MODE
		COUNTER_IFRAMES_CREATIONS--;
		if (COUNTER_IFRAMES_CREATIONS == 0)
			std::cout << "[DEBUG] All " << COUNTER_IFRAMES_CREATIONS_MAX << " instances of iframes has been correctly cleaned\n";
#endif
	}
#pragma endregion

#pragma region Static members:
	std::shared_ptr<Iframe> Iframe::CreateIframe(IEnvironment::Environments env, tinyxml2::XMLElement* el)
	{
		std::shared_ptr<Iframe> iframe;
		bool empty = TryParseBoolAttribute(el, "isEmpty");
		if (empty == false)
		{
			bool editorTree = TryParseBoolAttribute(el, "isEditorTree");
			if (editorTree == false)
			{
				// standard iframe
				iframe = std::shared_ptr<Iframe>(new Iframe(env));
			}
			else
			{
				// iframe containing editor tree
				iframe = std::shared_ptr<Iframe>(new IframeEditorTree(IEnvironment::Environments::e_editor));
			}
		}
		else
		{
			iframe = std::shared_ptr<Iframe>(new IframeEmpty(env));
		}

		// Set weak reference to itself
		assert(iframe);
		iframe->me = iframe;

		iframe->CreateFromXmlElement(el);

		return iframe;
	}

	Iframe* Iframe::currentIframe;
	Iframe* Iframe::GetCurrentIframe(void)
	{
		return Iframe::currentIframe;
	}
#pragma endregion


#pragma region Virtual members:
	void Iframe::Render(void)
	{
		if (this->bIsOpenedDefault_xml)
			this->Open();
		if (this->bIsOpened == false && this->bIsClosed == false)
			this->Close();
		if (this->bTabsInitialized == false)
			this->InitTabs();

		if (this->bIsOpened)
		{
			bool HasSkin = this->skin != nullptr;
			float LeftSkinPadding = (HasSkin) ? this->skin->GetLeftWidth() : 0.f;
			float RightSkinPadding = (HasSkin) ? this->skin->GetRightWidth() : 0.f;
			float TopSkinPadding = (HasSkin) ? this->skin->GetTopHeight() : 0.f;
			float BottomSkinPadding = (HasSkin) ? this->skin->GetBottomHeight() : 0.f;

			PyInterpreter& py_interpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
			py_interpreter.Bind("__iframe__", PyInterpreter::PyBindedTypes::Iframe, this);
			py_interpreter.Evaluate(this->runtimeFun);

			Iframe::currentIframe = this;

			ImGui::SetNextWindowSize(this->size, ImGuiCond_Appearing);
			ImGui::SetNextWindowPos(this->position, ImGuiCond_Appearing);

			// BEGIN PART
			if (HasSkin)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

				ImGui::PushStyleColor(ImGuiCol_Text, this->skin->titleData.text_color);
				ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0, 0, 0, 0));

				auto ImGuiWinFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HideTitleBar;
				if (this->bCanBeMoved_xml == false)
					ImGuiWinFlag |= ImGuiWindowFlags_NoMove;
				if (this->bInteractable_xml == false)
					ImGuiWinFlag |= ImGuiWindowFlags_NoInputs;
				this->bCanBeClosed_xml ? ImGui::Begin(("##" + this->title).c_str(), 0, ImGuiWinFlag) : ImGui::Begin(("##" + this->title).c_str(), nullptr, ImGuiWinFlag);

				ImVec2 clippingTopLeft{ this->position.x - LeftSkinPadding, this->position.y - TopSkinPadding };
				ImVec2 clippingBottomRight{ this->position.x + this->size.x + RightSkinPadding, this->position.y + this->size.y + BottomSkinPadding };
				ImGui::GetWindowDrawList()->PushClipRect(std::move(clippingTopLeft), std::move(clippingBottomRight), false);
			}
			else
			{
				auto ImGuiWinFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HideTitleBar;
				if (this->bCanBeMoved_xml == false)
					ImGuiWinFlag |= ImGuiWindowFlags_NoMove;
				if (this->bInteractable_xml == false)
					ImGuiWinFlag |= ImGuiWindowFlags_NoInputs;
				this->bCanBeClosed_xml ? ImGui::Begin(this->title.c_str(), &this->bIsOpened, ImGuiWinFlag) : ImGui::Begin(this->title.c_str(), nullptr, ImGuiWinFlag);
			}

			this->position = ImGui::GetWindowPos();
			this->size = ImGui::GetWindowSize();
			this->bIsFocused = ImGui::IsWindowFocused();

			// SKIN 
			if (HasSkin)
			{
				this->skin->Render(ImGui::GetWindowDrawList(), this->position, this->size);
			}

			// TITLE
			if (this->icon == "")
				this->icon = "ICON_APP";

			ImGui::SetCursorPos(ImVec2(7, 3));
			ImGui::PushFont(ImGui::GetFontByName(FONT_ICON_FILE_NAME_FA));
			ImGui::Text((const char*)GetIconUTF8Reference(this->icon));
			ImGui::PopFont();
			ImGui::SameLine();
			(HasSkin) ? ImGui::PushFont(ImGui::GetFontByName(this->skin->titleData.font)) : ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("IframeTitle")));
			ImGui::Text(this->title.c_str());
			ImGui::PopFont();

			// TABS
			if (this->bHasTabs)
			{
				if (ImGui::BeginTabBar(("tab_" + this->title).c_str()))
				{
					for (auto& it : this->listOfTabs)
					{
						it->AsTab()->iframe = this;
						it->Render();
						if (it->AsTab()->closeOtherTabs)
							this->CloseAllTabs(it->AsTab());
					}
					ImGui::EndTabBar();
				}
			}

			// ELEMENTS
			this->RenderElements(this);

			bool toBeClosed = false;

			// X button
			if (HasSkin == true && this->bCanBeClosed_xml == true && this->closeButton)
			{
				this->closeButton->Render();
				if (this->closeButton->IsClicked() == true)
				{
					this->closeButton->SetPressedValue(false);
					toBeClosed = true;
				}
			}

			if (HasSkin)
				ImGui::GetWindowDrawList()->PopClipRect();

			// END PART
			ImGui::End();

			if (HasSkin)
			{
				ImGui::PopStyleVar(1);
				ImGui::PopStyleColor(4);
			}

			// Close with X or with ESC
			if ((toBeClosed) || (this->bIsFocused && rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_ESCAPE)))
			{
				this->closeButton->SetPressedValue(false);
				this->Close();
			}
		}
	}
	ImVec2 Iframe::GetAbsolutePosition(void) const
	{
		return this->GetPosition();
	}
#pragma endregion


#pragma region To scripts members:
	std::shared_ptr<ImGuiElement> Iframe::operator[](const std::string& elementIdentificator) const
	{
		const char splitElement = '#';
		std::string elementTag;
		std::string elementIdStr;

		// Try to get tag and ID splitting provided string. 
		// Return if splitting was not possibile, since element of course doesn't exist
		const bool splittingResult = std::split_string(elementIdentificator, elementTag, elementIdStr, splitElement);
		if (splittingResult == false)
			return std::shared_ptr<ImGuiElement>();

		// Return if after splitting we don't have two strings, since element of course doesn't exist
		if (elementTag.empty() == true || elementIdStr.empty() == true)
			return std::shared_ptr<ImGuiElement>();

		// Return if second string starts with '-', since element of course doesn't exist (element ID are positive values!)
		if (elementIdStr[0] == '-')
			return std::shared_ptr<ImGuiElement>();

		ImGuiElement::elementID_t id = 0;
		try
		{
			// Try to convert str to (positive) number.
			// If str isn't a number or is too big, an exception will be thrown
			// We handle both simply return, since element of course doesn't exist (ID are positive values!)
			id = std::stoi(elementIdStr);
		}
		catch (const std::invalid_argument&)
		{
			return std::shared_ptr<ImGuiElement>();
		}
		catch (const std::out_of_range&)
		{
			return std::shared_ptr<ImGuiElement>();
		}

		// Return element (if exists)
		ImGuiElement::ClassesTypes type = ImGuiElement::GetImGuiElementTypeByStr(elementTag);
		return this->GetElementByTagAndId(type, id);
	}

	bool Iframe::CheckIfElementExistsByTagAndId_Py(const std::string& tag, const ImGuiElement::elementID_t id) const
	{
		ImGuiElement::ClassesTypes type = ImGuiElement::GetImGuiElementTypeByStr(tag);
		return this->CheckIfElementExistsByTagAndId(type, id);
	}

	void Iframe::ClearElementsByTag(const ImGuiElement::ClassesTypes classType)
	{
		if (this->elementsMap.contains(classType) == false)
			return;
		this->elementsNumber -= this->elementsMap.at(classType).elements.size();
		this->elementsMap.erase(classType);
	}

	void Iframe::Close(void)
	{
		if (this->bIsClosed == true)
			return;

		if (this->closingFunction)
		{
			// Run (if there is) iframe closing function 
			this->closingFunction(this->bIsEmpty_xml);
		}

		PyInterpreter& pyInterpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
		pyInterpreter.Bind(this->bIsTab ? "__tab__" : "__iframe__", PyInterpreter::PyBindedTypes::Iframe, this);
		pyInterpreter.Evaluate(this->closingFun);

		this->bIsOpened = false;
		this->bIsClosed = true;
		this->bIsFocused = false;
	}

	std::shared_ptr<ImGuiElement> Iframe::GetElementByTagAndId_Py(const std::string& tag, const ImGuiElement::elementID_t id)
	{
		ImGuiElement::ClassesTypes type = ImGuiElement::GetImGuiElementTypeByStr(tag);
		return this->GetElementByTagAndId(type, id);
	}

	std::string Iframe::GetId(void) const
	{
		return this->id;
	}

	std::pair<float, float> Iframe::GetPosition_Py(void) const
	{
		return std::pair<float, float>(this->position.x, this->position.y);
	}

	std::pair<float, float> Iframe::GetSize_Py(void) const
	{
		return std::pair<float, float>(this->size.x, this->size.y);
	}

	std::shared_ptr<Iframe> Iframe::GetTabByIndex(const uint8_t index) const
	{
		return (index < this->listOfTabs.size()) ? this->listOfTabs.at(index) : std::shared_ptr<Iframe>();
	}

	bool Iframe::IsOpened(void) const
	{
		return this->bIsOpened;
	}

	void Iframe::Open(void)
	{
		if (this->bIsOpened == true)
			return;

		if (this->openingFunction)
		{
			// Run (if there is) iframe opening function 
			this->openingFunction(this->bIsEmpty_xml);
		}

		PyInterpreter& pyInterpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
		pyInterpreter.Bind(this->bIsTab ? "__tab__" : "__iframe__", PyInterpreter::PyBindedTypes::Iframe, this);
		pyInterpreter.Evaluate(this->openingFun);

		this->bIsOpened = true;
		this->bIsClosed = false;
	}

	void Iframe::SetPosition_Py(std::pair<float, float> pos)
	{
		this->SetPosition(ImVec2(pos.first, pos.second));
	}

	void Iframe::SetSize_Py(std::pair<float, float> value)
	{
		this->SetSize(ImVec2(value.first, value.second));
	}
#pragma endregion


#pragma region Internal use:
	bool Iframe::ClearElementByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id)
	{
		if (this->CheckIfElementExistsByTagAndId(type, id) == false)
			return false;

		classTypeInfo_t& classTypeInfo = this->elementsMap.at(type);
		if (classTypeInfo.elements.size() > 1)
		{
			// If here, there are at least two elements.
			// So remove element having the given ID and then make ID it available.
			classTypeInfo.elements.erase(id);
			classTypeInfo.availableIDs.insert(id);
		}
		else
		{
			// There is just an element of the given type, so simply remove given type from map.
			this->elementsMap.erase(type);
			// Now, there are no elements of the given type --> All IDs of the given type are available
		}

		this->elementsNumber -= 1;

		return true;
	}

	bool Iframe::CheckIfElementExistsByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id) const
	{
		return (this->elementsMap.contains(type) == true && this->elementsMap.at(type).elements.contains(id) == true);
	}

	ImGuiElement::elementID_t Iframe::GetCurrentElementId(void) const
	{
		return this->currentElement.lock()->GetId();
	}

	std::shared_ptr<ImGuiElement> Iframe::GetElementByTagAndId(const ImGuiElement::ClassesTypes type, const ImGuiElement::elementID_t id) const
	{
		if (this->CheckIfElementExistsByTagAndId(type, id) == false)
			return std::shared_ptr<ImGuiElement>();
		return this->elementsMap.at(type).elements.at(id);
	}
#pragma endregion

	IEnvironment::Environments Iframe::GetEnv(void) const
	{
		return this->env;
	}

	ImVec2 Iframe::GetPosition(void) const
	{
		return this->position;
	}

	void Iframe::SetPosition(ImVec2 pos)
	{
		this->position = std::move(pos);
	}

	ImVec2 Iframe::GetSize(void) const
	{
		return this->size;
	}

	void Iframe::SetSize(ImVec2 size)
	{
		this->size = std::move(size);
	}

	bool Iframe::IsFocused(void) const
	{
		return this->bIsFocused;
	}

	bool Iframe::IsEmpty(void) const
	{
		return this->bIsEmpty_xml;
	}

	bool Iframe::HasFlag(const std::string& flag) const
	{
		return this->intFlags.contains(flag);
	}

	void Iframe::SetFlag(const std::string& key, const uint64_t value)
	{
		this->intFlags[key] = value;
	}

	std::optional<uint64_t> Iframe::GetFlag(const std::string& key) const
	{
		return (this->intFlags.contains(key) == true) ? this->intFlags.at(key) : std::optional<uint64_t>();
	}

	void Iframe::ClearFlag(const std::string& key)
	{
		this->intFlags.erase(key);
	}

	void Iframe::SetOpeningFunction(std::function<void(bool& empty)> _openingFunction)
	{
		this->openingFunction = std::move(_openingFunction);
	}

	void Iframe::SetClosingFunction(std::function<void(bool& empty)> _closingFunction)
	{
		this->closingFunction = std::move(_closingFunction);
	}

	void Iframe::SetSkin(std::shared_ptr<IframeSkinCollection::IframeSkin> _skin)
	{
		this->skin = std::move(_skin);
	}

	std::shared_ptr<IframeSkinCollection::IframeSkin>Iframe::GetSkin(void) const
	{
		return this->skin;
	}

	std::string Iframe::GetSkinName(void) const
	{
		return (this->skin) ? this->skinName : "";
	}

	void Iframe::CloseAllTabs(IframeTab* exceptThis)
	{
		for (auto& i : this->listOfTabs)
		{
			if (i.get() != exceptThis)
				i->Close();
		}
	}

	void Iframe::AddElement(std::shared_ptr<ImGuiElement> el)
	{
		assert(el);

		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		if (this->elementsNumber >= maxID)
			throw std::runtime_error("This iframe cannot contain other elements");  // TODO exception - Aggiungere apposita eccezione

		ImGuiElement::elementID_t newElementID = el->GetId();
		ImGuiElement::ClassesTypes newElementClassType = el->GetClassType();
		if (this->elementsMap.contains(newElementClassType) == true)
		{
			// If here, there is at least an other element having the type of the element that is being inserted

			classTypeInfo_t& curSet = this->elementsMap.at(newElementClassType);
			if (curSet.elements.contains(newElementID) == true)
			{
				// If here, this Iframe has already an element of a given type having the provided ID
				// For instance, this can happen if an XML is corrupted
				throw std::runtime_error("Duplicated ID for a given class in a given iframe");  // TODO exception - Aggiungere apposita eccezione
			}

			// Insert new element
			curSet.elements.insert({ newElementID, el });

			// Then remove element ID from available IDs
			assert(curSet.availableIDs.contains(newElementID) == true);
			curSet.availableIDs.erase(newElementID);
		}
		else
		{
			// If here, it's being inserted the first element of a given type
			// So, all IDs of the given type are available, except for the id of the element being added (see also classTypeInfo_t constructor)

			// Insert element of the given type
			elementsByIdMap_t newSet{ {newElementID, el} };
			classTypeInfo_t typeInfo{ std::move(newSet) };
			this->elementsMap.insert({ newElementClassType, std::move(typeInfo) });
		}

		// Insert element in the rendering list
		if (newElementClassType == ImGuiElement::ClassesTypes::e_image)
			this->elementsList.push_front(std::move(el));
		else
			this->elementsList.push_back(std::move(el));

		// A new element was added
		this->elementsNumber += 1;
	}

	std::shared_ptr<ImGuiImageButton> Iframe::AddButton(const float xPos, const float yPos, const float width, const float height, std::string text, std::string imageName, std::string onclickScript, const bool executeOnRelease)
	{
		std::shared_ptr<ImGuiImageButton> button;
		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		if (this->elementsNumber < maxID)
		{
			// If here, this iframe can contain other elements...
			// So it's possible create a new button and add it to the iframe 

			// Firstly, get an available ID for the button to create
			const ImGuiElement::elementID_t id = this->GetAvailableIdForType(ImGuiElement::ClassesTypes::e_imageButton);

			ImVec2 pos{ xPos, yPos };
			ImVec2 size{ width, height };
			button = ImGuiImageButton::CreateButton(this->me.lock(), id, std::move(pos), std::move(size), std::move(text), std::move(imageName), std::move(onclickScript), executeOnRelease);
			this->AddElement(button);
		}
		return button;
	}

	std::shared_ptr<ImGuiImage> Iframe::AddImage(const float xPos, const float yPos, const float width, const float height, std::string imageName, std::string tooltip)
	{
		std::shared_ptr<ImGuiImage> image;
		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		if (this->elementsNumber < maxID)
		{
			// If here, this iframe can contain other elements...
			// So it's possible create a new image and add it to the iframe 

			// Firstly, get an available ID for the image to create
			const ImGuiElement::elementID_t id = this->GetAvailableIdForType(ImGuiElement::ClassesTypes::e_image);

			ImVec2 pos{ xPos, yPos };
			ImVec2 size{ width, height };
			image = ImGuiImage::CreateImage(this->me.lock(), id, std::move(pos), std::move(size), std::move(imageName), std::move(tooltip));
			this->AddElement(image);
		}
		return image;
	}

	std::shared_ptr<ImGuiProgressBar> Iframe::AddProgressBar(const float xPos, const float yPos, const float width, const float height, const float progress, const bool inverted)
	{
		std::shared_ptr<ImGuiProgressBar> progressBar;
		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		if (this->elementsNumber < maxID)
		{
			// If here, this iframe can contain other elements...
			// So it's possible create a new progress bar and add it to the iframe 

			// Firstly, get an available ID for the progress bar to create
			const ImGuiElement::elementID_t id = this->GetAvailableIdForType(ImGuiElement::ClassesTypes::e_progressBar);

			ImVec2 pos{ xPos, yPos };
			ImVec2 size{ width, height };
			progressBar = ImGuiProgressBar::CreateProgressBar(this->me.lock(), id, std::move(pos), std::move(size), progress, inverted);
			this->AddElement(progressBar);
		}
		return progressBar;
	}

	std::shared_ptr<ImGuiText> Iframe::AddText(const float xPos, const float yPos, std::string text)
	{
		std::shared_ptr<ImGuiText> textSP;
		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		if (this->elementsNumber < maxID)
		{
			// If here, this iframe can contain other elements...
			// So it's possible create a new progress bar and add it to the iframe 

			// Firstly, get an available ID for the text to create
			const ImGuiElement::elementID_t id = this->GetAvailableIdForType(ImGuiElement::ClassesTypes::e_text);

			ImVec2 pos{ xPos, yPos };
			textSP = ImGuiText::CreateText(this->me.lock(), id, std::move(pos), std::move(text));
			this->AddElement(textSP);
		}
		return textSP;
	}


#pragma region Protected members:
	void Iframe::InitTabs(void)
	{
		const int nTabs = static_cast<int>(this->listOfTabs.size());
		for (int i = nTabs - 1; i >= 0; i--)
		{
			this->listOfTabs[i]->Open();
			this->listOfTabs[i]->Close();
		}
		this->bTabsInitialized = true;
	}

	void Iframe::RenderElements(const gui::Iframe* iframe)
	{
		auto element_it = this->elementsList.begin();
		while (element_it != this->elementsList.end())
		{
			auto element = (*element_it).lock();
			if (element)
			{
				if (element->IsHidden() == false)
				{
					this->currentElement = element;
					element->Render();
				}
				element_it++;
			}
			else
			{
				element_it = this->elementsList.erase(element_it);
			}
		}

		// ASSERTION: An iframe must not have more elements than a given value
		assert(this->elementsList.size() <= std::numeric_limits<ImGuiElement::elementID_t>::max());
		assert(this->elementsNumber <= std::numeric_limits<ImGuiElement::elementID_t>::max());
	}

	IframeTab* Iframe::AsTab(void)
	{
		return (IframeTab*)this;
	}

	IframeEditorTree* Iframe::AsEditorTree(void)
	{
		return (IframeEditorTree*)this;
	}
#pragma endregion


#pragma region Private members:
	void Iframe::CreateFromXmlElement(tinyxml2::XMLElement* el)
	{
#if CENTURION_DEBUG_MODE
		ResetCounters();
#endif

		auto& py = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
		auto& engine = Engine::GetInstance();

		if (this->bIsTab == false)
		{
			float _width_ = engine.GetViewportWidth();
			float _height_ = engine.GetViewportHeight();
			py.Bind("_width_", PyInterpreter::PyBindedTypes::Float, &_width_);
			py.Bind("_height_", PyInterpreter::PyBindedTypes::Float, &_height_);

			// set id
			this->id = TryParseStrAttribute(el, "id");

			// set size and position (dynamic python script)
			std::string sizeScript = TryParseStrAttribute(el, "size");
			std::string positionScript = TryParseStrAttribute(el, "position");
			this->size = ImVec2(350, 400); // default values
			this->position = ImVec2(100, 100); // default values
			if (sizeScript.empty() == false)
			{
				py.Evaluate(sizeScript);
				float w = 0.f, h = 0.f;
				py.Evaluate("w", PyInterpreter::PyEvaluationTypes::Float, &w);
				py.Evaluate("h", PyInterpreter::PyEvaluationTypes::Float, &h);
				this->size = ImVec2(float(w), float(h));
			}
			if (positionScript.empty() == false)
			{
				py.Evaluate(positionScript);
				float x = 0.f, y = 0.f;
				py.Evaluate("x", PyInterpreter::PyEvaluationTypes::Float, &x);
				py.Evaluate("y", PyInterpreter::PyEvaluationTypes::Float, &y);
				this->position = ImVec2(static_cast<float>(x), rattlesmake::peripherals::viewport::get_instance().GetHeight() - static_cast<float>(y));
			}

			// set skin
			this->skinName = TryParseStrAttribute(el, "skin");
			if (this->skinName.empty() == true)
				this->skinName = "emptyIframeMenu";
			this->SetSkin(IframeSkinCollection::IframeSkin::GetSkin(this->skinName));

			// create skin if not created
			if (this->skin->bCreated == false)
				this->skin->Create();

			// read tabs
			if (this->bIsEditorTree == false && el->FirstChildElement("tabArray") != nullptr)
			{
				for (XMLElement* i = el->FirstChildElement("tabArray")->FirstChildElement(); i != nullptr; i = i->NextSiblingElement())
				{
					this->listOfTabs.push_back(std::shared_ptr<Iframe>(new IframeTab(this->env, this->size, this->position)));
					this->listOfTabs.back()->me = this->listOfTabs.back();
					this->listOfTabs.back()->SetSkin(this->skin);
					this->listOfTabs.back()->CreateFromXmlElement(i);
				}
			}
			this->bHasTabs = this->listOfTabs.size() > 0;

			// set closing button
			if (this->skin != nullptr && this->skin->button_x)
			{
				ImVec2 closeButtonPos{ this->size.x - this->skin->button_x->get_image_width() - 2, 2 };
				this->closeButton = ImGuiImageButton::CreateCloseButton(this->me.lock(), std::move(closeButtonPos), this->skin->button_x_name);
			}
		}

		// set title
		if (this->skinName != "emptyIframeMenu")
		{
			std::string title = TryParseStrAttribute(el, "title");
			dbTranslation_t translation = SqlService::GetInstance().GetTranslation(title, false);
			this->title = (translation != "") ? std::move(translation) : std::move(title);
		}
		else
		{
			this->title = TryParseStrAttribute(el, "title");
		}

		// python functions
		this->openingFun = TryParseStrAttribute(el->FirstChildElement("openingScript"), "fun");
		if (this->openingFun.empty() == false)
			this->openingFun = this->openingFun + PyService::GetInstance().GetArgs(this->env);
		this->closingFun = TryParseStrAttribute(el->FirstChildElement("closingScript"), "fun");
		if (this->closingFun.empty() == false)
			this->closingFun = this->closingFun + PyService::GetInstance().GetArgs(this->env);
		this->runtimeFun = TryParseStrAttribute(el->FirstChildElement("runtimeScript"), "fun");
		if (this->runtimeFun.empty() == false)
			this->runtimeFun = this->runtimeFun + PyService::GetInstance().GetArgs(this->env);

		// set icon
		this->icon = TryParseStrAttribute(el, "icon");

		// set booleans
		this->bIsOpenedDefault_xml = TryParseBoolAttribute(el, "isOpened");
		this->bCanBeMoved_xml = TryParseBoolAttribute(el, "canBeMoved", true);
		this->bCanBeClosed_xml = TryParseBoolAttribute(el, "canBeClosed", true);
		this->bInteractable_xml = TryParseBoolAttribute(el, "interactable", true);

		if (this->bIsEditorTree == true)
			return;

		// Set elements
		auto& listOfArrays = ImGuiElement::GetImGuiElementArrayTags();
		for (auto const& s : listOfArrays)
			this->ReadImGuiElementsFromXmlArray(el, s.c_str());
	}

	void Iframe::ReadImGuiElementsFromXmlArray(tinyxml2::XMLElement* el, const std::string& xmlArrayTag)
	{
		const char* xml_array_cstr = xmlArrayTag.c_str();
		if (el->FirstChildElement(xml_array_cstr) != nullptr)
		{
			int defaultWidth = TryParseDynamicIntAttribute(el->FirstChildElement(xml_array_cstr), "width");
			int defaultHeight = TryParseDynamicIntAttribute(el->FirstChildElement(xml_array_cstr), "height");

			//ASSERTION: When the iframe was created, a reference to itself must be set
			assert(this->me.expired() == false);

			for (XMLElement* i = el->FirstChildElement(xml_array_cstr)->FirstChildElement(); i != nullptr; i = i->NextSiblingElement())
			{
				ImVec2 el_size{ static_cast<float>(defaultWidth), static_cast<float>(defaultHeight) };
				std::shared_ptr<ImGuiElement> new_el = ImGuiElement::CreateElement(xmlArrayTag, i, this->me.lock());
				this->AddElement(new_el);
			}
		}
	}

	ImGuiElement::elementID_t Iframe::GetAvailableIdForType(const ImGuiElement::ClassesTypes type) const
	{
		ImGuiElement::elementID_t id = 0;
		if (this->elementsMap.contains(type) == true)
		{
			auto& ids_set = this->elementsMap.at(type).availableIDs;
			id = *ids_set.begin();
		}
		return id;
	}
#pragma endregion

#pragma region Iframe Tab

	IframeTab::IframeTab(IEnvironment::Environments env, ImVec2 size, ImVec2 position) : Iframe(env)
	{
		this->bIsTab = true;
		this->size = size;
		this->position = position;
	}
	void IframeTab::Render()
	{
		this->closeOtherTabs = false;
		ImGui::PushStyleColor(ImGuiCol_Text, this->skin->tabData.text_color);
		ImGui::PushStyleColor(ImGuiCol_Tab, this->skin->tabData.background_color);
		ImGui::PushStyleColor(ImGuiCol_TabHovered, this->skin->tabData.hover_background_color);
		ImGui::PushStyleColor(ImGuiCol_TabActive, this->skin->tabData.selected_background_color);
		ImGui::PushStyleColor(ImGuiCol_Separator, this->skin->tabData.text_color);

		if (this->bIsOpened == false)
			ImGui::PushFont(ImGui::GetFontByName(this->skin->tabData.font));
		else
			ImGui::PushFont(ImGui::GetFontByName(this->skin->tabData.active_font));

		if (this->bIsOpened)
		{
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Bind("__tab__", PyInterpreter::PyBindedTypes::Iframe, this);
			PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(this->runtimeFun);
		}

		ImGui::PushItemFlag(ImGuiTabItemFlags_NoTooltip, true);
		if (ImGui::BeginTabItem(this->title.c_str()))
		{
			if (this->bIsClosed)
			{
				closeOtherTabs = true;
				this->Open();
			}

			// elements
			this->RenderElements(this->iframe);

			ImGui::EndTabItem();
		}
		ImGui::PopStyleColor(5);
		ImGui::PopFont();
		ImGui::PopItemFlag();
	}
	ImVec2 IframeTab::GetAbsolutePosition(void) const
	{
		return this->iframe->GetPosition();
	}

#pragma endregion

	void IframeEmpty::Render()
	{
		if (this->bIsOpenedDefault_xml)
			this->Open();
		if (this->bIsOpened == false && this->bIsClosed == false)
			this->Close();
		if (this->bTabsInitialized == false)
			this->InitTabs();

		if (this->bIsOpened)
		{
			PyInterpreter& py_interpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
			py_interpreter.Bind("__iframe__", PyInterpreter::PyBindedTypes::Iframe, this);
			py_interpreter.Evaluate(this->runtimeFun);

			ImGui::SetNextWindowSize(this->size, ImGuiCond_Appearing);
			ImGui::SetNextWindowPos(this->position, ImGuiCond_Appearing);

			// BEGIN PART
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin(this->title.c_str(), nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_EmptyBackground | ImGuiHoveredFlags_AllowWhenOverlapped);

			ImVec2 clippingTopLeft{ this->position.x, this->position.y };
			ImVec2 clippingBottomRight{ this->position.x + this->size.x, this->position.y + this->size.y };
			ImGui::GetWindowDrawList()->PushClipRect(std::move(clippingTopLeft), std::move(clippingBottomRight), false);

			// ELEMENTS
			this->RenderElements(this);

			ImGui::GetWindowDrawList()->PopClipRect();

			// END PART
			ImGui::End();
			ImGui::PopStyleVar(2);
		}
	}

	IframeEmpty::IframeEmpty(IEnvironment::Environments _env) : Iframe(_env)
	{
		this->bIsEmpty_xml = true;
	}

	void IframeEditorTree::Render()
	{
		if (this->editorTree == nullptr)
			return;

		if (this->bIsOpenedDefault_xml)
			this->Open();
		if (this->bIsOpened == false && this->bIsClosed == false)
			this->Close();
		if (this->bTabsInitialized == false)
			this->InitTabs();

		bool HasSkin = this->skin != nullptr;
		float LeftSkinPadding = (HasSkin) ? this->skin->GetLeftWidth() : 0.f;
		float RightSkinPadding = (HasSkin) ? this->skin->GetRightWidth() : 0.f;
		float TopSkinPadding = (HasSkin) ? this->skin->GetTopHeight() : 0.f;
		float BottomSkinPadding = (HasSkin) ? this->skin->GetBottomHeight() : 0.f;

		if (this->bIsOpened)
		{
			PyInterpreter& py_interpreter = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
			py_interpreter.Bind("__iframe__", PyInterpreter::PyBindedTypes::Iframe, this);
			py_interpreter.Evaluate(this->runtimeFun);

			ImGui::SetNextWindowPos(this->position, ImGuiCond_Appearing);

			// BEGIN PART
			if (HasSkin)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

				ImGui::PushStyleColor(ImGuiCol_Text, this->skin->titleData.text_color);
				ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0, 0, 0, 0));

				auto ImGuiWinFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HideTitleBar;

				// reset the boolean because we want to start inserting object only once!

				std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->bStartInsertingObject = false;
				if (std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->bCreated == false)
					std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->ClearSets(); // clear useless information ;)

				ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_Appearing);
				ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 400), ImVec2(-1, INFINITY));
				ImGui::PushStyleColor(ImGuiCol_ResizeGrip, this->skin->treeData.resize_grip_color);
				ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, this->skin->treeData.resize_grip_hover_color);
				ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, this->skin->treeData.resize_grip_pressed_color);
				ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, this->skin->treeData.slider_background_color);
				ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, this->skin->treeData.slider_color);
				ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, this->skin->treeData.slider_hover_background_color);
				ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, this->skin->treeData.slider_pressed_background_color);

				ImGui::Begin(("##" + this->title).c_str(), 0, ImGuiWinFlag);

				ImVec2 clippingTopLeft{ this->position.x - LeftSkinPadding, this->position.y - TopSkinPadding };
				ImVec2 clippingBottomRight{ this->position.x + this->size.x + RightSkinPadding, this->position.y + this->size.y + BottomSkinPadding };
				ImGui::GetWindowDrawList()->PushClipRect(std::move(clippingTopLeft), std::move(clippingBottomRight), false);
			}
			else
			{
				ImGui::SetNextWindowSize(this->size, ImGuiCond_Appearing);
				auto ImGuiWinFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HideTitleBar;
				if (this->bCanBeMoved_xml == false)
					ImGuiWinFlag |= ImGuiWindowFlags_NoMove;
				this->bCanBeClosed_xml ? ImGui::Begin(("##" + this->title).c_str(), &this->bIsOpened, ImGuiWinFlag) : ImGui::Begin(("##" + this->title).c_str(), nullptr, ImGuiWinFlag);
			}

			this->position = ImGui::GetWindowPos();
			this->size = ImGui::GetWindowSize();
			this->bIsFocused = ImGui::IsWindowFocused();

			// SKIN 
			if (HasSkin)
			{
				this->skin->Render(ImGui::GetWindowDrawList(), this->position, this->size);
			}

			// TITLE
			//ImGui::SetCursorPos(ImVec2(7, 3));
			//(HasSkin) ? ImGui::PushFont(ImGui::GetFontByName(this->skin->treeData.font_big)) : ImGui::PushFont(ImGui::GetFontByName(Database::ConstantStrings::Fonts::GetFontByContext("IframeTitle")));
			//ImGui::Text(this->title.c_str());
			//ImGui::PopFont();
			//ImGui::Spacing();

			// ELEMENTS
			this->RenderElements(this);

			// vertical space to avoid a glitch
			ImGui::Dummy(ImVec2(0.0f, 20.0f));

			bool toBeClosed = false;

			// X button
			if (HasSkin && this->bCanBeClosed_xml && closeButton != nullptr && this->skin->button_x)
			{
				this->closeButton->SetPosition(ImVec2(this->size.x - this->skin->button_x->get_image_width() - 2, 2)); // must be here and not in creation
				this->closeButton->Render();
				if (this->closeButton->IsClicked() == true)
				{
					this->closeButton->SetPressedValue(false);
					toBeClosed = true;
				}
			}

			if (HasSkin)
				ImGui::GetWindowDrawList()->PopClipRect();

			// END PART
			ImGui::End();

			if (HasSkin)
			{
				ImGui::PopStyleVar(1);
				ImGui::PopStyleColor(11);
			}

			// Close with X or with ESC
			if ((toBeClosed) || (this->bIsFocused && rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_ESCAPE)))
			{
				this->Close();
			}
		}
	}

	IframeEditorTree::IframeEditorTree(IEnvironment::Environments _env) : Iframe(_env)
	{
		this->bIsEditorTree = true;
	}

	void Iframe::AddEditorTree(const std::vector<std::pair<std::string, std::string>>& source, std::function<void(std::string par1, unsigned int par2)> onclick)
	{
		// prepare
		this->editorTree = std::shared_ptr<ImGuiElement>(new ImGuiEditorTree(this->me.lock()));
		std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->SetTranslations();
		//std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->title;
		this->SetSkin(IframeSkinCollection::IframeSkin::GetSkin("egyptianSand"));
		this->AddElement(this->editorTree);
		this->position = ImVec2(100, 100);

		// set close button
		if (this->skin != nullptr && this->skin->button_x)
			this->closeButton = ImGuiImageButton::CreateCloseButton(this->me.lock(), std::nullopt, this->skin->button_x_name);

		// initialize
		for (auto const& _pair : source)
			std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->AddItemFromString(_pair.first, _pair.second);
		std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->Sort();
		std::static_pointer_cast<ImGuiEditorTree>(this->editorTree)->SetOnclickFunction(onclick);
	}

#pragma region classTypeInfo_s
	Iframe::classTypeInfo_s::classTypeInfo_s(elementsByIdMap_t&& elementsMap) :
		elements(std::move(elementsMap))
	{
		auto constexpr maxID = std::numeric_limits<ImGuiElement::elementID_t>::max();
		auto id = maxID;
		while (true)
		{
			// An ID is available only if it's not present in the elements map
			if (this->elements.contains(id) == false)
				this->availableIDs.insert(id);
			if (id == 0)
				break;
			id -= 1;
		}
	}

	Iframe::classTypeInfo_s::classTypeInfo_s(classTypeInfo_s&& other) noexcept :
		availableIDs(std::move(other.availableIDs)), elements(std::move(other.elements))
	{
	}
#pragma endregion
};