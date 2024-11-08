#include "settings.h"

#include <engine.h>
#include <camera.h>
#include <viewport.h>
#include <environments/menu/menu.h>
#include <services/logservice.h>
#include <fileservice.h>
#include <services/sqlservice.h>
#include <GLFW/glfw3.h>

#define SETTINGS_PATH "settings.xml"

Settings* Settings::instance;

#pragma region PLAYERS SETTINGS
PlayersSettings::PlayerSettings& PlayersSettings::PlayersSettings::operator[](const uint32_t idx) noexcept
{
	assert(idx < this->playerSettingsArray.size());
	return this->playerSettingsArray[idx];
}
PlayersSettings::PlayersSettings()
{
	this->playerSettingsArray = std::vector<PlayersSettings::PlayerSettings>(8, PlayersSettings::PlayerSettings());
	this->ResetToDefault();
}
PlayersSettings::PlayersSettings(const size_t nPlayers)
{
	this->playerSettingsArray = std::vector<PlayersSettings::PlayerSettings>(nPlayers, PlayersSettings::PlayerSettings());
	this->ResetToDefault();
}
void PlayersSettings::ResetToDefault(void)
{
	const size_t n = this->playerSettingsArray.size();
	for (size_t i = 0; i < n; i++)
	{
		this->playerSettingsArray[i] = PlayersSettings::PlayerSettings();
		this->playerSettingsArray[i].Id = static_cast<int>(i) + 1;
	}
}
void PlayersSettings::Deserialize(tinyxml2::XMLElement* el)
{
	if (el == nullptr)
		return;

	for (tinyxml2::XMLElement* p = el->FirstChildElement("PlayerSettings"); p != nullptr; p = p->NextSiblingElement())
	{
		PlayersSettings::PlayerSettings ps;
		ps.Deserialize(p);
		if (ps.Id > 0 && ps.Id <= this->playerSettingsArray.size())
			this->playerSettingsArray[static_cast<int64_t>(ps.Id) - 1] = ps;
	}
}
void PlayersSettings::Serialize(std::ofstream& oFile)
{
	oFile << "\t\t" << "<PlayersSettings>" << std::endl;
	for (size_t i = 0; i < this->playerSettingsArray.size(); i++)
	{
		this->playerSettingsArray[i].Serialize(oFile, i + 1);
	}
	oFile << "\t\t" << "</PlayersSettings>" << std::endl;
}
void PlayersSettings::PlayerSettings::Deserialize(tinyxml2::XMLElement* p)
{
	if (p->FirstChildElement("Id") != nullptr)
		this->Id = p->FirstChildElement("Id")->IntText(0);

	if (this->Id > 0)
	{
		if (p->FirstChildElement("Race") != nullptr && p->FirstChildElement("Race")->GetText() != nullptr)
			this->Race = p->FirstChildElement("Race")->GetText();

		if (p->FirstChildElement("Name") != nullptr && p->FirstChildElement("Name")->GetText() != nullptr)
			this->Name = p->FirstChildElement("Name")->GetText();

		if (p->FirstChildElement("Active") != nullptr)
			this->Active = p->FirstChildElement("Active")->BoolText(true);
	}
}
void PlayersSettings::PlayerSettings::Serialize(std::ofstream& oFile, size_t Id)
{
	oFile << "\t\t\t" << "<PlayerSettings>" << std::endl;
	oFile << "\t\t\t\t" << "<Id>" << Id << "</Id>" << std::endl;
	oFile << "\t\t\t\t" << "<Name>" << this->Name << "</Name>" << std::endl;
	oFile << "\t\t\t\t" << "<Race>" << this->Race << "</Race>" << std::endl;
	oFile << "\t\t\t\t" << "<Active>" << std::boolalpha << this->Active << "</Active>" << std::endl;
	oFile << "\t\t\t" << "</PlayerSettings>" << std::endl;
}
#pragma endregion

#pragma region RANDOM MAP SETTINGS
void RandomMapSettings::Deserialize(tinyxml2::XMLElement* el)
{
	if (el == nullptr)
		return;
	if (el->FirstChildElement("ScenarioSize") != nullptr && el->FirstChildElement("ScenarioSize")->GetText() != nullptr)
		this->ScenarioSize = el->FirstChildElement("ScenarioSize")->GetText();
	if (el->FirstChildElement("ScenarioType") != nullptr && el->FirstChildElement("ScenarioType")->GetText() != nullptr)
		this->ScenarioType = el->FirstChildElement("ScenarioType")->GetText();
}
void RandomMapSettings::Serialize(std::ofstream& oFile)
{
	oFile << "\t\t" << "<RandomMapSettings>" << std::endl;
	oFile << "\t\t\t" << "<ScenarioSize>" << this->ScenarioSize << "</ScenarioSize>" << std::endl;
	oFile << "\t\t\t" << "<ScenarioType>" << this->ScenarioType << "</ScenarioType>" << std::endl;
	oFile << "\t\t" << "</RandomMapSettings>" << std::endl;
}
#pragma endregion

