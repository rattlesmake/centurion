#include "ui.h"
#include "dialogWindows.h"
#include <keyboard.h>
#include <zipservice.h>
#include <services/pyservice.h>
#include <fileservice.h>
#include <encode_utils.h>

gui::UI::UI(void)
{
}
gui::UI::~UI(void)
{
}
bool gui::UI::IsMenubarHidden(void) const
{
	return this->bMenubarHidden;
}
void gui::UI::ShowMenubar(void)
{
	this->bMenubarHidden = false;
}
void gui::UI::HideMenubar(void)
{
	this->bMenubarHidden = true;
}
void gui::UI::CreateFromXML(tinyxml2::XMLElement* uiXML)
{
	if (uiXML == nullptr) 
		return;

	// SHORTCUTS
	if (uiXML->FirstChildElement("shortcuts") != nullptr)
		this->ReadShortcutsFromXML(uiXML->FirstChildElement("shortcuts"));

	// MENUBAR
	if (uiXML->FirstChildElement("menuBar") != nullptr)
		this->ReadMenubarFromXML(uiXML->FirstChildElement("menuBar"));

	// IFRAMES
	if (uiXML->FirstChildElement("iframeArray") != nullptr)
		this->ReadIframesFromXML(uiXML->FirstChildElement("iframeArray"));

	// Bottombar & Topbar heights
	this->BottomBarHeight = tinyxml2::TryParseDynamicIntAttribute(uiXML, "bottomBarHeight");
	this->TopBarHeight = tinyxml2::TryParseDynamicIntAttribute(uiXML, "topBarHeight");
}
void gui::UI::EvaluatePythonScripts(std::string zip, std::string internalPath)
{
	std::string pyText = rattlesmake::services::zip_service::get_instance().get_text_file(zip, internalPath);
	PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(pyText);

}
void gui::UI::Render(void)
{
	return; //Don't execute anything.
}
bool gui::UI::AnyIframeIsOpened(void) const
{
	if (this->bIframesHidden)
		return false;
	return bAnyIframeOpened;
}
bool gui::UI::IsAnyIframeFocused(void)
{
	return this->bAnyIframeFocused;
}
void gui::UI::HideIframes(void)
{
	this->bIframesHidden = true;
}
void gui::UI::ShowIframes(void)
{
	this->bIframesHidden = false;
}
void gui::UI::OpenIframe(const std::string& iframeID)
{
	if (this->iframesMap.contains(iframeID) == false)
		return;
	this->iframesMap[iframeID]->Open();
}
void gui::UI::CloseAllIframes(const bool closeEmpty)
{
	std::for_each(this->iframesList.begin(), this->iframesList.end(), [closeEmpty](std::weak_ptr<gui::Iframe> it)
	{
		if (closeEmpty == false && it.lock()->IsEmpty())
			return;
		it.lock()->Close();
	});
	this->bAnyIframeOpened = false;
}
void gui::UI::NewQuestionWindow(std::string&& message, std::string&& functionYes, std::string&& functionNo)
{
	gui::NewQuestionWindow(std::move(message), std::move(functionYes), std::move(functionNo), this->env);
}
void gui::UI::NewInfoWindow(std::string&& message)
{
	gui::NewInfoWindow(std::move(message), this->env);
}
std::unordered_map<std::string, std::shared_ptr<gui::Iframe>>& gui::UI::GetIframesMap(void)
{
	return this->iframesMap;
}
std::list<std::weak_ptr<gui::Iframe>>& gui::UI::GetIframesList(void)
{
	return this->iframesList;
}
void gui::UI::ReadMenubarFromXML(tinyxml2::XMLElement* el)
{
	this->menubar = std::shared_ptr<Menubar>(new Menubar(el));
}
void gui::UI::ReadIframesFromXML(tinyxml2::XMLElement* el)
{
	for (tinyxml2::XMLElement* child = el->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())
	{
		auto id = TryParseStrAttribute(child, "id");
		if (id.empty()) 
			continue;

		bool alreadyInMap = this->iframesMap.contains(id);

		this->iframesMap[id] = gui::Iframe::CreateIframe(this->env, child);

		if (alreadyInMap == false)
			this->iframesList.push_back(this->iframesMap[id]);
	}
}
void gui::UI::ReadShortcutsFromXML(tinyxml2::XMLElement* el)
{
	auto iframes_map_ptr = &this->iframesMap;
	std::function<bool(std::string& iframeToOpen)> openIframeFun = [iframes_map_ptr](std::string iframeToOpen) -> bool {
		if (iframes_map_ptr->contains(iframeToOpen) == false) return false;

		auto iframe_ptr = (*iframes_map_ptr)[iframeToOpen];
		if (iframe_ptr->IsOpened())
			return false;

		iframe_ptr->Open();
		return true;
	};
	this->shortcuts = std::shared_ptr<gui::Shortcuts>(new gui::Shortcuts(el, env));
	this->shortcuts->SetOpenIframeFun(openIframeFun);
}
void gui::UI::RenderIframes(void)
{
	bool iframes_hidden = this->bIframesHidden;
	bool any_iframe_focused = false;
	bool any_iframe_opened = false;

	// Render all iframes
	std::for_each(this->iframesList.begin(), this->iframesList.end(), [&iframes_hidden, &any_iframe_focused, &any_iframe_opened](std::weak_ptr<gui::Iframe>& it)
	{
		auto it_sp = it.lock();
		if (it_sp->IsEmpty() == false && iframes_hidden == true)
			return;
		it_sp->Render();
		if (it_sp->IsEmpty() == false)
		{
			// update booleans
			any_iframe_focused |= it_sp->IsFocused();
			any_iframe_opened |= it_sp->IsOpened();
		}
	});

	this->bAnyIframeFocused = any_iframe_focused;
	this->bAnyIframeOpened = any_iframe_opened;
}
void gui::UI::RenderMenubar(void)
{
	if (this->bMenubarHidden == false && this->menubar != nullptr)
		this->menubar->Render(this);
}
void gui::UI::RenderShortcuts(void)
{
	if (rattlesmake::peripherals::keyboard::get_instance().IsAnyKeyPressed() && this->shortcuts != nullptr)
		this->shortcuts->Run();
}
void gui::UI::RenderUIElements(const bool bRenderIframes, const bool bRenderMenubar, const bool bRenderShortcuts)
{
	if (bRenderShortcuts)
		this->RenderShortcuts();
	if (bRenderMenubar)
		this->RenderMenubar();
	if (bRenderIframes)
		this->RenderIframes();
}
