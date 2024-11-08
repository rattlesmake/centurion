#include "adventure.h"
#include <fileservice.h>
#include<services/sqlservice.h>
#include <services/logservice.h>
#include <zipservice.h>

#include <settings.h>
#include <encode_utils.h>
#include <engine.h>
#include <players/diplomacy.h>

#include <dialogWindows.h>
#include <environments/game/editor/editor.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>

// Assets
#include <xml_classes.h>


Adventure::OpenScenario_Internal_Data Adventure::sOpenScenario_Internal;


#pragma region Init methods and constructor and destructor:
void Adventure::InitEmpty(std::shared_ptr<Adventure>& adventureToInitSP, std::string advName)
{
	///If I'm here it means that I'm initializing a new EMPTY adventure
	// if advName = "" --> "empty" adventure with no zip opened
	Adventure& newAdventure = (*adventureToInitSP);
	
	// Adventure is empty, so we use default colors as player colors
	const std::shared_ptr<ColorsArray> colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players = std::shared_ptr<PlayersArray>(new PlayersArray(adventureToInitSP->classesDataSP, colorSP));

	// Initialize default properties
	newAdventure.SetCreationDate();
	newAdventure.SetLastEditDate();
	                                                  
	std::list<dbWord_t> wordsToTranslate{ "w_nobody", "w_no_description", "e_text_untitled_adventure" };
	auto translations = SqlService::GetInstance().GetTranslations(std::move(wordsToTranslate), false);
	newAdventure.SetAuthor(translations.at("w_nobody"));
	newAdventure.SetDescription(translations.at("w_no_description"));
	newAdventure.SetAdventureName(advName.empty() ? translations.at("e_text_untitled_adventure") : advName);

	newAdventure.SetZipName(std::move(advName));

	// Init current scenario as empty scenario. When static method returns, newAdventure.scenario will be inited
	Scenario::InitEmpty(newAdventure.scenario, newAdventure.GetZipName(), std::move(players));

	if (newAdventure.GetZipName() != "NA") // if advname != ""
		newAdventure.SaveAll();

	// Finally, set a pointer to the created adventure into the scenario belonging to the created adventure.
	newAdventure.scenario->SetCurrentAdv(adventureToInitSP);
}

void Adventure::InitRandom(std::shared_ptr<Adventure>& adventureToInitSP)
{
	/// If I'm here it means that I'm starting a random map generation from Match Menu
	const uint32_t seed = 0;  // todo
	Adventure& newAdventure = (*adventureToInitSP);

	// TODO: colors should be chosen by the user.
	const std::shared_ptr<ColorsArray> colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players = std::shared_ptr<PlayersArray>(new PlayersArray(adventureToInitSP->classesDataSP, colorSP));

	// Apply players settings before create new scenario
	players->ApplyPlayersSettings(newAdventure.settings.GetMatchPreferences().GetPlayersSettings());
	newAdventure.surfaceSettings.ApplyRandomMapSettings(newAdventure.settings.GetMatchPreferences().GetRandomMapSettings());

	// Init current scenario as random scenario. When static method returns, newAdventure.scenario will be inited
	Scenario::InitRandom(newAdventure.scenario, newAdventure.GetZipName(), seed, std::move(players));

	newAdventure.zipName = "";
	newAdventure.SetAdventureName("");

	// Finally, set a pointer to the created adventure into the scenario belonging to the created adventure.
	newAdventure.scenario->SetCurrentAdv(adventureToInitSP);
}

void Adventure::InitFromZip(std::shared_ptr<Adventure>& adventureToInitSP, std::string zipName)
{
	///If I'm here it means that I'm loading an adventure from an existing one via zip file
	Adventure& newAdventure = (*adventureToInitSP);

	newAdventure.SetZipName(std::move(zipName));
	newAdventure.LoadProperties();

	// TODO: colors should be loaded from the adventure zip.
	const std::shared_ptr<ColorsArray> colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players = std::shared_ptr<PlayersArray>(new PlayersArray(adventureToInitSP->classesDataSP, colorSP));

	// Init current scenario loading it from zip. When static method returns, newAdventure.scenario will be inited
	Scenario::InitFromZip(newAdventure.scenario, newAdventure.GetZipName(), newAdventure.properties.startingScenario, std::move(players));

	newAdventure.numberOfScenarios = static_cast<uint32_t>(newAdventure.GetListOfValidScenarios().size());

	// Finally, set a pointer to the created adventure into the scenario belonging to the created adventure.
	newAdventure.scenario->SetCurrentAdv(adventureToInitSP);
}

