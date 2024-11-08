/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <ui.h>
#include <environments/ienvironment.h>

#include <environments/file_save_info.h>

namespace gui { class Iframe; }
class PyService;

class Menu : public IEnvironment
{
public:	
	Menu(const Menu& other) = delete;
	Menu& operator = (const Menu& other) = delete;
	~Menu(void);

	[[nodiscard]] static std::shared_ptr<Menu> CreateMenu(std::string pageToOpen = "mainmenu");

	/// <summary>
	/// This function performs the picking assignment and the rendering of the menu.
	/// </summary>
	void Run(void) override;

	[[nodiscard]] std::string GetOpenPage(void) const;
	[[nodiscard]] std::string GetFileToLoad(void) const;

	#pragma region To scripts members:
	void OpenPage(const std::string& pageToOpen);
	[[nodiscard]] std::unordered_map<std::string, std::shared_ptr<gui::Iframe>>& GetIframes(void);
	void Load(std::string fileName);
	[[nodiscard]] FileSaveInfo GetFileSaveInfo(std::string fileName);
	#pragma endregion
private:
	explicit Menu(std::string&& pageToOpen);

	class MenuPage : public gui::UI
	{
		friend class Menu;
	public:
		explicit MenuPage(tinyxml2::XMLElement* el);
		MenuPage(const MenuPage& other) = delete;
		~MenuPage(void);

		void Render(void) override;
		[[nodiscard]] std::string GetPageName(void) const;
	private:	
		std::string pageName;
		bool isClosingEverything = false;
	};

	std::unordered_map<std::string, std::shared_ptr<gui::Iframe>> iframesMap;
	std::unordered_map<std::string, std::shared_ptr<MenuPage>> pagesMap;
	std::string currentPage;
	std::string fileToLoad;
};
