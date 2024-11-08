#include "menu.h"

#include <engine.h>
#include <services/logservice.h>
#include <fileservice.h>
#include <zipservice.h>
#include <services/pyservice.h>
#include <services/oggservice.h>

#include <png_shader.h>

Menu::Menu(std::string&& pageToOpen) : IEnvironment(IEnvironment::Environments::e_menu), currentPage(pageToOpen)
{
	try
	{
		const vector<string> files = rattlesmake::services::zip_service::get_instance().get_all_files_within_folder("?data.zip", "interface/menu", "xml");

		const string pyText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "interface/menu.py");
		PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate("import admin");
		PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin).Evaluate(pyText);

		// read images
		rattlesmake::image::png_shader::get_instance().clear_cache();

		const string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "interface/menu.xml");
		tinyxml2::XMLDocument xmlFile;
		auto error = xmlFile.Parse(xmlText.c_str());
		if (error != tinyxml2::XML_SUCCESS)
			throw std::exception("Menu XML file is corrupted.");

		tinyxml2::XMLElement* _mPages = xmlFile.FirstChildElement("menuPages");
		if (_mPages != nullptr)
		{
			for (tinyxml2::XMLElement* child = _mPages->FirstChildElement("menuPage"); child != nullptr; child = child->NextSiblingElement())
			{
				const string name = tinyxml2::TryParseStrAttribute(child, "name");

				this->pagesMap[name] = std::shared_ptr<MenuPage>(new MenuPage(child));

				// add iframes to "global" iframe map for Python
				auto& iframeList = this->pagesMap[name]->GetIframesList();
				for (auto const& iframe : iframeList)
				{
					auto iframeID = name + "." + iframe.lock()->GetId();
					this->iframesMap[iframeID] = iframe.lock();
				}
			}
		}
		xmlFile.Clear();
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}

Menu::~Menu(void)
{
	#if CENTURION_DEBUG_MODE
	std::cout << "Menu destructor" << std::endl;
	#endif // CENTURION_DEBUG_MODE
}

std::shared_ptr<Menu> Menu::CreateMenu(std::string pageToOpen)
{
	std::shared_ptr<Menu> newMenu{ new Menu(std::move(pageToOpen)) };
	return newMenu;
}

void Menu::Run(void)
{
	try
	{
		//Rendering
		this->pagesMap[currentPage]->Render();

		#if SFML_ENABLED
		this->envMusic->Update();
		#endif
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		throw e;
	}
	catch (std::exception e)
	{
		EngineException* ex = new EngineException(e, __FUNCSIG__);
		throw ex->AsCenturionException();
	}
}

std::string Menu::GetOpenPage(void) const
{
	return this->currentPage;
}

std::string Menu::GetFileToLoad(void) const
{
	return this->fileToLoad;
}

#pragma region To scripts members:
void Menu::OpenPage(const std::string& pageToOpen)
{
	if (this->pagesMap.contains(pageToOpen) == false)
		return;

	this->pagesMap[this->currentPage]->isClosingEverything = true;
	this->currentPage = pageToOpen;
}

std::unordered_map<std::string, std::shared_ptr<gui::Iframe>>& Menu::GetIframes(void)
{
	return this->iframesMap;
}

void Menu::Load(std::string fileName)
{
	this->fileToLoad = std::move(fileName);
	Engine::GetInstance().SetEnvironment(IEnvironment::Environments::e_match);
}

FileSaveInfo Menu::GetFileSaveInfo(std::string fileName)
{
	BinaryFileReader bfr;
	bfr.SetFile(rattlesmake::services::file_service::get_instance().get_folder_shortcut("saves") + fileName + "." + CENTURION_SAVE_EXTENSION);
	return FileSaveInfo::GetFileSaveInfo(bfr);
}
#pragma endregion

#pragma region Menu Page
Menu::MenuPage::MenuPage(tinyxml2::XMLElement* el)
{
	this->env = IEnvironment::Environments::e_menu;
	this->pageName = TryParseStrAttribute(el, "name");
	this->pageName = std::trim_copy(this->pageName);
	if (this->pageName.empty()) 
		return;

	this->CreateFromXML(el);
}

Menu::MenuPage::~MenuPage(void)
{
}

void Menu::MenuPage::Render(void)
{
	this->isClosingEverything = false; // always reset
	
	this->RenderUIElements(true, true, true);

	// during rendering i can close the current menupage
	// so the boolean will be true and i close all the iframes.
	// the iframes with "isOpened" property in the XML will open automatically
	// not only the first time, but every time we open a page
	if (this->isClosingEverything == true)
		this->CloseAllIframes(true);
}

string Menu::MenuPage::GetPageName(void) const
{
	return this->pageName;
}
#pragma endregion