void Adventure::InitFromBinFile(std::shared_ptr<Adventure>& adventureToInitSP, BinaryFileReader& bfr)
{
	/// If I'm here it means that I'm loading an adventure from a binary file
	/// If you change loading order here, go to Adventure::SaveAsBinaryFile and impose the same saving order
	Adventure& newAdventure = (*adventureToInitSP);
	// TODO: colors should be loaded from the adventure save file.
	const std::shared_ptr<ColorsArray> colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players = std::shared_ptr<PlayersArray>(new PlayersArray(adventureToInitSP->classesDataSP, colorSP));

	// Init current scenario loading it from a binary save file. When static method returns, newAdventure.scenario will be inited
	Scenario::InitFromBinFile(newAdventure.scenario, bfr, std::move(players));

	// Finally, set a pointer to the created adventure into the scenario belonging to the created adventure.
	newAdventure.scenario->SetCurrentAdv(adventureToInitSP);
}

Adventure::Adventure(classesData_t _classesDataSP) :
	zipService(rattlesmake::services::zip_service::get_instance()),
	settings(Settings::GetInstance()),
	surfaceSettings(SurfaceSettings::GetInstance()),
	classesDataSP(std::move(_classesDataSP))
{
	// Create a scenario object without any initialization. It serves only to have an allocated object inside the heap.
	this->scenario = std::shared_ptr<Scenario>(new Scenario(this->classesDataSP));
}

Adventure::~Adventure(void)
{
	this->objectives.Clear();
	#if CENTURION_DEBUG_MODE
	std::cout << "Adventure destructor" << std::endl;
	#endif
}
#pragma endregion

void Adventure::NewScenario_Internal(const std::string& scenarioName, const bool saveBefore, const bool increaseScenarios)
{
	auto adv = Engine::GetInstance().GetEnvironment()->AsEditor()->GetCurrentAdventure();
	if (!adv)
		return;
	if (saveBefore) 
		adv->SaveAll();

	auto colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players{ new PlayersArray{ adv->classesDataSP, colorSP } };

	// Reset current scenario and init the new one as empty scenario. When static method returns, adv->scenario will be inited
	adv->scenario = std::shared_ptr<Scenario>(new Scenario(adv->classesDataSP));
	Scenario::InitEmpty(adv->scenario, adv->GetZipName(), std::move(players));

	adv->scenario->SetFolderName(scenarioName);
	adv->SetStartingScenario(scenarioName);
	adv->SaveAll();

	if (increaseScenarios == true)
		adv->IncreaseNumberOfScenarios();
}

void Adventure::OpenScenario_Internal(const std::string& scenarioName, const bool saveBefore)
{
	if (sOpenScenario_Internal.bActive == false)
	{
		sOpenScenario_Internal.bActive = true;
		sOpenScenario_Internal.bSaveBefore = saveBefore;
		sOpenScenario_Internal.scenarioName = scenarioName;
		return;
	}
	sOpenScenario_Internal.bActive = false;

	auto adv = std::static_pointer_cast<Editor>(Engine::GetInstance().GetEnvironment())->GetCurrentAdventure();
	if (!adv)
		return;
	if (saveBefore) 
		adv->SaveAll();

	auto colorSP = Engine::GetInstance().GetDefaultColorsArray();
	std::shared_ptr<PlayersArray> players{ new PlayersArray{ adv->classesDataSP, colorSP} };

	// Reset current scenario and init new one loading it from zip. When static method returns, adv->scenario will be inited
	adv->scenario = std::shared_ptr<Scenario>(new Scenario(adv->classesDataSP));
	Scenario::InitFromZip(adv->scenario, adv->GetZipName(), adv->properties.startingScenario, std::move(players));

	adv->SetStartingScenario(scenarioName);
	adv->SaveAdventure();
}

