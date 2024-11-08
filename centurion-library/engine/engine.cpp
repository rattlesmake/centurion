#include "header.h"
#include "engine.h"

#include <camera.h>
#include <viewport.h>
#include <mouse.h>
#include <keyboard.h>

//Environments
#include <environments/menu/menu.h>
#include <environments/game/editor/editor.h>
#include <environments/game/match/match.h>
#include <mds/mds.h>

//Players, races and colors
#include <players/player.h>
#include <players/colors_array.h>

#include "debugUi.h"
#include <environments/game/races/races_interface.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/adventure/scenario/surface/randommap/randomMapGenerator.h>

#include <icons.h>
#include <iframeSkinCollection.h>
#include <dialogWindows.h>

#include <fileservice.h>
#include <services/logservice.h>
#include <zipservice.h>
#include <services/pyservice.h>
#include <services/sqlservice.h>
#include <encode_utils.h>
#include <GLFW/glfw3.h>

#if SFML_ENABLED
#include <SFML/Audio.hpp>
#endif

#include <png_shader.h>
#include <xml_entity_shader.h>
#include <rectangle_shader.h>
#include <circle_shader.h>
#include <environments/game/adventure/scenario/surface/sea/sea_shader.h>
#include <environments/game/adventure/scenario/surface/clouds/clouds_shader.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainchunk_shader.h>

#include <framebuffers/shadows_fb.h>

namespace
{
	DebugUI DEBUG_UI;

	std::chrono::steady_clock::time_point FUNC_DURATION_PROFILING_TIME;
};

Engine Engine::instance;

Engine& Engine::GetInstance(void)
{
	return instance;
}
Engine::Engine(void) :
	camera(rattlesmake::peripherals::camera::get_instance()),
	viewport(rattlesmake::peripherals::viewport::get_instance()),
	settings(Settings::GetInstance()),
	mouse(rattlesmake::peripherals::mouse::get_instance()),
	keyboard(rattlesmake::peripherals::keyboard::get_instance()),
	zipService(rattlesmake::services::zip_service::get_instance()),
	pyService(PyService::GetInstance()),
	pyConsole(PyConsole::GetInstance()),
	fileService(rattlesmake::services::file_service::get_instance()),
	shadowsFrameBuffer(centurion::fb::shadows_fb::get_instance())
#if SFML_ENABLED
	oggService(OggService::GetInstance())
#endif
{
}

Engine::~Engine(void)
{
}

// Launch method --> It contains the main while loop
uint8_t Engine::Launch(void)
{
	try
	{
		Logger::Info("Running OpenGL Version " + this->GetOpenglVersion());
		Logger::Info("Max Array Texture Layers value: " + this->GetMaxArrayTextureLayers());
		Logger::Info("C++ Compiler Version: " + this->GetCppVersion());

		this->fps.Initialize();
		
		while (viewport.GetShouldClose() == false)
		{
			// 1. check if environment should be changed
			if (this->bChangeEnvironment == true)
				this->ChangeEnvironment();

			// 2. update fps
			this->fps.Update();

			// 3. init new frame of func duration profiling
			this->FuncDurationProfilingBegin();

			// 4. new frame of peripherals
			viewport.begin_engine_frame(ImGui::GetIO().WantCaptureMouse);
			mouse.begin_engine_frame(this->game_time.GetTotalSeconds());
			camera.begin_engine_frame(this->game_time.GetCurrentFrame());

			// feed inputs to dear imgui, start new frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (this->environment)
			{
				this->environment->Run();

				// TODO @pongo serve?
				// Load/delete useless objects textures
				// ClassesData::UpdateEntities();
			}

			// Debug ui
			if (settings.GetGlobalPreferences().IsDebugActive())
			{
				DEBUG_UI.Render();
			}

			pyConsole.Render();
			gui::RenderDialogWindows();

			// Render dear imgui into screen
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			
			// end frame of peripherals
			viewport.end_engine_frame();
			keyboard.end_engine_frame();
			mouse.end_engine_frame();

			this->FuncDurationProfilingEnd();

			this->fps.SleepFps();

			if (this->game_time.IsFrameMultipleOf(300) == true)
				zipService.clear_cache();  // each 300frames (~5 sec) clear cache

			pyConsole.ExecuteCommand();
		}

		// clear everything when click on X
		this->environment.reset();

		Logger::SaveParamsXML();

		this->settings.Save();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();
		return 0;
	}
	catch (CenturionException* e)
	{
		e->AddFuncSignature(__FUNCSIG__);
		Logger::Error(e);
		throw e;
	}
	//catch (std::exception e)
	//{
	//	EngineException* ex = new EngineException(e, __FUNCSIG__);
	//	Logger::Error(ex);
	//	throw ex->AsCenturionException();
	//}
}
Time& Engine::GetGameTime(void)
{
	Engine& e = Engine::GetInstance();
	return (e.game_time);
}

