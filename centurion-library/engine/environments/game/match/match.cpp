#include "match.h"

#include <engine.h>
#include <dialogWindows.h>

#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainBrush.h>
#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/minimap.h>

#include <environments/game/classes/objectsSet/objects_collection.h>
#include <environments/game/classes/objectsSet/selected_objects.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/gobject.h>

#include <png.h>
#include <rectangle_shader.h>
#include <circle_shader.h>

#include <fileservice.h>
#include <services/pyservice.h>

#include <camera.h>
#include <keyboard.h>
#include <mouse.h>
#include <viewport.h>

// Assets
#include <xml_assets.h>
#include <xml_entity_shader.h>

#include <GLFW/glfw3.h>  // always the last


Match* Match::currentMatch = nullptr;

Match::Match(const IEnvironment::Environments currentEnv, const bool bEmpty) :
	IGame(IEnvironment::Environments::e_match, currentEnv, bEmpty),
	bFromEditor(currentEnv == IEnvironment::Environments::e_editor),
	vfxCollection(), pathfinder(this->adventure->scenario->GetSurface()->GetSurfaceGrid().lock())
{
	auto& camera = rattlesmake::peripherals::camera::get_instance();

	// Some operations are done by IGame constructor
	auto visibleMapSize = this->GetCurrentScenario()->GetSurface()->GetVisibleMapSize();

	Match::currentMatch = this;

	// Initialize pathfinding
	this->pathfinder.Init();

	// Create a UI
	this->UI = std::shared_ptr<IGameUI>(new MatchUI());

	// Initialize camera matrix
	camera.new_game(visibleMapSize, static_cast<float>(this->UI->TopBarHeight), static_cast<float>(this->UI->BottomBarHeight));
}

Match::~Match(void)
{
#if CENTURION_DEBUG_MODE
	std::cout << "Match destructor" << std::endl;
#endif
	// This instruction is safe because copy constructor and operator= are marked as deleted.
	Match::currentMatch = nullptr;
}

#pragma region Static members:
std::shared_ptr<Match> Match::CreateMatch(const IEnvironment::Environments currentEnv, std::string fileToLoad)
{
	const bool bLoad = fileToLoad.empty() == false;
	std::shared_ptr<Match> newMatch{ new Match(currentEnv, bLoad) };
	if (bLoad)
	{
		// Init created match with a save file... (usually this happens because a match is being loaded from main menu)
		newMatch->Load(std::move(fileToLoad));
		const bool bLoaded = newMatch->LoadMatch();
		if (bLoaded == false)
			newMatch = std::shared_ptr<Match>();  // Match wan't created, an error occurs during loading it from file save. So return an empty shared_ptr
	}
	newMatch->UI->AsMatchUI()->SetOwnerWeakRef(newMatch);
	return newMatch;
}
#pragma endregion


std::optional<std::weak_ptr<MatchUI>> Match::GetMatchUI(void) const
{
	return (this->UI != nullptr) ? std::dynamic_pointer_cast<MatchUI>(this->UI) : std::weak_ptr<MatchUI>();
}

VFXCollection& Match::GetVFXCollection(void)
{
	return this->vfxCollection;
}


#pragma region To scripts members:
bool Match::IsLoading(void) const noexcept
{
	return (this->fileToLoad.empty() == false);
}

void Match::Load(std::string fileName)
{
	if (this->currentMatch == nullptr)
		return;
	fileName.append("." + CENTURION_SAVE_EXTENSION);
	this->fileToLoad = rattlesmake::services::file_service::get_instance().get_folder_shortcut("saves") + std::move(fileName);
}

void Match::Save(void)
{
	if (this->currentMatch == nullptr)
		return;
	this->fileToSave = "";
}

void Match::Quit(void)
{
	if (this->currentMatch == nullptr)
		return;

	// this function will ALWAYS call if you are sure to exit.	
	auto envToOpen = this->bFromEditor ? IEnvironment::Environments::e_editor : IEnvironment::Environments::e_menu;
	std::function<void()> emptyFun;
	std::function<void()> quitFun = [envToOpen]() {
		Engine::GetInstance().SetEnvironment(envToOpen);
	};
	gui::NewQuestionWindow("g_text_closing_confirmation", quitFun, emptyFun, IEnvironment::Environments::e_match);
}