void Adventure::DeleteScenario_Internal(const std::string& scenarioName, const bool openAnother)
{
	auto adv = Engine::GetInstance().GetEnvironment()->AsEditor()->GetCurrentAdventureW();
	if (adv.expired())
		return;

	rattlesmake::services::zip_service::get_instance().remove_folder(adv.lock()->GetZipName(), "scenarios/" + scenarioName);
	adv.lock()->DecreaseNumberOfScenarios();
	
	if (openAnother)
	{
		auto list = adv.lock()->GetListOfValidScenarios();
		std::string scenarioToOpen = list[0];
		OpenScenario_Internal(scenarioToOpen, false);
	}
}

void Adventure::GenerateRandomScenario_Internal(const uint32_t seed)
{
	auto adv = Engine::GetInstance().GetEnvironment()->AsEditor()->GetCurrentAdventure();
	if (!adv)
		return;

	std::shared_ptr<PlayersArray> players;
	{
		// save information about players
		// that has been just changed in generate random map window
		auto parray_ptr = adv->scenario->GetPlayersArray();
		auto n_players = parray_ptr->GetEffectiveNumberOfPlayers();
		auto players_colors = parray_ptr->GetPlayersColorsList();
		auto players_races = parray_ptr->GetPlayersRacesList();

		// Create new players array based on previous information
		players = std::shared_ptr<PlayersArray>(new PlayersArray(adv->classesDataSP, n_players, players_colors, players_races));
	}	// End of scope. Here parray_ptr is destroyed, decrease counter and so scenario.reset can correctly destroy playersArray.

	// Reset current scenario and init new one as random scenario. When static method returns, newAdventure.scenario will be inited
	adv->scenario = std::shared_ptr<Scenario>(new Scenario(adv->classesDataSP));
	Scenario::InitRandom(adv->scenario, adv->GetZipName(), seed, std::move(players));

	// close all iframes
	auto ui = Engine::GetInstance().GetEnvironment()->AsEditor()->GetUIRef();
	assert(ui);
	ui->CloseAllIframes();

	adv->MarkAsEdited();
}

void Adventure::DecreaseNumberOfScenarios(void)
{
	assert(this->numberOfScenarios > 1);  // This adventure has 0 scenarios!
	this->numberOfScenarios--;
	#if CENTURION_DEBUG_MODE
	std::cout << "[DEBUG] Adventure has now " << this->numberOfScenarios << " scenarios.\n";
	#endif
}

void Adventure::IncreaseNumberOfScenarios(void)
{
	this->numberOfScenarios++;
	#if CENTURION_DEBUG_MODE
	std::cout << "[DEBUG] Adventure has now " << this->numberOfScenarios << " scenarios.\n";
	#endif
}

bool Adventure::CheckIntegrity(const std::string& zipName)
{
	std::string path = rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + zipName;
	rattlesmake::services::zip_service::get_instance().add_zipfile_shortcut(zipName, path);
	try
	{
		// Check for adventure's properties.xml file existence.
		if (rattlesmake::services::zip_service::get_instance().check_if_file_exists(zipName, "properties.xml") == false)
			throw std::exception("File properties.xml does not exist.");

		// Check if that XML is fine.
		auto xmlText = rattlesmake::services::zip_service::get_instance().get_text_file(zipName, "properties.xml");
		tinyxml2::XMLDocument xmlFile;
		if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
			throw std::exception("Properties.xml file is corrupted.");

		// Check for default scenario.
		auto properties = xmlFile.FirstChildElement("properties");
		if (properties == nullptr || properties->FirstChildElement("startingScenario") == nullptr)
			throw std::exception("Corrupted properties.xml file.");

		// Check if the adventure owns at least one scenario.
		std::vector<std::string> scenarios = rattlesmake::services::zip_service::get_instance().get_all_folders_within_folder(zipName, "scenarios");
		if (scenarios.size() == 0)
			throw std::exception("No scenarios.");

		// Check for scenarios properties.xml file existence.
		for (uint8_t i = 0; i < scenarios.size(); i++)
			if (Scenario::CheckIntegrity(zipName, "scenarios/" + scenarios[i]) == false)
				throw std::exception(std::string("Failed integrity check for scenario " + scenarios[i]).c_str());

		return true;
	}
	catch (const std::exception& ex)
	{
		Logger::LogMessage msg = Logger::LogMessage("Adventure \"" + zipName + "\"" + " files integrity has failed. Reason: " + ex.what(), "Warn", "", "Adventure", __FUNCTION__);
		Logger::Warn(msg);
		return false;
	}
}

