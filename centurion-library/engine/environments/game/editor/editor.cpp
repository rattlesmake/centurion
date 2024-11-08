#include "editor.h"

#include <environments/game/adventure/scenario/minimap.h>

//Engine, peripherals and input
#include <engine.h>
#include <dialogWindows.h>
#include "addObjectWindow.h"
//Terrain
#include <environments/game/adventure/scenario/surface/terrain/terrainTextureChunks.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainBrush.h>
#include <environments/game/adventure/scenario/surface/surface.h>
//Classes
#include <environments/game/classes/objectsSet/objects_collection.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/classes/building.h>
//Services and utils
#include <services/logservice.h>
#include <fileservice.h>
#include <services/sqlservice.h>
#include <encode_utils.h>

// Assets
#include <xml_assets.h>
#include <xml_entity_shader.h>

#include <keyboard.h>
#include <camera.h>
#include <viewport.h>
#include <mouse.h>

#include <rectangle_shader.h>
#include <circle_shader.h>
#include <environments/game/adventure/scenario/surface/sea/sea_shader.h>
#include <environments/game/adventure/scenario/surface/clouds/clouds_shader.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainchunk_shader.h>

#include <GLFW/glfw3.h>


Editor* Editor::currentEditor = nullptr;
Editor::OpenAdventure_Internal_Data Editor::sOpenAdventure_Internal;
Editor::NewAdventure_Internal_Data Editor::sNewAdventure_Internal;


#pragma region Constructor and destructor:
Editor::Editor(const IEnvironment::Environments currentEnv) :
	IGame(IEnvironment::Environments::e_editor, currentEnv)
{
	auto& camera = rattlesmake::peripherals::camera::get_instance();

	auto visibleMapSize = this->GetCurrentScenario()->GetSurface()->GetVisibleMapSize();

	// Some operations are done by IGame constructor

	// Set a pointer to the current editor instance.
	Editor::currentEditor = this;

	this->xmlAssets->get_xml_classes()->fill_editor_tree_src(this->editorTreeSource);
	this->UI = std::shared_ptr<IGameUI>(new EditorUI(this->editorTreeSource));

	// initialize camera matrix
	camera.new_game(visibleMapSize, 0.f, 0.f);
}

Editor::~Editor(void)
{
	#if CENTURION_DEBUG_MODE
	std::cout << "Editor destructor" << std::endl;
	#endif
	// This instruction is safe because copy constructor and operator= are marked as deleted.
	Editor::currentEditor = nullptr;
	// Set last adventure.
	Settings::GetInstance().GetGlobalPreferences().SetLastAdventure(this->adventure->GetZipName());
}
#pragma endregion

std::shared_ptr<Editor> Editor::CreateEditor(const IEnvironment::Environments currentEnv)
{
	std::shared_ptr<Editor> newEditor{ new Editor(currentEnv) };
	return newEditor;
}