std::shared_ptr<GObject> Match::PlaceObject(std::string className, const uint32_t x, const uint32_t y, const uint8_t playerID) const
{
	if (this->currentMatch == nullptr)
		return std::shared_ptr<GObject>();

	auto classesDataSP = this->xmlAssets->get_xml_classes();
	if (!classesDataSP || !classesDataSP->get_xml_class(className))
		return std::shared_ptr<GObject>();  // If there isn't any class having the provided class name

	// TODO - aggiungere i controlli sui parametri passati in input.
	glm::vec2 pos{ x,y };
	std::list<std::pair<glm::vec2, bool>> point{ {std::move(pos), true } };
	assert(this->GetCurrentScenario()->GetSurface());
	return ObjsCollection::CreateGObject(classesDataSP, className, point, playerID, false, this->adventure->GetScenario()->GetSurface());
}

FileSaveInfo Match::GetFileSaveInfo(std::string fileName)
{
	fileName.append("." + CENTURION_SAVE_EXTENSION);
	this->bfr.SetFile(fileName);
	return FileSaveInfo::GetFileSaveInfo(this->bfr);
}
#pragma endregion

#pragma region Private members:
void Match::CheckSaveLoadMatch(void)
{	
	// Firstly (i.e. when a new frame has been started) check always if a match should be loaded, saved
	if (this->fileToSave.has_value() == true)
		this->SaveMatch();
	if (this->fileToLoad.empty() == false)
		this->LoadMatch();
}
void Match::SaveMatch(void)
{
	try
	{
		std::string PATH_IN_WHICH_SAVE = rattlesmake::services::file_service::get_instance().get_folder_shortcut("saves");
		std::string& fileName = this->fileToSave.value();
		if (fileName.empty() == true)
		{
			auto end = std::chrono::system_clock::now();
			std::time_t endTime = std::chrono::system_clock::to_time_t(end);
			const char* currentTime = std::ctime(&endTime);
			fileName = currentTime;
			fileName.pop_back();
			std::replace(fileName.begin(), fileName.end(), ' ', '_');
			std::replace(fileName.begin(), fileName.end(), ':', '-');
		}

		this->bfw.SetFile(PATH_IN_WHICH_SAVE + fileName + "." + CENTURION_SAVE_EXTENSION);
		Time& gameTime = Engine::GetInstance().GetGameTime();

		// Save file info (necessary for the UI)
		this->bfw.PushString(gameTime.GetFullTimeString());
		this->bfw.PushString("UNDEFINED");
		this->bfw.PushNumber(this->adventure->GetScenario()->GetPlayersArray()->GetEffectiveNumberOfPlayers());

		// Save game time
		{
			std::vector<byte_t> timeAsByte;
			gameTime.GetBinRepresentation(timeAsByte);
			this->bfw.PushBinaryData(std::move(timeAsByte));
		}

		// Save current adventure
		this->adventure->SaveAsBinaryFile(this->bfw);

		// Get a preview image
		PATH_IN_WHICH_SAVE = rattlesmake::services::file_service::get_instance().get_folder_shortcut("previews") + fileName.append(".png");

		// TODO: Hide or Disable Pause iframe and console (if it is enabled) before taking the screenshot
		rattlesmake::peripherals::viewport::get_instance().TakeScreenshot(std::move(PATH_IN_WHICH_SAVE));
		// TODO: Show or Enable Pause iframe and console (if it was disabled) after screenshot has been taken

		// Wait until save is ended
		this->bfw.WaitForEmptyQueue();
	}
	catch (const std::exception& ex)
	{
		std::string errorLogMex = "Undefined error";
		// TODO @Lezzo
		std::cout << "Fake Logger : " << errorLogMex << std::endl;
		gui::NewInfoWindow("bad_saving", IEnvironment::Environments::e_match);
	}

	// Close the saving file (N.B.: this method waits if writing to file has not finished yet)
	this->bfw.CloseFile();

	// Match was saved (you have to clear this variable even an exception occurred previously)
	this->fileToSave = std::nullopt;
}