void Adventure::NewScenario(std::string& scenarioName)
{
	// starts a flow to create a new scenario

	std::string scenario_name = std::trim_copy(scenarioName);
	Encode::ToLowercase(&scenario_name);

	// if the name inserted is empty/whitespace
	// do nothing but warn the user
	if (scenario_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string current_scenario = std::trim_copy(this->scenario->GetFolderName());
	Encode::ToLowercase(&current_scenario);

	// overwrite current?
	if (scenario_name == current_scenario)
	{
		std::function<void()> yes = [scenario_name]() {
			NewScenario_Internal(scenario_name, false, false);
		};
		std::function<void()> no;
		gui::NewQuestionWindow("Overwrite current scenario?", yes, no, IEnvironment::Environments::e_editor); //TODO Translation!
		return;
	}

	// overwrite other?
	bool overwrite = zipService.check_if_folder_exists(this->GetZipName(), "scenarios/" + scenario_name);
	if (overwrite)
	{
		if (this->IsEdited())
		{
			std::function<void()> yes = [scenario_name]() {
				std::function<void()> yesDepth2 = [scenario_name]() {
					NewScenario_Internal(scenario_name, true, true);
				};
				std::function<void()> noDepth2 = [scenario_name]() {
					NewScenario_Internal(scenario_name, false, true);
				};
				gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yesDepth2, noDepth2, IEnvironment::Environments::e_editor);
			};
			std::function<void()> no;
			gui::NewQuestionWindow("Overwrite scenario", yes, no, IEnvironment::Environments::e_editor); //TODO Translation!
		}
		else
		{
			std::function<void()> yes = [scenario_name]() {
				NewScenario_Internal(scenario_name, false, true);
			};
			std::function<void()> no;
			gui::NewQuestionWindow("Overwrite scenario?", yes, no, IEnvironment::Environments::e_editor); //TODO Translation!
		}
		return;
	}

	// just check for saving changes
	if (this->IsEdited())
	{
		std::function<void()> yes = [scenario_name]() {
			NewScenario_Internal(scenario_name, true, true);
		};
		std::function<void()> no = [scenario_name]() {
			NewScenario_Internal(scenario_name, false, true);
		};
		gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes, no, IEnvironment::Environments::e_editor);
	}
	else
	{
		NewScenario_Internal(scenario_name, false, true);
	}
}