#pragma region PYTHON API
void Editor::Quit(void)
{
	//This function will ALWAYS be called whenever you try to exit the editor.
	//If the answer is YES and the adventure is edited a "Save the adventure" question will be asked as well.

	bool edited = this->adventure->IsEdited();
	std::function<void()> emptyFun;
	std::function<void()> quitFun = [edited]() {
		if (edited) // ask for saving
		{
			std::function<void()> saveFunYes = []() { // save and close
				SaveAdventure_Internal();
				Engine::GetInstance().SetEnvironment(IEnvironment::Environments::e_menu);
			};
			std::function<void()> saveFunNo = []() { // close
				Engine::GetInstance().SetEnvironment(IEnvironment::Environments::e_menu);
			};
			gui::NewQuestionWindow("e_text_save_adv_before_new_operation", saveFunYes, saveFunNo, IEnvironment::Environments::e_editor);
		}
		else // close directly if not edited
		{
			Engine::GetInstance().SetEnvironment(IEnvironment::Environments::e_menu);
		}
	};
	gui::NewQuestionWindow("e_text_closing_confirmation", quitFun, emptyFun, IEnvironment::Environments::e_editor);
}
void Editor::NewAdventure(std::string& advName)
{
	// starts a flow to create a new adventure
	auto adv_name = std::trim_copy(advName); // this will be the ZIP name (without .zip!!)
	adv_name = std::remove_extension(adv_name); // just to be sure at 100%

	// if the name inserted is empty/whitespaced
	// do nothing but warn the user
	if (adv_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + adv_name + ".zip";
	bool already_exist = rattlesmake::services::file_service::get_instance().check_if_file_exists(adv_name_full);

	// if the name is not empty
	// we have different cases

	auto iframe_ptr = gui::Iframe::GetCurrentIframe();
	assert(iframe_ptr != nullptr);

	// 1. No adventure is opened and there is no adventure with the same zip file name 
	// simplest case: just do it
	if (this->adventure->IsOpened() == false && already_exist == false)
	{
		NewAdventure_Internal(adv_name, already_exist);
	}

	// 2. No adventure is opened and there is already an adventure with the same zip file name 
	// ask for resetting
	else if (this->adventure->IsOpened() == false && already_exist == true)
	{
		std::function<void()> yes = [adv_name, already_exist]() {
			NewAdventure_Internal(adv_name, already_exist);
		};
		std::function<void()> no = [iframe_ptr] {
			iframe_ptr->Open();
		};
		gui::NewQuestionWindow("e_text_reset_other_adventure", yes, no, IEnvironment::Environments::e_editor);
	}

	// 3. Adventure is opened and there is no adventure with the same zip file name 
	// ask for saving if edited, else do it
	else if (this->adventure->IsOpened() && already_exist == false)
	{
		if (this->adventure->IsEdited())
		{
			std::function<void()> yes = [adv_name, already_exist]() {
				SaveAdventure_Internal();
				NewAdventure_Internal(adv_name, already_exist);
			};
			std::function<void()> no = [adv_name, already_exist]() {
				NewAdventure_Internal(adv_name, already_exist);
			};
			gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes, no, IEnvironment::Environments::e_editor);
		}
		else
		{
			NewAdventure_Internal(adv_name, already_exist);
		}
	}

	// 4. Adventure is opened and there is already an adventure with the same zip file name 
	// most complicated case
	else if (this->adventure->IsOpened() && already_exist == true)
	{
		std::string adv_name_lower = adv_name + ".zip";
		Encode::ToLowercase(&adv_name_lower);
		std::string current_adv_name_lower = this->adventure->GetZipName();
		Encode::ToLowercase(&current_adv_name_lower);
		bool overwriting = current_adv_name_lower == adv_name_lower;

		// Has the current adventure the same name?
		// ask for resetting
		if (overwriting)
		{
			std::function<void()> yes = [adv_name, already_exist]() {
				NewAdventure_Internal(adv_name, already_exist);
			};
			std::function<void()> no = [iframe_ptr] {
				iframe_ptr->Open();
			};
			gui::NewQuestionWindow("e_text_reset_this_adventure", yes, no, IEnvironment::Environments::e_editor);
		}

		// Hasn't the current adventure the same name?
		// ask for overwriting 
		// and then for saving if edited
		else
		{
			if (this->adventure->IsEdited())
			{
				std::function<void()> yes = [adv_name, already_exist]() {
					std::function<void()> yes_depth2 = [adv_name, already_exist]() {
						SaveAdventure_Internal();
						NewAdventure_Internal(adv_name, already_exist);
					};
					std::function<void()> no_depth2 = [adv_name, already_exist]() {
						NewAdventure_Internal(adv_name, already_exist);
					};
					gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes_depth2, no_depth2, IEnvironment::Environments::e_editor);
				};
				std::function<void()> no = [iframe_ptr] {
					iframe_ptr->Open();
				};
				gui::NewQuestionWindow("e_text_reset_other_adventure", yes, no, IEnvironment::Environments::e_editor);
			}
			else
			{
				std::function<void()> yes = [adv_name, already_exist]() {
					NewAdventure_Internal(adv_name, already_exist);
				};
				std::function<void()> no = [iframe_ptr] {
					iframe_ptr->Open();
				};
				gui::NewQuestionWindow("e_text_reset_other_adventure", yes, no, IEnvironment::Environments::e_editor);
			}
		}
	}
}
void Editor::OpenAdventure(std::string& advName)
{
	// starts a flow to open a new adventure
	std::string adv_name = advName;
	adv_name = std::trim_copy(adv_name);

	// if the adv_name is the same of the current adventure, do nothing
	std::string adv_name_lower = adv_name;
	Encode::ToLowercase(&adv_name_lower);
	std::string current_name_lower = this->adventure->GetZipName();
	Encode::ToLowercase(&current_name_lower);
	if (adv_name_lower == current_name_lower)
	{
		gui::NewInfoWindow("e_text_adventure_already_opened", IEnvironment::Environments::e_editor);
		return;
	}

	auto iframe_ptr = gui::Iframe::GetCurrentIframe();
	assert(iframe_ptr != nullptr);

	// 1. if no adventure is opened simply open the selected adv
	if (this->adventure->IsOpened() == false)
	{
		OpenAdventure_Internal(adv_name);
	}

	// 2. if is opened, check if it's edited
	// if yes, ask for saving
	else if (this->adventure->IsOpened() == true && this->adventure->IsEdited() == true)
	{
		std::function<void()> yes = [adv_name]() {
			SaveAdventure_Internal();
			OpenAdventure_Internal(adv_name);
		};
		std::function<void()> no = [adv_name]() {
			OpenAdventure_Internal(adv_name);
		};
		gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes, no, IEnvironment::Environments::e_editor);
	}

	// 3. if not, open the adv
	else if (this->adventure->IsOpened() == true && this->adventure->IsEdited() == false)
	{
		OpenAdventure_Internal(adv_name);
	}
}
void Editor::SaveAdventureAs(std::string& advName)
{
	// starts a flow to "save as" the current adventure

	auto adv_name = std::trim_copy(advName);  // this will be the ZIP name (without .zip!!)
	adv_name = std::remove_extension(adv_name);  // just to be sure at 100%

	// if the name inserted is empty/whitespace
	// do nothing but warn the user
	if (adv_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + adv_name + ".zip";
	bool already_exist = rattlesmake::services::file_service::get_instance().check_if_file_exists(adv_name_full);

	auto iframe_ptr = gui::Iframe::GetCurrentIframe();
	assert(iframe_ptr != nullptr);

	// if you are overwriting the same adventure, perform a simple save
	if (already_exist)
	{
		std::string adv_name_lower = adv_name;
		Encode::ToLowercase(&adv_name_lower);
		std::string current_name_lower = this->adventure->GetZipName();
		current_name_lower = std::remove_extension(current_name_lower);
		Encode::ToLowercase(&current_name_lower);
		if (adv_name_lower == current_name_lower)
		{
			//We are using SaveAll instead of the static internal one because we will overwrite it
			// It needs only to save the adventure avoiding zip coyping operation
			std::function<void()> yes = []() {
				SaveAdventure_Internal();
			};
			std::function<void()> no = [iframe_ptr] {
				iframe_ptr->Open();
			};
			gui::NewQuestionWindow("e_text_overwrite_this_adventure", yes, no, IEnvironment::Environments::e_editor);
		}

		// if already exists another adventure ask for overwriting
		else
		{
			std::function<void()> yes = [adv_name]() {
				SaveAdventureAs_Internal(adv_name, true);
			};
			std::function<void()> no = [iframe_ptr] {
				iframe_ptr->Open();
			};
			gui::NewQuestionWindow("e_text_overwrite_confirmation", yes, no, IEnvironment::Environments::e_editor);
		}
	}

	// else simply change name and save
	else
	{
		SaveAdventureAs_Internal(adv_name, false);
	}
}
void Editor::SaveAdventureCopy(std::string& advName)
{
	// starts a flow to "save a copy" of the current adventure
	auto adv_name = std::trim_copy(advName);  // this will be the ZIP name (without .zip!!)
	adv_name = std::remove_extension(adv_name);  // just to be sure at 100%

	// if the name inserted is empty/whitespace
	// do nothing but warn the user
	if (adv_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + adv_name + ".zip";
	bool already_exist = rattlesmake::services::file_service::get_instance().check_if_file_exists(adv_name_full);

	auto iframe_ptr = gui::Iframe::GetCurrentIframe();
	assert(iframe_ptr != nullptr);

	// If a zip file with the specified file name already exists
	if (already_exist == true)
	{
		std::string adv_name_lower = adv_name;
		Encode::ToLowercase(&adv_name_lower);
		std::string current_name_lower = this->adventure->GetZipName();
		current_name_lower = std::remove_extension(current_name_lower);
		Encode::ToLowercase(&current_name_lower);

		// If adv_name is equal to current_name, perform a simple save
		if (adv_name_lower == current_name_lower)
		{
			// We are using SaveAll instead of the static internal one because we will overwrite it
			// It needs only to save the adventure avoiding zip coyping operation
			std::function<void()> yes = []() {
				SaveAdventure_Internal();
			};
			std::function<void()> no = [iframe_ptr] {
				iframe_ptr->Open();
			};
			gui::NewQuestionWindow("e_text_overwrite_this_adventure", yes, no, IEnvironment::Environments::e_editor);
		}

		// Ask for overwriting other adventure otherwise.
		else
		{
			std::function<void()> yes = [adv_name]() {
				SaveAdventureCopy_Internal(adv_name, true);
			};
			std::function<void()> no = [iframe_ptr] {
				iframe_ptr->Open();
			};
			gui::NewQuestionWindow("e_text_overwrite_confirmation", yes, no, IEnvironment::Environments::e_editor);
		}
	}

	// else simply change name and save
	else
	{
		SaveAdventureCopy_Internal(adv_name, false);
	}
}
void Editor::DeleteAdventure(std::string& advName)
{
	// Starts a flow to delete the selected adventure
	std::string adv_name = advName;
	adv_name = std::trim_copy(adv_name);
	adv_name = std::remove_extension(adv_name);  // Just to make sure it hasn't ".zip" in the name
	adv_name = adv_name + ".zip";

	auto iframe_ptr = gui::Iframe::GetCurrentIframe();
	assert(iframe_ptr != nullptr);

	// If adv_name is equal to current_name, we create an empty adventure
	std::string adv_name_lower = adv_name;
	Encode::ToLowercase(&adv_name_lower);
	std::string current_name_lower = this->adventure->GetZipName();
	Encode::ToLowercase(&current_name_lower);

	if (adv_name_lower == "randommapsettlements.zip")
	{
		gui::NewInfoWindow("e_text_unable_to_delete_owners_adventure", IEnvironment::Environments::e_editor);
		return;
	}

	if (adv_name_lower == current_name_lower)
	{
		std::function<void()> yes = [adv_name]() {
			DeleteAdventure_Internal(adv_name, true);
		};
		std::function<void()> no = [iframe_ptr] {
			iframe_ptr->Open();
		};
		gui::NewQuestionWindow("e_text_delete_this_adventure_confirmation", yes, no, IEnvironment::Environments::e_editor);
		return;
	}

	// If it isn't, just delete the .zip file
	else
	{
		std::function<void()> yes = [adv_name]() {
			DeleteAdventure_Internal(adv_name, false);
		};
		std::function<void()> no = [iframe_ptr] {
			iframe_ptr->Open();
		};
		gui::NewQuestionWindow("e_text_delete_adventure_confirmation", yes, no, IEnvironment::Environments::e_editor);
		return;
	}
}
void Editor::RunAdventure(void)
{
	bool adv_edited = this->adventure->IsEdited();

	std::function<void()> yes = [adv_edited]() {
		if (adv_edited)
		{
			std::function<void()> yes = []() {
				SaveAdventure_Internal();
				RunAdventure_Internal();
			};
			std::function<void()> no = []() {
				RunAdventure_Internal();
			};
			gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes, no, IEnvironment::Environments::e_editor);
			return;
		}
		else
		{
			RunAdventure_Internal();
		}
	};
	std::function<void()> no = [] {};
	gui::NewQuestionWindow("e_text_ask_for_running", yes, no, IEnvironment::Environments::e_editor);
	return;
}
void Editor::InsertObject(std::string className, const uint32_t playerID)
{
	if (Editor::currentEditor == nullptr)
		return;

	try
	{
		if (className.empty() == false && playerID < PlayersArray::GetArraySize())
		{
			auto& mouse = rattlesmake::peripherals::mouse::get_instance();
			mouse.ResetAllButtons();

			glm::vec2 pos{ mouse.GetXMapCoordinate(), mouse.GetYMapCoordinate() };  // here NOT the Y 2D coord
			std::pair <glm::vec2, bool> pair{ std::move(pos), true };
			std::list<std::pair<glm::vec2, bool>> point{ std::move(pair) };
			assert(this->GetCurrentScenario()->GetSurface());

			this->insertingObject.playerId = (uint8_t)playerID;
			this->insertingObject.className = className;
			this->insertingObject.objectToInsert = ObjsCollection::CreateGObject(
				this->xmlAssets->get_xml_classes(),
				this->insertingObject.className,
				point,
				this->insertingObject.playerId,
				true,
				this->GetCurrentScenario()->GetSurface()
			);

			if (this->insertingObject.objectToInsert.has_value() == true)
			{
				this->SetState_InsertingObject();
			}
		}
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
void Editor::ChangeTerrainType(const int size, std::string type, std::string textureName, float heightValue)
{
	assert(this->GetTerrainBrushPtr()->IsEnabled() == false);

	//if (Minimap::IsActive() == true) return;
	auto& mouse = rattlesmake::peripherals::mouse::get_instance();
	if (mouse.GetCursorType() != CURSOR_TYPE_CIRCLE)
		mouse.ChangeCursorType(CURSOR_TYPE_CIRCLE);
	mouse.RightClick = false;

	// This part of code is executed when you are BEGINNING the terrain change
	this->GetTerrainBrushPtr()->Initialize(type, textureName, heightValue);
	this->GetTerrainBrushPtr()->UpdateDiameter(size);
	this->SetState_ChangingTerrain();
}

std::vector<std::string> Editor::GetEditorTerrainTreeList1(void)
{
	std::vector<std::string> editorTreeList;
	for (auto const& i : editorTerrainTree)
	{
		if (std::find(editorTreeList.begin(), editorTreeList.end(), i[0]) != editorTreeList.end()) continue;
		editorTreeList.push_back(i[0]);
	}
	std::sort(editorTreeList.begin(), editorTreeList.end());
	return editorTreeList;
}

std::vector<std::string> Editor::GetEditorTerrainTreeList2(const std::string filter1)
{
	std::vector<std::string> editorTreeList;
	for (auto const& i : editorTerrainTree)
	{
		if (std::find(editorTreeList.begin(), editorTreeList.end(), i[1]) != editorTreeList.end())
			continue;
		if (i[0] != filter1)
			continue;
		editorTreeList.push_back(i[1]);
	}
	std::sort(editorTreeList.begin(), editorTreeList.end());
	return editorTreeList;
}

bool Editor::CheckOpenObjectPropertiesCondition(void) const
{
	if (this->currentEditor == nullptr)
		return false;
	return (this->Selos()->Count() > 0);
}

void Editor::OpenObjectProperties(void) const
{
	if (this->currentEditor == nullptr)
		return;
	auto selos = this->Selos();
	if (selos && selos->Count() > 0)
		this->GetCurrentScenario()->GetPlayersArray()->OpenObjPropsIframe((*selos->Get(0)));
}
#pragma endregion

//todo
//todo
//todo
std::vector<std::array<std::string, 2>> Editor::editorTerrainTree;
std::vector<std::pair<std::string, std::string>> Editor::editorTreeSource;
void Editor::AddEditorObjectTreeElement(const std::string& path, const std::string& className)
{
	editorTreeSource.push_back(std::pair<std::string, std::string>(path, className));
}
void Editor::AddEditorTerrainTreeElement(const std::string& filter1, const std::string& filter2)
{
	std::array<std::string, 2> element = { filter1, filter2 };
	editorTerrainTree.push_back(element);
}
//todo
//todo
//todo

void Editor::ExecuteInternalMethods(void)
{
	if (sOpenAdventure_Internal.bActive == true)
	{
		OpenAdventure_Internal(sOpenAdventure_Internal.advName);
		sOpenAdventure_Internal.bActive = false;
	}
	else if (sNewAdventure_Internal.bActive == true)
	{
		NewAdventure_Internal(sNewAdventure_Internal.advName, sNewAdventure_Internal.bAlreadyExists);
		sNewAdventure_Internal.bActive = false;
	}
	this->adventure->ExecuteInternalMethods();
}

void Editor::NewAdventure_Internal(std::string advName, const bool alreadyExists)
{
	if (sNewAdventure_Internal.bActive == false)
	{
		sNewAdventure_Internal.advName = advName;
		sNewAdventure_Internal.bAlreadyExists = alreadyExists;
		sNewAdventure_Internal.bActive = true;
		return;
	}
	if (alreadyExists == true)
	{
		std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + advName + ".zip";
		rattlesmake::services::file_service::get_instance().delete_file(adv_name_full);
	}

	// Reset current adventure and init the new one as empty
	Editor::currentEditor->adventure = std::shared_ptr<Adventure>(new Adventure(Editor::currentEditor->xmlAssets->get_xml_classes()));
	Adventure::InitEmpty(Editor::currentEditor->adventure, advName);
}
void Editor::OpenAdventure_Internal(std::string advName)
{
	if (sOpenAdventure_Internal.bActive == false)
	{
		sOpenAdventure_Internal.advName = advName;
		sOpenAdventure_Internal.bActive = true;
		return;
	}

	//Reset current adeventure and init the new one loading it from zip
	Editor::currentEditor->adventure = std::shared_ptr<Adventure>(new Adventure(Editor::currentEditor->xmlAssets->get_xml_classes()));
	Adventure::InitFromZip(Editor::currentEditor->adventure, advName);
}

void Editor::SaveAdventureAs_Internal(std::string advName, const bool alreadyExists)
{
	if (alreadyExists)
	{
		std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + advName + ".zip";
		rattlesmake::services::file_service::get_instance().delete_file(adv_name_full);
	}

	auto oldZipName = Editor::currentEditor->adventure->GetZipPath();
	Editor::currentEditor->adventure->SetZipName(advName);
	auto newZipName = Editor::currentEditor->adventure->GetZipPath();

	//copy the zip
	rattlesmake::services::file_service::get_instance().copy_file(oldZipName, newZipName);

	// save
	SaveAdventure_Internal();
}

void Editor::SaveAdventureCopy_Internal(std::string advName, const bool alreadyExists)
{
	if (alreadyExists)
	{
		std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + advName + ".zip";
		rattlesmake::services::file_service::get_instance().delete_file(adv_name_full);
	}

	//Store the current .zip name, edited status and path
	auto oldZipName = Editor::currentEditor->adventure->GetZipName();
	auto oldZipPath = Editor::currentEditor->adventure->GetZipPath();
	auto edited = Editor::currentEditor->adventure->IsEdited();

	//Copy the .zip file
	std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + advName + ".zip";
	rattlesmake::services::file_service::get_instance().copy_file(oldZipPath, adv_name_full);

	//Save the adventure with the new path
	Editor::currentEditor->adventure->SetZipName(std::move(advName));
	SaveAdventure_Internal();

	//Set old name and edited status
	Editor::currentEditor->adventure->SetZipName(oldZipName);
	if (edited)
		Editor::currentEditor->adventure->MarkAsEdited();
}

void Editor::SaveAdventure_Internal(void)
{
	Editor::currentEditor->adventure->SaveAll();
}

void Editor::DeleteAdventure_Internal(std::string advName, const bool createEmpty)
{
	std::string adv_name_full = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + advName;
	if (rattlesmake::services::file_service::get_instance().delete_file(adv_name_full) == false)
	{
		gui::NewInfoWindow("e_text_unable_to_delete_adventure", IEnvironment::Environments::e_editor);
		return;
	}
	if (createEmpty == true)
	{
		NewAdventure_Internal("", false);
	}
}

void Editor::RunAdventure_Internal()
{
	Engine::GetInstance().OpenEnvironment("match");
}

std::shared_ptr<TerrainBrush> Editor::GetTerrainBrushPtr(void) const
{
	return std::static_pointer_cast<EditorUI>(this->UI)->terrainBrush;
}

void Editor::SetFocusedObject(const std::shared_ptr<GObject>& focusedObject)
{
	this->movingObject.focusedObject = focusedObject;
}

bool Editor::IsInsertingObject(void) const
{
	return this->insertingObject.objectToInsert.has_value();
}

bool Editor::IsShiftingObject(void) const
{
	return (this->movingObject.objectsToShift.expired() == false);
}

bool Editor::IsChangingTerrain(void) const
{
	return this->current_state == editor_state::changing_terrain;
}

bool Editor::IsIdle(void) const
{
	return this->current_state == editor_state::idle;
}

editor_state Editor::GetState(void)
{
	return this->current_state;
}

void Editor::SetState_Idle(void)
{
	auto& mouse = rattlesmake::peripherals::mouse::get_instance();
	std::string empty;

	// reset all variables
	// inserting object
	this->insertingObject.objectToInsert.reset();
	this->insertingObject.objectOffsets.clear();
	this->insertingObject.playerId = 0;
	this->insertingObject.className = empty;

	// shifting object
	this->movingObject.objectsToShift.reset();

	// changing terrain
	this->GetTerrainBrushPtr()->Disable();

	//Show UI again.
	this->UI->ShowIframes();
	this->UI->ShowMenubar();
	this->UI->UpdateInfoText(empty);

	mouse.ChangeCursorType(CURSOR_TYPE_DEFAULT);
	this->current_state = editor_state::idle;
}

void Editor::SetState_InsertingObject(void)
{
	// Hide UI
	this->UI->HideIframes();
	this->UI->HideMenubar();
	this->current_state = editor_state::inserting_object;
}

void Editor::SetState_ShiftingObject(void)
{
	// Hide UI
	this->UI->HideIframes();
	this->UI->HideMenubar();
	this->current_state = editor_state::shifting_object;
}

void Editor::SetState_ChangingTerrain(void)
{
	auto& mouse = rattlesmake::peripherals::mouse::get_instance();

	// Hide UI
	this->UI->HideIframes();
	this->UI->HideMenubar();
	std::string infoText = SqlService::GetInstance().GetTranslation("e_text_cancel_brush", false);
	this->UI->UpdateInfoText(infoText);

	this->current_state = editor_state::changing_terrain;
}