std::weak_ptr<Surface> Engine::GetSurface(void) const
{
	auto ptr = this->GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSurfaceW();
	assert(ptr.expired() == false);
	return ptr;
}

void Engine::SetEnvironment(const IEnvironment::Environments _environment)
{
	//if (_environment == IEnvironment::Environments::e_mds)
	//{
	//	//@Lezzo
	//	gui::NewInfoWindow("MDS not implmented yet", static_cast<IEnvironment::Environments>(this->environment->GetType()));
	//	return;
	//}

	//N.B.: Environment will be changed by Engine::Launch when a new frame will start
	this->nextEnvironment = _environment;
	this->bChangeEnvironment = true;

	if (this->D_LOG != nullptr)
		this->D_LOG->EnableClearing();
}

IEnvironment::Environments Engine::GetEnvironmentId(void) const
{
	if (this->environment)
		return this->environment->GetType();
	else  // This happens if some function (e.g. in gui::iframe) calls GetEnvironmentId while the environments is still being created.
		return this->nextEnvironment;
}

const std::shared_ptr<ColorsArray> Engine::GetDefaultColorsArray(void) const
{
	return this->defaultColorsArray;
}

bool Engine::Initialize(const char* exe_root)
{
	try
	{
		// Initialize file service and zip service (it must be the first operation)
		fileService.initialize(exe_root, {
			{"game", "/"},
			{"assets", "/assets/"},
			{"adventures", "/adventures/"},
			{"fonts", "/fonts/"},
			{"saves", "/saves/"},
			{"previews", "/saves/previews/"}
			});

		auto assets_path = fileService.get_folder_shortcut("assets");
		zipService.initialize({
			{"?data.zip", assets_path + "data.zip"},
			{"?entities.zip", assets_path + "entities.zip"},
			{"?music.zip", assets_path + "music.zip"},
			{"?sprites.zip", assets_path + "sprites.zip"},
			{"?sprites_extra.zip", assets_path + "sprites_extra.zip"},
			{"?terrain.zip", assets_path + "terrain.zip"},
			{"?ui.zip", assets_path + "ui.zip"}
			});

		// Initialize GLFW (important to be the second operation)
		if (glfwInit() != GLFW_TRUE)
			return false;

		// Read settings from XML (or generate default one) (important to be the third operation)
		settings.Initialize();

		// peripherals initialization.
		// it depends on settings.xml
		// so it must be after settings initialization
		viewport.initialize(
			settings.GetGlobalPreferences().GetWindowSize().x, 
			settings.GetGlobalPreferences().GetWindowSize().y, 
			settings.GetGlobalPreferences().IsFullscreenActive(),
			settings.GetGameNameStr()
		);
		camera.initialize(
			settings.GetGlobalPreferences().GetCameraMaxZoom(), 
			settings.GetGlobalPreferences().GetCameraMovespeed()
		);
		keyboard.initialize();

		// initialize shaders
		rattlesmake::geometry::rectangle_shader::get_instance().initialize();
		rattlesmake::geometry::circle_shader::get_instance().initialize();
		rattlesmake::image::png_shader::get_instance().initialize(this->viewport.GetProjectionMatrix(), this->viewport.GetViewMatrix());
		centurion::assets::xml_entity_shader::get_instance().initialize();
		sea_shader::get_instance().initialize();
		clouds_shader::get_instance().initialize();
		terrainchunk_shader::get_instance().initialize();

		// initialize frame buffers
		shadowsFrameBuffer.initialize_buffer(this->viewport.GetWidth(), this->viewport.GetHeight());
		shadowsFrameBuffer.initialize_shader(this->viewport.GetProjectionMatrix(), this->viewport.GetViewMatrix());

		this->ReadDataXml(); // read data.xml --> Colors and Races

		SurfaceSettings::GetInstance().Initialize();

#if SFML_ENABLED
		// initialize all audio context
		this->oggService.Initialize();
#endif

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(viewport.GetGlfwWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
		ImGui::StyleColorsDark(); // Setup Dear ImGui style
		this->ReadFontsXml();

		// Reading skin
		gui::IframeSkinCollection::IframeSkin::Initialize();

		Logger::CleanLogs();  // Clean logs

		// Init the environment. The starting environment should be always menu (modify engine.h to change this preference).
		this->ChangeEnvironment();

		return true;
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

void Engine::ReadDataXml(void)
{
	const string xmlText = zipService.get_text_file("?data.zip", "data.xml");

	tinyxml2::XMLDocument xmlFile;
	auto error = xmlFile.Parse(xmlText.c_str());
	if (error != tinyxml2::XML_SUCCESS)
		throw std::exception("Data XML file is corrupted.");

	// colors 
	tinyxml2::XMLElement* _colorsArray = xmlFile.FirstChildElement("data")->FirstChildElement("playerColorArray");
	this->defaultColorsArray = std::shared_ptr<ColorsArray>(new ColorsArray(_colorsArray));

	// races
	tinyxml2::XMLElement* _racesArray = xmlFile.FirstChildElement("data")->FirstChildElement("raceArray");
	RacesInterface::ReadRacesXml(_racesArray);
	xmlFile.Clear();
}

void Engine::ReadFontsXml(void)
{
	const std::string fonts_path = rattlesmake::services::file_service::get_instance().get_folder_shortcut("fonts");

	tinyxml2::XMLDocument xmlFile;
	const std::string path = fonts_path + "fonts.xml";
	xmlFile.LoadFile(path.c_str());
	ImGuiIO& io = ImGui::GetIO();

	tinyxml2::XMLElement* _fonts = xmlFile.FirstChildElement("fonts");
	for (tinyxml2::XMLElement* _font = _fonts->FirstChildElement(); _font != NULL; _font = _font->NextSiblingElement())
	{
		std::string name = _font->Attribute("name");
		std::string source = _font->Attribute("source");
		float size = (float)_font->IntAttribute("size");
		ImFont* currentFont;
		if (name != FONT_ICON_FILE_NAME_FA)
			currentFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((fonts_path + source).c_str(), size);
		else
			currentFont = ImGui::GetIO().Fonts->AddFontFromFileTTF((fonts_path + source).c_str(), size, NULL, io.Fonts->GetGlyphRangesIcon());
		ImGui::SaveFont(name, currentFont);
	}
}

string Engine::GetCppVersion(void) const
{
	string version;
	if (__cplusplus > 201703L) version = "C++20";
	else if (__cplusplus == 201703L) version = "C++17";
	else if (__cplusplus == 201402L) version = "C++14";
	else if (__cplusplus == 201103L) version = "C++11";
	else if (__cplusplus == 199711L) version = "C++98";
	else version = "pre-standard C++";
	return version;
}

std::string Engine::GetOpenglVersion(void) const
{
	std::ostringstream ss;
	ss << glGetString(GL_VERSION);
	return ss.str();
}

std::string Engine::GetMaxArrayTextureLayers(void) const
{
	GLint max_layers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);
	return std::to_string(max_layers);
}

void Engine::FuncDurationProfilingBegin(void)
{
	if (this->FUNC_DURATION_PROFILING)
	{
		auto now = std::chrono::steady_clock::now();
		auto delta = now - FUNC_DURATION_PROFILING_TIME;
		if (delta > std::chrono::seconds(1))
		{
			FUNC_DURATION_PROFILING_TIME = now;
			this->FUNC_DURATION_PROFILING_STAMP = true;
		}
	}

	if (this->FUNC_DURATION_PROFILING_STAMP)
	{
		this->D_LOG = new DurationLogger("Engine::Launch");
	}
	else
	{
		this->D_LOG = nullptr;
	}
}

void Engine::FuncDurationProfilingStamp(void)
{
	this->FUNC_DURATION_PROFILING_STAMP = true;
	FUNC_DURATION_PROFILING_TIME = std::chrono::steady_clock::now();
}

void Engine::FuncDurationProfilingStart(void)
{
	this->FUNC_DURATION_PROFILING = true;
}

void Engine::FuncDurationProfilingStop(void)
{
	this->FUNC_DURATION_PROFILING = false;
}

void Engine::FuncDurationProfilingEnd(void)
{
	if (this->FUNC_DURATION_PROFILING_STAMP)
	{
		if (this->D_LOG != nullptr)
		{
			delete D_LOG;
			this->D_LOG = nullptr;
		}
		this->FUNC_DURATION_PROFILING_STAMP = false;
	}
}

void Engine::ChangeEnvironment(void)
{
	this->bChangeEnvironment = false;

	// Get current environment
	const IEnvironment::Environments currentEnv = this->GetEnvironmentId();

	std::string openPageMenu{ "" };
	std::string matchToLoad{ "" };
	if (this->environment)
	{
		if (currentEnv == IEnvironment::Environments::e_menu)
		{
			matchToLoad = std::static_pointer_cast<Menu>(this->environment)->GetFileToLoad();
			openPageMenu = std::static_pointer_cast<Menu>(this->environment)->GetOpenPage();
		}
	}

	// Destroy previous environment
	this->environment.reset();

	// Create a new environment
	switch (this->nextEnvironment)
	{
	case IEnvironment::Environments::e_menu:
		if (openPageMenu == "")
			this->environment = Menu::CreateMenu();
		else
			this->environment = Menu::CreateMenu(std::move(openPageMenu));
		break;
	case IEnvironment::Environments::e_editor:
		this->environment = Editor::CreateEditor(currentEnv);
		break;
	case IEnvironment::Environments::e_match:
		this->environment = Match::CreateMatch(currentEnv, matchToLoad);
		if (!this->environment && currentEnv == IEnvironment::Environments::e_menu)
		{
			// An error occurs while loading a match from menu.
			// Restore main menu.
			this->environment = Menu::CreateMenu(std::move(openPageMenu));
		}
		break;
	case IEnvironment::Environments::e_mds:
		this->environment = MDS::CreateMDS();
		break;
	default:
		// I never must arrive here
		break;
	}
	// ASSERTION: An environment must be created.
	assert(this->environment);
}

#pragma region To scripts members:
void Engine::DeleteImage(const std::string image_relative_path)
{
	//todo
	//todo
	//todo
	// checkare questa parte
	//this->imageService.DeleteImage(image_relative_path);
}

void Engine::GameClose(void)
{
	if (this->pyService.CurrentInterpreterIsAdmin() == false)
	{
		throw SecurityLevelException("Not Admin");
	}

	// Ask for game closing
	std::function<void()> yes = []() {
		rattlesmake::peripherals::viewport::get_instance().SetShouldClose();
	};
	std::function<void()> no;
	gui::NewQuestionWindow("m_closing_confirmation", yes, no, IEnvironment::Environments::e_menu);
}

std::vector<std::string> Engine::GetAllRacesNames(void) const
{
	return RacesInterface::GetAllRacesNames();
}

std::vector<std::string> Engine::GetAvailableLanguages(void) const
{
	return SqlService::GetInstance().GetAllLanguages();
}

std::shared_ptr<IEnvironment> Engine::GetEnvironment(void) const
{
	return this->environment;
}

std::vector<std::string> Engine::GetListOfSaveFiles(void) const
{
	std::vector<std::string> listOfString = fileService.get_all_files_names_within_folder(rattlesmake::services::file_service::get_instance().get_folder_shortcut("saves"), CENTURION_SAVE_EXTENSION);
	for (uint16_t i = 0; i < listOfString.size(); i++)
	{
		listOfString[i] = std::remove_extension(listOfString[i]);
	}
	return listOfString;
}

std::vector<std::string> Engine::GetListOfValidAdventures(void) const
{
	std::vector<std::string> listOfStrings;
	std::vector<std::string> zipFiles = fileService.get_all_files_names_within_folder(rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures"), "zip");
	for (uint16_t i = 0; i < zipFiles.size(); i++)
	{
		if (Adventure::CheckIntegrity(zipFiles[i]))
			listOfStrings.push_back(zipFiles[i]);
	}
	return listOfStrings;
}

std::vector<std::string> Engine::GetListOfValidAdventuresWithDetails(void) const
{
	std::vector<std::string> listOfStrings;
	std::vector<std::string> zipFiles = fileService.get_all_files_names_within_folder(rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures"), "zip");

	for (uint16_t i = 0; i < zipFiles.size(); i++)
	{
		if (Adventure::CheckIntegrity(zipFiles[i]))
		{
			zipService.add_zipfile_shortcut(zipFiles[i], rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + zipFiles[i]);
			std::string xmlText = zipService.get_text_file(zipFiles[i], "properties.xml");

			tinyxml2::XMLDocument xmlFile;
			auto error = xmlFile.Parse(xmlText.c_str());
			if (error != tinyxml2::XML_SUCCESS)
				throw std::exception("Properties XML file is corrupted.");

			auto properties = xmlFile.FirstChildElement("properties");
			if (properties == nullptr)
				continue;

			std::string creationDate = tinyxml2::TryParseFirstChildStrContent(properties, "creationDate");
			if (creationDate.empty()) creationDate = "N/A";

			std::string lastEditDate = tinyxml2::TryParseFirstChildStrContent(properties, "lastEditDate");
			if (lastEditDate.empty()) lastEditDate = "N/A";

			// Removing extension from the original name to display correct value into the list
			string _name = std::remove_extension(zipFiles[i]);
			std::list<dbWord_t> wordToTranslate{ "e_text_creation_date" , "e_text_last_edit_date" };
			auto translations = SqlService::GetInstance().GetTranslations(std::move(wordToTranslate), false);
			listOfStrings.push_back(_name + '\n' + translations.at("e_text_creation_date") + creationDate + '\n' + translations.at("e_text_last_edit_date") + lastEditDate);
		}
	}
	return listOfStrings;
}

std::vector<std::string> Engine::GetScenarioSizes(void) const
{
	return SurfaceSettings::GetInstance().GetListOfMapSizes();
}

std::vector<std::string> Engine::GetScenarioTypes(void) const
{
	return SurfaceSettings::GetInstance().GetListOfMapTypes();
}

float Engine::GetViewportHeight(void) const
{
	return this->viewport.GetHeight();
}

float Engine::GetViewportWidth(void) const
{
	return this->viewport.GetWidth();
}

bool Engine::IsKeyPressed(std::string keyName) const
{
	const int keyID = rattlesmake::peripherals::keyboard::GetGlfwKeyIdFromString(keyName);
	return (keyID != -1) ? this->keyboard.IsKeyNotReleased(static_cast<uint32_t>(keyID)) : false;
}

void Engine::LoadImage(const std::string image_relative_path)
{
	//todo
	//todo
	//todo
	// checkare questa parte
	//this->imageService.LoadImage(image_relative_path);
}

void Engine::OpenEnvironment(string environment)
{
	Encode::ToLowercase(&environment);

	if (environment == "menu")
		this->SetEnvironment(IEnvironment::Environments::e_menu);
	else if (environment == "editor")
		this->SetEnvironment(IEnvironment::Environments::e_editor);
	else if (environment == "match")
		this->SetEnvironment(IEnvironment::Environments::e_match);
	else if (environment == "mds")
		this->SetEnvironment(IEnvironment::Environments::e_mds);
	rattlesmake::peripherals::mouse::get_instance().LeftClick = false;
}

dbTranslation_t Engine::Translate(std::string word) const
{
	return SqlService::GetInstance().GetTranslation(word, false);
}

std::unordered_map<dbWord_t, dbTranslation_t> Engine::TranslateWords(std::list<dbWord_t> wordsToTranslate) const
{
	return SqlService::GetInstance().GetTranslations(std::move(wordsToTranslate), false);
}
#pragma endregion //End To scripts members

void Engine::Fps::Initialize(void)
{
	this->start = std::chrono::steady_clock::now();
	this->frameBeginTime = start;
}

uint16_t Engine::Fps::GetFps(void) const
{
	return this->_Fps;
}

uint16_t Engine::Fps::GetMpfs(void) const
{
	return this->Mpfs;
}

void Engine::Fps::ToggleSleepFps(void)
{
	this->bSleepFps = !this->bSleepFps;
}

void Engine::Fps::Update(void)
{
	// Questa funzione crasha se metto un punto di debug in engine::launch.
	// Questo perche' la condizione che verifica se il tempo e' maggiore di un secondo e' vera
	// e poi nel calcolo di Mpfs si prova a dividere per 0.
	this->frameBeginTime = std::chrono::steady_clock::now();
	auto diff = frameBeginTime - start;
	if (diff >= std::chrono::seconds(1) && this->nframes > 0)
	{
		this->start = this->frameBeginTime;
		this->_Fps = this->nframes;
		this->Mpfs = 1000 / this->nframes;
		this->nframes = 0;
	}
}

void Engine::Fps::SleepFps(void)
{
	auto now = std::chrono::steady_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - frameBeginTime);
	auto gap = std::chrono::milliseconds(0);
	//std::cout << "1: " << diff.count() << std::endl;
	if (this->bSleepFps && diff <= std::chrono::milliseconds(16))
	{
		std::unique_lock<std::mutex> lk(this->cvSleepFPS_mutex);
		gap = std::chrono::milliseconds(16) - diff;
		//std::cout << "2: " << gap.count() << std::endl;
		this->cvSleepFPS.wait_for(lk, gap);
	}
	auto diff1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - frameBeginTime);
	//std::cout << "3: " << diff1.count() << std::endl;
	this->nframes++;
}