void Adventure::OpenScenario(std::string& scenarioName)
{
	// starts a flow to open an existing scenario
	std::string scenario_name = std::trim_copy(scenarioName);
	Encode::ToLowercase(&scenario_name);

	// if the name inserted is empty/whitespace
	// do nothing but warn the user
	if (scenario_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string current_scenario = std::trim_copy(this->scenario->GetFolderName());
	Encode::ToLowercase(&current_scenario);

	// opening the current scenario?
	if (scenario_name == current_scenario)
	{
		gui::NewInfoWindow("Scenario already opened", IEnvironment::Environments::e_editor); //TODO Translation!
		return;
	}

	// just check for saving changes
	if (this->IsEdited())
	{
		std::function<void()> yes = [scenario_name]() {
			OpenScenario_Internal(scenario_name, true);
		};
		std::function<void()> no = [scenario_name]() {
			OpenScenario_Internal(scenario_name, false);
		};
		gui::NewQuestionWindow("e_text_save_adv_before_new_operation", yes, no, IEnvironment::Environments::e_editor);
	}
	else
	{
		OpenScenario_Internal(scenario_name, false);
	}
}

void Adventure::DeleteScenario(std::string& scenarioName)
{
	// starts a flow to delete an existing scenario

	std::string scenario_name = std::trim_copy(scenarioName);
	Encode::ToLowercase(&scenario_name);

	// if the name inserted is empty/whitespace
	// do nothing but warn the user
	if (scenario_name.empty())
	{
		gui::NewInfoWindow("e_text_empty_name", IEnvironment::Environments::e_editor);
		return;
	}

	std::string current_scenario = std::trim_copy(this->scenario->GetFolderName());
	Encode::ToLowercase(&current_scenario);

	//Deleting the current scenario?
	if (scenario_name == current_scenario)
	{
		if (this->numberOfScenarios == 1)
		{
			std::function<void()> yes = [scenarioName]() {
				NewScenario_Internal(scenarioName, true, false);
			};
			std::function<void()> no;
			gui::NewQuestionWindow("e_text_delete_last_scenario", yes, no, IEnvironment::Environments::e_editor);
		}
		else
		{
			std::function<void()> yes = [scenario_name]() {
				DeleteScenario_Internal(scenario_name, true);
			};
			std::function<void()> no;
			gui::NewQuestionWindow("e_text_delete_scenario_confirmation", yes, no, IEnvironment::Environments::e_editor);
		}
		return;
	}

	//If not, just delete the folder inside the .zip file
	else
	{
		std::function<void()> yes = [scenario_name]() {
			DeleteScenario_Internal(scenario_name, false);
		};
		std::function<void()> no;
		gui::NewQuestionWindow("e_text_delete_scenario_confirmation", yes, no, IEnvironment::Environments::e_editor);
	}
}

void Adventure::GenerateRandomScenario(const uint32_t seed)
{
	std::function<void()> yes = [seed]() {
		GenerateRandomScenario_Internal(seed);
	};
	std::function<void()> no;
	gui::NewQuestionWindow("e_text_generate_random_scenario", yes, no, IEnvironment::Environments::e_editor);
	return;
}

std::vector<std::string> Adventure::GetListOfValidScenariosWithDetails(void) const
{
	std::vector<std::string> output;

	//Check if the current adventure owns at least one valid scenario.
	std::vector<std::string> scenarios = rattlesmake::services::zip_service::get_instance().get_all_folders_within_folder(this->GetZipName(), "scenarios");
	for (auto& scenarioPath : scenarios)
	{
		if (Scenario::CheckIntegrity(this->GetZipName(), "scenarios/" + scenarioPath) == false)
			continue;
		output.push_back(scenarioPath); // todo
	}
	return output;
}

std::vector<std::string> Adventure::GetListOfValidScenarios(void) const
{
	std::vector<std::string> output;

	//Check if the current adventure owns at least one valid scenario.
	std::vector<std::string> scenarios = rattlesmake::services::zip_service::get_instance().get_all_folders_within_folder(this->GetZipName(), "scenarios");
	for (auto& scenarioPath : scenarios)
	{
		if (Scenario::CheckIntegrity(this->GetZipName(), "scenarios/" + scenarioPath) == false) 
			continue;
		output.push_back(scenarioPath);
	}
	return output;
}

void Adventure::LoadProperties(void)
{
	std::string xmlText = zipService.get_text_file(GetZipName(), "properties.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	this->properties = Properties(xmlFile.FirstChildElement("properties"));
}

void Adventure::LoadObjectives(void)
{
	std::string xmlText = zipService.get_text_file(GetZipName(), "objectives.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS) 
		return;

	this->objectives = ObjectiveList(xmlFile.FirstChildElement("objectiveArray"));
}

void Adventure::SaveAll(void)
{
	this->SaveAdventure(false);
	//TODO - ciclare su tutti gli scenari quando sarà possibile avere più di uno scenario
	this->scenario->SaveFromEditor(false); //Save scenario

	zipService.save_and_close(this->GetZipName());

	Logger::LogMessage msg = Logger::LogMessage("The adventure has been successfully saved with the following name: \"" + GetZipName() + "\"", "Info", "Game", "Adventure", __FUNCTION__);
	Logger::Info(msg);
}

void Adventure::SaveAdventure(const bool closeZip)
{
	if (this->GetZipName() == "NA") 
		return;

	//Creates a .zip file and add the respective content to it
	this->SetLastEditDate();
	{
		auto properties = this->properties.Serialize();
		auto objectives = this->objectives.Serialize();
		zipService.add_file(this->GetZipName(), "properties.xml", properties);
		zipService.add_file(this->GetZipName(), "objectives.xml", objectives);
	}

	if (closeZip)
		zipService.save_and_close(this->GetZipName());

	this->MarkAsNonEdited();
}

void Adventure::SaveAsBinaryFile(BinaryFileWriter& bfw)
{
	///If you change saving order here, go to Adventure::InitFromBinFile and impose the same loading order
	
	//TODO salvare players info, diplomazia, etc

	//Saving scenario
	this->scenario->SaveAsBinaryFile(bfw);
}

void Adventure::SetZipName(std::string newName)
{
	newName = std::remove_extension(newName);
	newName += ".zip";
	this->zipName = std::move(newName);
	if (this->scenario != nullptr) 
		this->scenario->zipKey = this->GetZipName();
	zipService.add_zipfile_shortcut(this->GetZipName(), this->GetZipPath());
}

std::string Adventure::GetZipName(void) const
{
	return (this->zipName == ".zip") ? "NA" : this->zipName;
}

std::string Adventure::GetZipPath(void) const
{
	return rattlesmake::services::file_service::get_instance().get_folder_shortcut("adventures") + this->zipName;
}

void Adventure::SetAdventureName(std::string newName)
{
	this->properties.adventureName = std::move(newName);
}

std::string Adventure::GetAdventureName(void) const
{
	return this->properties.adventureName;
}

void Adventure::SetAuthor(std::string author)
{
	this->properties.author = std::move(author);
}

std::string Adventure::GetAuthor(void) const
{
	return this->properties.author;
}

void Adventure::SetCreationDate(void)
{
	this->properties.creationDate = rattlesmake::services::file_service::get_instance().get_current_datetime("%d/%m/%Y");
}

std::string Adventure::GetCreationDate(void) const
{
	return this->properties.creationDate;
}

void Adventure::SetLastEditDate(void)
{
	this->properties.lastEditDate = rattlesmake::services::file_service::get_instance().get_current_datetime("%d/%m/%Y - %H:%M:%S");
}

std::string Adventure::GetLastEditDate(void) const
{
	return this->properties.lastEditDate;
}

void Adventure::SetDescription(std::string description)
{
	this->properties.description = std::move(description);
}

std::string Adventure::GetDescription(void) const
{
	return this->properties.description;
}

void Adventure::SetGameMode(uint8_t mode)
{
	this->properties.gameMode = mode;
}

uint8_t Adventure::GetGameMode(void) const
{
	return this->properties.gameMode;
}

void Adventure::SetStartingScenario(std::string scenario)
{
	this->properties.startingScenario = std::move(scenario);
}

std::string Adventure::GetStartingScenario(void) const
{
	return this->properties.startingScenario;
}

void Adventure::SetPlayersNumber(uint8_t number)
{
	this->properties.playersNumber = number;
}

uint8_t Adventure::GetPlayersNumber(void) const
{
	return this->properties.playersNumber;
}

void Adventure::MarkAsEdited(void)
{
	this->edited = true;
}

void Adventure::MarkAsNonEdited(void)
{
	this->edited = false;
}

bool Adventure::IsEdited(void) const
{
	return this->edited;
}

bool Adventure::IsOpened(void) const
{
	return (this->GetZipName() != "NA");
}

void Adventure::UnableToDelete(void)
{
	gui::NewInfoWindow("e_text_unable_to_delete_adventure", IEnvironment::Environments::e_editor);
}

std::shared_ptr<Scenario> Adventure::GetScenario(void) const
{
	assert(this->scenario != nullptr);
	return this->scenario;
}

Adventure::ObjectiveList* Adventure::GetObjectives(void)
{
	return &this->objectives;
}

void Adventure::ExecuteInternalMethods(void)
{
	// perform open scenario
	if (sOpenScenario_Internal.bActive == true)
	{
		OpenScenario_Internal(sOpenScenario_Internal.scenarioName, sOpenScenario_Internal.bSaveBefore);
	}
}

classesData_t Adventure::GetClassesDataSp(void) const noexcept
{
	return this->classesDataSP;
}

Adventure::Objective::Objective(tinyxml2::XMLElement* el)
{
	if (el == nullptr) 
		return;

	this->id = tinyxml2::TryParseFirstChildStrContent(el, "id");
	this->title = tinyxml2::TryParseFirstChildStrContent(el, "title");
	this->description = tinyxml2::TryParseFirstChildStrContent(el, "description");
	this->scenario = tinyxml2::TryParseFirstChildStrContent(el, "scenario");
	this->icon = tinyxml2::TryParseFirstChildStrContent(el, "icon");
	this->minimap = tinyxml2::TryParseFirstChildStrContent(el, "minimap") == "true";
	this->x = tinyxml2::TryParseIntAttribute(el->FirstChildElement("coordinates"), "x");
	this->y = tinyxml2::TryParseIntAttribute(el->FirstChildElement("coordinates"), "y");
}

Adventure::ObjectiveList::ObjectiveList(tinyxml2::XMLElement* el)
{
	if (el == NULL) 
		return;

	for (tinyxml2::XMLElement* obj = el->FirstChildElement(); obj != NULL; obj = obj->NextSiblingElement())
	{
		Objective* objective = new Objective(obj);
		this->objectives.push_back(objective);
	}
}

Adventure::Objective* Adventure::ObjectiveList::GetObjectiveById(std::string id)
{
	for (auto const& o : this->objectives)
	{
		if (o->id == id)
		{
			return o;
		}
	}

	return nullptr;
}

std::vector<std::string> Adventure::ObjectiveList::GetListOfObjectivesId(void)
{
	std::vector<std::string> l;
	for (auto const& o : this->objectives)
	{
		l.push_back(o->id);
	}
	return l;
}

bool Adventure::ObjectiveList::IsObjectiveIdAvailable(std::string id, Objective* o)
{
	for (auto const& ob : this->objectives)
	{
		if (ob == o) continue;
		if (ob->id == id) return false;
	}
	return true;
}

void Adventure::ObjectiveList::Clear()
{
	for (auto& o : this->objectives)
	{
		if (o != nullptr)
		{
			delete o;
			o = nullptr;
		}
	}
	this->objectives.clear();
}

void Adventure::ObjectiveList::RemoveObjectiveById(std::string id)
{
	int idToErase = -1;
	for (int i = 0; i < this->objectives.size(); i++)
	{
		if (this->objectives[i]->id == id)
		{
			idToErase = i;
			break;
		}
	}
	if (idToErase != -1)
	{
		if (this->objectives[idToErase] != nullptr)
		{
			delete this->objectives[idToErase];
			this->objectives[idToErase] = nullptr;
		}
		this->objectives.erase(this->objectives.begin() + idToErase);
	}
}

void Adventure::ObjectiveList::NewObjective()
{
	Objective* o = new Objective();
	int _id = 1;
	std::vector<std::string> l;
	for (auto const& o : this->objectives)
	{
		l.push_back(o->id);
	}
	std::string s = "";
	if (l.size() == 0)
		s = "Objective0"; //TODO Translation
	else
	{
		while (_id != l.size() + 1)
		{
			s = "Objective" + std::to_string(_id); //TODO Translation
			if (std::find(l.begin(), l.end(), s) != l.end())
				_id++;
			else
				break;
		}
	}
	o->id = s;
	this->objectives.push_back(o);
}

std::stringstream Adventure::ObjectiveList::Serialize(void) const
{
	std::stringstream xml{ "" };
	xml << "<objectiveArray>\n";

	for (auto const& ob : this->objectives)
	{
		xml << "\t<objective>\n";
		xml << "\t\t<id>" + ob->id + "</id>\n";
		xml << "\t\t<title>" + ob->title + "</title>\n";
		xml << "\t\t<description>" + ob->description + "</description>\n";
		xml << "\t\t<scenario>" + ob->scenario + "</scenario>\n";
		xml << "\t\t<icon>" + ob->icon + "</icon>\n";
		std::string minimapStr = ob->minimap ? "true" : "false";
		xml << "\t\t<minimap>" + minimapStr + "</minimap>\n";
		xml << "\t\t<coordinates x=\"" + std::to_string(ob->x) + "\" y=\"" + std::to_string(ob->y) + "\" />\n";
		xml << "\t</objective>\n";
	}

	xml << "</objectiveArray>";
	return xml;
}

Adventure::Properties::Properties(tinyxml2::XMLElement* el)
{
	if (el == nullptr) 
		return;

	this->adventureName = tinyxml2::TryParseFirstChildStrContent(el, "name");
	this->author = tinyxml2::TryParseFirstChildStrContent(el, "author");
	this->creationDate = tinyxml2::TryParseFirstChildStrContent(el, "creationDate");
	this->lastEditDate = tinyxml2::TryParseFirstChildStrContent(el, "lastEditDate");
	this->description = tinyxml2::TryParseFirstChildStrContent(el, "description");

	//Following properties must be loaded properly since otherwise adventure could crash or have unexpected behaviors.
	//Default scenario
	try
	{
		std::string s = tinyxml2::TryParseFirstChildStrContent(el, "startingScenario");
		if (s.empty() == false) this->startingScenario = std::move(s);
	}
	catch (...)
	{
		gui::NewInfoWindow("e_text_missing_starting_scenario", IEnvironment::Environments::e_editor);
	}

	//Game Mode
	try
	{
		this->gameMode = tinyxml2::TryParseFirstChildIntContent(el, "gameMode");
	}
	catch (...)
	{
		gui::NewInfoWindow("e_text_missing_game_mode", IEnvironment::Environments::e_editor);
	}

	//Players number
	try
	{
		this->playersNumber = tinyxml2::TryParseFirstChildIntContent(el, "playersNumber");
	}
	catch (...)
	{
		gui::NewInfoWindow("e_text_missing_players_number", IEnvironment::Environments::e_editor);
	}

	//Diplomacy
	try
	{
		tinyxml2::XMLElement* _players = el->FirstChildElement("diplomacy");
		if (_players == nullptr)
			return; //Va lanciata l'eccezione

		for (tinyxml2::XMLElement* _player = _players->FirstChildElement("player"); _player != NULL; _player = _player->NextSiblingElement())
		{
			uint8_t player = tinyxml2::TryParseIntAttribute(_player, "id");
			for (tinyxml2::XMLElement* _otherPlayer = _player->FirstChildElement("otherPlayer"); _otherPlayer != NULL; _otherPlayer = _otherPlayer->NextSiblingElement())
			{
				uint8_t otherPlayer = tinyxml2::TryParseIntAttribute(_otherPlayer, "id");
				Diplomacy::SetCeaseFire(player, otherPlayer, tinyxml2::TryParseFirstChildIntContent(_otherPlayer, "ceaseFire"));
				Diplomacy::SetShareSupport(player, otherPlayer, tinyxml2::TryParseFirstChildIntContent(_otherPlayer, "shareSupport"));
				Diplomacy::SetShareView(player, otherPlayer, tinyxml2::TryParseFirstChildIntContent(_otherPlayer, "shareView"));
				Diplomacy::SetShareControl(player, otherPlayer, tinyxml2::TryParseFirstChildIntContent(_otherPlayer, "shareControl"));
			}
		}
	}
	catch (...)
	{
		gui::NewInfoWindow("e_text_missing_diplomacy", IEnvironment::Environments::e_editor);
	}
}

std::stringstream Adventure::Properties::Serialize(void) const
{
	std::stringstream xml{ "" };

	xml << "<properties>\n";
	xml << "\t<name>" + this->adventureName + "</name>\n";
	xml << "\t<author>" + this->author + "</author>\n";
	xml << "\t<creationDate>" + this->creationDate + "</creationDate>\n";
	xml << "\t<lastEditDate>" + this->lastEditDate + "</lastEditDate>\n";
	xml << "\t<description>" + this->description + "</description>\n";
	xml << "\t<startingScenario>" + this->startingScenario + "</startingScenario>\n";
	xml << "\t<gameMode>" + std::to_string(this->gameMode) + "</gameMode>\n";
	xml << "\t<playersNumber>" + std::to_string(this->playersNumber) + "</playersNumber>\n";
	xml << "\t<diplomacy>\n";
	for (uint8_t i = 1; i <= this->playersNumber; i++)
	{
		xml << "\t\t<player id=\"" + std::to_string(i) + "\">\n";
		for (uint8_t j = 1; j <= this->playersNumber; j++)
		{
			if (j != i)
			{
				xml << "\t\t\t<otherPlayer id=\"" + std::to_string(j) + "\">\n";
				xml << "\t\t\t\t<ceaseFire>" + std::to_string(Diplomacy::GetCeaseFire(i, j)) + "</ceaseFire>\n";
				xml << "\t\t\t\t<shareSupport>" + std::to_string(Diplomacy::GetShareSupport(i, j)) + "</shareSupport>\n";
				xml << "\t\t\t\t<shareView>" + std::to_string(Diplomacy::GetShareView(i, j)) + "</shareView>\n";
				xml << "\t\t\t\t<shareControl>" + std::to_string(Diplomacy::GetShareControl(i, j)) + "</shareControl>\n";
				xml << "\t\t\t</otherPlayer>\n";
			}
		}
		xml << "\t\t</player>\n";
	}
	xml << "\t</diplomacy>\n";
	xml << "</properties>";

	return xml;
}