#pragma region GLOBAL PREFERENCES
void GlobalPreferences::SetCameraMovespeed(float speed)
{
	this->CameraMovespeed = speed;
	rattlesmake::peripherals::camera::get_instance().SetMovementSpeed(speed);
}
void GlobalPreferences::SetCameraMaxZoom(float zoom) 
{
	this->CameraMaxZoom = zoom;
	rattlesmake::peripherals::camera::get_instance().SetMaxZoom(this->CameraMaxZoom);
}
float GlobalPreferences::GetCameraMaxZoom(void) const
{
	return this->CameraMaxZoom;
}
float GlobalPreferences::GetCameraMovespeed(void) const
{
	return this->CameraMovespeed;
}
void GlobalPreferences::Deserialize(tinyxml2::XMLElement* el)
{
	if (el == nullptr)
		return;

	if (el->FirstChildElement("CameraMaxZoom") != nullptr)
		this->CameraMaxZoom = el->FirstChildElement("CameraMaxZoom")->FloatText(20.f);
	if (el->FirstChildElement("CameraMovespeed") != nullptr)
		this->CameraMovespeed = el->FirstChildElement("CameraMovespeed")->FloatText(10.f);
	if (el->FirstChildElement("IsDebugActive") != nullptr)
		this->bIsDebugActive = el->FirstChildElement("IsDebugActive")->BoolText(true);
	if (el->FirstChildElement("IsFullscreenActive") != nullptr)
		this->bIsFullscreenActive = el->FirstChildElement("IsFullscreenActive")->BoolText(false);
	if (el->FirstChildElement("Language") != nullptr && el->FirstChildElement("Language")->GetText() != nullptr)
		this->Language = el->FirstChildElement("Language")->GetText();
	if (el->FirstChildElement("LastAdventure") != nullptr && el->FirstChildElement("LastAdventure")->GetText() != nullptr)
		this->LastAdventure = el->FirstChildElement("LastAdventure")->GetText();

	if (el->FirstChildElement("WindowSize") != nullptr)
	{
		if (el->FirstChildElement("WindowSize")->FirstChildElement("Width") != nullptr)
			this->WindowSize.x = el->FirstChildElement("WindowSize")->FirstChildElement("Width")->FloatText(1366.f);
		if (el->FirstChildElement("WindowSize")->FirstChildElement("Height") != nullptr)
			this->WindowSize.y = el->FirstChildElement("WindowSize")->FirstChildElement("Height")->FloatText(768.f);
	}
}
void GlobalPreferences::Serialize(std::ofstream& oFile)
{
	oFile << "\t" << "<GlobalPreferences>" << std::endl;

	oFile << "\t\t" << "<CameraMaxZoom>" << this->CameraMaxZoom << "</CameraMaxZoom>" << std::endl;
	oFile << "\t\t" << "<CameraMovespeed>" << this->CameraMovespeed << "</CameraMovespeed>" << std::endl;
	oFile << "\t\t" << "<IsDebugActive>" << std::boolalpha << this->bIsDebugActive << "</IsDebugActive>" << std::endl;
	oFile << "\t\t" << "<IsFullscreenActive>" << std::boolalpha << this->bIsFullscreenActive << "</IsFullscreenActive>" << std::endl;
	oFile << "\t\t" << "<Language>" << this->Language << "</Language>" << std::endl;
	oFile << "\t\t" << "<LastAdventure>" << this->LastAdventure << "</LastAdventure>" << std::endl;
	oFile << "\t\t" << "<WindowSize>" << std::endl;
	oFile << "\t\t\t" << "<Width>" << this->WindowSize.x << "</Width>" << std::endl;
	oFile << "\t\t\t" << "<Height>" << this->WindowSize.y << "</Height>" << std::endl;
	oFile << "\t\t" << "</WindowSize>" << std::endl;

	oFile << "\t" << "</GlobalPreferences>" << std::endl;
}
void GlobalPreferences::SetLanguage(string lang)
{
	try
	{
		if (this->Language == lang)
			return;
		this->Language = std::move(lang);

		//Reset db cache
		SqlService::GetInstance().ResetTranslationCache();
		//Recreate match in order to reload match ui translations
		Engine::GetInstance().SetEnvironment(IEnvironment::Environments::e_menu);
		//Serialize();
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
std::string GlobalPreferences::GetLanguage(void) const
{
	return this->Language;
}
bool GlobalPreferences::IsDebugActive(void) const
{
	return this->bIsDebugActive;
}
bool GlobalPreferences::IsFullscreenActive(void) const
{
	return this->bIsFullscreenActive;
}
void GlobalPreferences::SetWindowSize(int width, int height)
{
	this->WindowSize = glm::vec2(width, height);
}
const glm::vec2& GlobalPreferences::GetWindowSize(void) const
{
	return this->WindowSize;
}
std::string GlobalPreferences::GetLastAdventure(void)
{
	const std::string last_adv_path = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + this->LastAdventure;

	if (rattlesmake::services::file_service::get_instance().check_if_file_exists(last_adv_path) == false)
	{
		this->LastAdventure = "";
		//this->Serialize();
	}
	return this->LastAdventure;
}
void GlobalPreferences::SetLastAdventure(std::string advName)
{
	try
	{
		if (this->LastAdventure == advName)
			return;
		this->LastAdventure = std::move(advName);
		//this->Serialize();
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
#pragma endregion

#pragma region MATCH PREFERENCES
MatchPreferences::MatchPreferences(void)
{
	this->playersSettings = PlayersSettings(8);
}
PlayersSettings& MatchPreferences::GetPlayersSettings(void)
{
	return this->playersSettings;
}
RandomMapSettings& MatchPreferences::GetRandomMapSettings(void)
{
	return this->randomMapSettings;
}
void MatchPreferences::Deserialize(tinyxml2::XMLElement* el)
{
	if (el == nullptr)
		return;
	this->playersSettings.Deserialize(el->FirstChildElement("PlayersSettings"));
	this->randomMapSettings.Deserialize(el->FirstChildElement("RandomMapSettings"));
}
void MatchPreferences::Serialize(std::ofstream& oFile)
{
	oFile << "\t" << "<MatchPreferences>" << std::endl;
	this->playersSettings.Serialize(oFile);
	this->randomMapSettings.Serialize(oFile);
	oFile << "\t" << "</MatchPreferences>" << std::endl;
}
#pragma endregion

#pragma region EDITOR PREFERENCES
EditorPreferences::EditorPreferences(void)
{
	//this->playersSettings = PlayersSettings(16);
}
RandomMapSettings& EditorPreferences::GetRandomMapSettings(void)
{
	return this->randomMapSettings;
}
void EditorPreferences::Deserialize(tinyxml2::XMLElement* el)
{
	if (el == nullptr)
		return;
	this->randomMapSettings.Deserialize(el->FirstChildElement("RandomMapSettings"));
}
void EditorPreferences::Serialize(std::ofstream& oFile)
{
	oFile << "\t" << "<EditorPreferences>" << std::endl;
	this->randomMapSettings.Serialize(oFile);
	oFile << "\t" << "</EditorPreferences>" << std::endl;
}
#pragma endregion

#pragma region SETTINGS (CONTAINER OF EVERYTHING)
Settings::Settings() { }
Settings::~Settings()
{
	if (Settings::instance != nullptr)
	{
		delete Settings::instance;
		Settings::instance = nullptr;
	}
}
Settings& Settings::GetInstance(void)
{
	if (Settings::instance == nullptr)
		Settings::instance = new Settings();
	return (*instance);
}
void Settings::Initialize(void)
{
	// Deserialize
	{
		std::string settingsFilePath = rattlesmake::services::file_service::get_instance().get_main_root() + SETTINGS_PATH;
		tinyxml2::XMLDocument settingsXmlFile;
		auto result = settingsXmlFile.LoadFile(settingsFilePath.c_str());
		if (result == tinyxml2::XMLError::XML_SUCCESS)
		{
			tinyxml2::XMLElement* settingsNode = settingsXmlFile.FirstChildElement("Settings");
			if (settingsNode != nullptr)
			{
				this->globalPreferences.Deserialize(settingsNode->FirstChildElement("GlobalPreferences"));
				this->editorPreferences.Deserialize(settingsNode->FirstChildElement("EditorPreferences"));
				this->matchPreferences.Deserialize(settingsNode->FirstChildElement("MatchPreferences"));
			}
		}
	}
	
	// Serialize
	this->Save();

	if (this->globalPreferences.IsFullscreenActive() == true)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		this->globalPreferences.SetWindowSize(mode->width, mode->height);
	}
}
void Settings::Save(void)
{
	std::string settingsFilePath = rattlesmake::services::file_service::get_instance().get_main_root() + SETTINGS_PATH;

	std::ofstream settingsFile(settingsFilePath);
	if (settingsFile.is_open())
	{
		settingsFile << "<Settings>" << std::endl;
		this->globalPreferences.Serialize(settingsFile);
		this->editorPreferences.Serialize(settingsFile);
		this->matchPreferences.Serialize(settingsFile);
		settingsFile << "</Settings>" << std::endl;
	}
	settingsFile.close();
}
GlobalPreferences& Settings::GetGlobalPreferences(void)
{
	return this->globalPreferences;
}
MatchPreferences& Settings::GetMatchPreferences(void)
{
	return this->matchPreferences;
}
EditorPreferences& Settings::GetEditorPreferences(void)
{
	return this->editorPreferences;
}
#pragma endregion
