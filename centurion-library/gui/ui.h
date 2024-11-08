/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <iframe/iframe.h>
#include "shortcuts.h"
#include "menubar.h"

namespace gui
{
	class UI
	{
	public:
		UI(void);
		~UI(void);

		[[nodiscard]] bool IsMenubarHidden(void) const;
		void ShowMenubar(void);
		void HideMenubar(void);
		void CreateFromXML(tinyxml2::XMLElement* uiXML);
		void EvaluatePythonScripts(std::string zip, std::string internalPath);
		virtual void Render();

		// iframes methods
		bool IsAnyIframeFocused(void);
		[[nodiscard]] bool AnyIframeIsOpened(void) const;
		void HideIframes(void);
		void ShowIframes(void);
		void OpenIframe(const std::string& iframeID);
		void CloseAllIframes(const bool closeEmpty = false);

		void NewQuestionWindow(std::string&& message, std::string&& functionYes, std::string&& functionNo);
		void NewInfoWindow(std::string&& message);

		std::unordered_map<std::string, std::shared_ptr<Iframe>>& GetIframesMap(void);
		std::list<std::weak_ptr<gui::Iframe>>& GetIframesList(void);
		int BottomBarHeight = 0;
		int TopBarHeight = 0;

	protected:

		void ReadMenubarFromXML(tinyxml2::XMLElement* el);
		void ReadIframesFromXML(tinyxml2::XMLElement* el);
		void ReadShortcutsFromXML(tinyxml2::XMLElement* el);
		void RenderUIElements(const bool bRenderIframes, const bool bRenderMenubar, const bool bRenderShortcuts);

		std::unordered_map<std::string, std::shared_ptr<gui::Iframe>> iframesMap;
		std::list<std::weak_ptr<gui::Iframe>> iframesList;
		std::shared_ptr<gui::Shortcuts> shortcuts;
		std::shared_ptr<gui::Menubar> menubar;

		bool bAnyIframeOpened = false;
		bool bAnyIframeFocused = false;
		bool bIframesHidden = false;
		bool bMenubarHidden = false;

		IEnvironment::Environments env{ IEnvironment::Environments::e_unknown };

	private:

		void RenderIframes(void);
		void RenderMenubar(void);
		void RenderShortcuts(void);
	};
};