bool Match::LoadMatch(const IEnvironment::Environments env)
{
	bool bLoaded = true;

	/// Make some useful copies of some data in order to can restore them if the loading will fail

	// Copy time
	const uniqueID_t currentLastNextId = GObject::GetNextId();
	Time oldTime{ Engine::GetInstance().GetGameTime() };

	// Copy selos family (needed since python can be considered as static!!)
	const uint32_t selectedGObjects = this->GetNumberOfSelectedObjects();
	struct oldSelection_s
	{
		std::shared_ptr<GObject> oldSelo;
		std::shared_ptr<Building> oldSelb;
		std::shared_ptr<Hero> oldSelh;
		std::shared_ptr<Playable> oldSelp;
		std::shared_ptr<Unit> oldSelu;
		std::shared_ptr<Settlement> oldSels;
		std::shared_ptr<ObjsList> oldSelos;
		std::shared_ptr<SelectedObjects> oldSelectedUnits;
	}	oldSelection;
	oldSelection.oldSelo = this->Selo();
	oldSelection.oldSelb = this->Selb();
	oldSelection.oldSelh = this->Selh();
	oldSelection.oldSelp = this->Selp();
	oldSelection.oldSelu = this->Selu();
	oldSelection.oldSels = this->Sels();
	if (selectedGObjects >= 1)
	{
		oldSelection.oldSelos = this->Selos();
		oldSelection.oldSelectedUnits = this->SelectedUnits();
	}

	try
	{
		// Set file from which load the adventure
		this->bfr.SetFile(this->fileToLoad);

		Time& gameTime = Engine::GetInstance().GetGameTime();

		// Load file info and then ignore them.
		// They was saved only to print some info in the UI, but we need to load them anyway or the loading will fail
		FileSaveInfo fsiNotUsedHere = FileSaveInfo::GetFileSaveInfo(this->bfr);

		// Load old game time
		{
			bfr.ReadBinaryData();
			std::vector<byte_t> timeBytes = bfr.GetBinData();
			Engine::GetInstance().GetGameTime() = Time(std::move(timeBytes));
		}
		
		// Try to load an adventure 
		std::shared_ptr<Adventure> loadedAdventure{ new Adventure{ this->xmlAssets->get_xml_classes() } };
		Adventure::InitFromBinFile(loadedAdventure, bfr);

		// Wait until the loading is complete or until an error occurs
		this->bfw.WaitForEmptyQueue();

		// Delete previous adventure from memory and and set as current the one just loaded 
		this->adventure.reset();
		this->adventure = std::move(loadedAdventure);
	}
	catch (const std::exception& ex)
	{
		///If here, there was an error during the loading

		// Since there was an error:
		// 1) restore previous game time (only if loading is done by match)
		// 2) restores each static ptr with the values they had before attempting to load a new match (only if loading is done by match)
		// 3) restores selo family
		// 4) write a message into a log file
		// 5) show an info window

		if (env == IEnvironment::Environments::e_match)
		{
			// Restore previous game time
			Engine::GetInstance().GetGameTime() = oldTime;

			// Restore each static ptr
			const std::shared_ptr<Scenario> oldScenario = this->adventure->GetScenario();
			ObjsCollection::SetPlayersArrayWeakRef(oldScenario->GetPlayersArray());
			GObject::SetPlayersArrayWRef(oldScenario->GetPlayersArray());
			GObject::SetNextId(currentLastNextId);
			Area::SetSurface(this->adventure->GetScenario()->GetSurface());
			CommandsQueue::SetClassesDataRef(oldScenario->GetClassesDataSp());

			// Restore selo family (needed since python can be considered as static!!)
			auto& py = PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin);
			py.BindSharedPtr("selo", PyInterpreter::PyBindedSPTypes::GObject, oldSelection.oldSelo);
			py.BindSharedPtr("selb", PyInterpreter::PyBindedSPTypes::GObject, oldSelection.oldSelb);
			py.BindSharedPtr("selh", PyInterpreter::PyBindedSPTypes::GObject, oldSelection.oldSelb);
			py.BindSharedPtr("selp", PyInterpreter::PyBindedSPTypes::GObject, oldSelection.oldSelp);
			py.BindSharedPtr("sels", PyInterpreter::PyBindedSPTypes::Settlement, oldSelection.oldSels);
			py.BindSharedPtr("selu", PyInterpreter::PyBindedSPTypes::GObject, oldSelection.oldSelu);
			py.BindSharedPtr("selos", PyInterpreter::PyBindedSPTypes::ObjsList, oldSelection.oldSelos);
			py.BindSharedPtr("selectedObjects", PyInterpreter::PyBindedSPTypes::MultiSelection, nullptr);
		}

		// Log message
		std::string errorLogMex = "Undefined error";
		// Logger should consider both env (because a match can be loaded both from menu and match) and errorLog msg 
		//TODO @Lezzo
		std::cout << "Fake Logger : " << errorLogMex << std::endl;

		// Info window
		//TODO @Lezzo
		if (env == IEnvironment::Environments::e_match)
			gui::NewInfoWindow("bad_loading_from_match", env);  // Partita corrente sarà quindi preservata
		else  // Menu
			gui::NewInfoWindow("bad_loading_from_menu", env);

		//File wasn't loaded
		bLoaded = false;
	}

	// Close the loading file (N.B.: this method brutally stops reading from file)
	this->bfr.CloseFile();

	// Match was loaded (you have to clear this variable even an exception occurred previously)
	this->fileToLoad.clear();

	return bLoaded;
}
#pragma endregion
