#include "pyservice.h"

#include <engine.h>
#include <settings.h>

#include <environments/file_save_info.h>
#include <environments/menu/menu.h>
#include <environments/game/match/match.h>
#include <environments/game/editor/editor.h>
#include <environments/game/adventure/scenario/surface/randommap/randomMapGenerator.h>
#include <environments/game/adventure/scenario/surface/surfaceSettings.h>
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/unit.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/wagon.h>
#include <environments/game/classes/objectsSet/selected_objects.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/objectsSet/ordered_units_list.h>

#include <imgui.h>
#include <iframe/imgui_elements_include.h>  // Includes all ImGui elements

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

#include <memory>

#include <keyboard.h>

#include <GLFW/glfw3.h> // must be the last

namespace py = pybind11;

#pragma region BIND EMBEDDED MODULES

PYBIND11_EMBEDDED_MODULE(admin, m)
{
	py::class_<Engine, std::unique_ptr<Engine, py::nodelete>>(m, "Engine")
		.def("DeleteImage", &Engine::DeleteImage)
		.def("GameClose", &Engine::GameClose)
		.def("GetAllRacesNames", &Engine::GetAllRacesNames)
		.def("GetAvailableLanguages", &Engine::GetAvailableLanguages)
		.def("GetEnvironment", &Engine::GetEnvironment, py::return_value_policy::reference)
		.def("GetListOfSaveFiles", &Engine::GetListOfSaveFiles)
		.def("GetListOfValidAdventures", &Engine::GetListOfValidAdventures)
		.def("GetListOfValidAdventuresWithDetails", &Engine::GetListOfValidAdventuresWithDetails)
		.def("GetScenarioSizes", &Engine::GetScenarioSizes)
		.def("GetScenarioTypes", &Engine::GetScenarioTypes)
		.def("GetViewportHeight", &Engine::GetViewportHeight)
		.def("GetViewportWidth", &Engine::GetViewportWidth)
		.def("IsKeyPressed", &Engine::IsKeyPressed)
		.def("LoadImage", &Engine::LoadImage)
		.def("OpenEnvironment", &Engine::OpenEnvironment)
		.def("Translate", &Engine::Translate)
		.def("TranslateWords", &Engine::TranslateWords)
		;

#pragma region Preferences & Settings
	py::class_<PlayersSettings::PlayerSettings, std::shared_ptr<PlayersSettings::PlayerSettings>>(m, "PlayerSettings")
		.def_readwrite("active", &PlayersSettings::PlayerSettings::Active)
		.def_readwrite("name", &PlayersSettings::PlayerSettings::Name)
		.def_readwrite("race", &PlayersSettings::PlayerSettings::Race)
		;

	py::class_<PlayersSettings, std::unique_ptr<PlayersSettings, py::nodelete>>(m, "PlayersSettings")
		.def("ResetToDefault", &PlayersSettings::ResetToDefault)
		.def("__getitem__", &PlayersSettings::operator[], py::return_value_policy::reference)
		;

	py::class_<RandomMapSettings, std::unique_ptr<RandomMapSettings, py::nodelete>>(m, "RandomMapSettings")
		.def_readwrite("scenarioSize", &RandomMapSettings::ScenarioSize)
		.def_readwrite("scenarioType", &RandomMapSettings::ScenarioType)
		;

	py::class_<GlobalPreferences, std::unique_ptr<GlobalPreferences, py::nodelete>>(m, "GlobalPreferences")
		.def_property("language", &GlobalPreferences::GetLanguage, &GlobalPreferences::SetLanguage)
		;

	py::class_<MatchPreferences, std::unique_ptr<MatchPreferences, py::nodelete>>(m, "MatchPreferences")
		.def_property_readonly("playersSettings", &MatchPreferences::GetPlayersSettings, py::return_value_policy::reference)
		.def_property_readonly("randomMapSettings", &MatchPreferences::GetRandomMapSettings, py::return_value_policy::reference)
		;

	py::class_<EditorPreferences, std::unique_ptr<EditorPreferences, py::nodelete>>(m, "EditorPreferences")
		.def_property_readonly("randomMapSettings", &EditorPreferences::GetRandomMapSettings, py::return_value_policy::reference)
		;

	py::class_<Settings, std::unique_ptr<Settings, py::nodelete>>(m, "Settings")
		.def_property_readonly("editorPreferences", &Settings::GetEditorPreferences, py::return_value_policy::reference)
		.def_property_readonly("matchPreferences", &Settings::GetMatchPreferences, py::return_value_policy::reference)
		.def_property_readonly("globalPreferences", &Settings::GetGlobalPreferences, py::return_value_policy::reference)
		;
#pragma endregion

	py::class_<Menu, std::unique_ptr<Menu, py::nodelete>>(m, "Menu")
		.def("OpenPage", &Menu::OpenPage)
		.def("Load", &Menu::Load)
		.def("GetFileSaveInfo", &Menu::GetFileSaveInfo)
		.def_property_readonly("iframes", &Menu::GetIframes, py::return_value_policy::reference)
		;

	py::class_<IGame, std::unique_ptr<IGame, py::nodelete>>(m, "IGame")
		.def("Quit", &IGame::Quit)
		.def("GetUI", &IGame::GetUIRef, py::return_value_policy::reference)
		.def_property_readonly("iframes", &IGame::GetIframes, py::return_value_policy::reference)
		;

	py::class_<Match, IGame, std::unique_ptr<Match, py::nodelete>>(m, "Match")
		.def("IsLoading", &Match::IsLoading)
		.def("Load", &Match::Load)
		.def("PlaceObject", &Match::PlaceObject)
		.def("Save", &Match::Save)
		.def("GetFileSaveInfo", &Match::GetFileSaveInfo)
		.def_property_readonly("scenario", &Match::GetCurrentScenario, py::return_value_policy::reference)
		;

	py::class_<Editor, IGame, std::unique_ptr<Editor, py::nodelete>>(m, "Editor")
		.def("OpenAdventure", &Editor::OpenAdventure)
		.def("NewAdventure", &Editor::NewAdventure)
		.def("SaveAdventureAs", &Editor::SaveAdventureAs)
		.def("SaveAdventureCopy", &Editor::SaveAdventureCopy)
		.def("DeleteAdventure", &Editor::DeleteAdventure)
		.def("RunAdventure", &Editor::RunAdventure)
		.def("InsertObject", &Editor::InsertObject)
		.def("ChangeTerrainType", &Editor::ChangeTerrainType)
		.def("GetEditorTerrainTreeList1", &Editor::GetEditorTerrainTreeList1)
		.def("GetEditorTerrainTreeList2", &Editor::GetEditorTerrainTreeList2)
		.def("CheckOpenObjectPropertiesCondition", &Editor::CheckOpenObjectPropertiesCondition)
		.def("OpenObjectProperties", &Editor::OpenObjectProperties)
		.def_property_readonly("adventure", &Editor::GetCurrentAdventure, py::return_value_policy::reference)
		.def_property_readonly("scenario", &Editor::GetCurrentScenario, py::return_value_policy::reference)
		;

	py::class_<IGameUI, std::unique_ptr<IGameUI, py::nodelete>>(m, "IGameUI")
		.def("NewQuestionWindow", &IGameUI::NewQuestionWindow)
		.def("NewInfoWindow", &IGameUI::NewInfoWindow)
		.def_property_readonly("infoText", &IGameUI::GetInfoText)
		;

	//py::class_<EditorUI, IGameUI, std::unique_ptr<EditorUI, py::nodelete>>(m, "EditorUI");
	//py::class_<MatchUI, IGameUI, std::unique_ptr<MatchUI, py::nodelete>>(m, "MatchUI");

	py::class_<Adventure, std::unique_ptr<Adventure, py::nodelete>>(m, "Adventure")
		.def_property_readonly("zipName", &Adventure::GetZipName)
		.def_property("name", &Adventure::GetAdventureName, &Adventure::SetAdventureName)
		.def_property("author", &Adventure::GetAuthor, &Adventure::SetAuthor)
		.def_property("description", &Adventure::GetDescription, &Adventure::SetDescription)
		.def_property("startingScenario", &Adventure::GetStartingScenario, &Adventure::SetStartingScenario)
		.def_property("playersNumber", &Adventure::GetPlayersNumber, &Adventure::SetPlayersNumber)
		.def("Save", &Adventure::SaveAll)
		.def("IsEdited", &Adventure::IsEdited)
		.def("IsOpened", &Adventure::IsOpened)
		.def("MarkAsEdited", &Adventure::MarkAsEdited)
		.def("MarkAsNonEdited", &Adventure::MarkAsNonEdited)
		.def("NewScenario", &Adventure::NewScenario)
		.def("OpenScenario", &Adventure::OpenScenario)
		.def("DeleteScenario", &Adventure::DeleteScenario)
		.def("GenerateRandomScenario", &Adventure::GenerateRandomScenario)
		.def("GetListOfValidScenariosWithDetails", &Adventure::GetListOfValidScenariosWithDetails)
		.def("GetListOfValidScenarios", &Adventure::GetListOfValidScenarios)
		;

	py::class_<Scenario, std::unique_ptr<Scenario, py::nodelete>>(m, "Scenario")
		.def_property_readonly("folderName", &Scenario::GetFolderName)
		.def_property_readonly("size", &Scenario::GetScenarioSize)
		.def_property_readonly("areaArray", &Scenario::GetAreaArrayPtr, py::return_value_policy::reference)
		.def_property_readonly("playersArray", &Scenario::GetPlayersArray, py::return_value_policy::reference)
		.def_property("name", &Scenario::GetName, &Scenario::SetName)
		.def("ToggleGrid", &Scenario::ToggleGrid)
		.def("ToggleHitboxes", &Scenario::ToggleHitboxRendering)
		.def("ToggleWireframe", &Scenario::ToggleWireframe)
		.def("ToggleClouds", &Scenario::ToggleClouds)
		.def("ToggleSea", &Scenario::ToggleSea)
		.def("ToggleTracingDebugging", &Scenario::ToggleTracingDebugging)
		.def("ToggleDrawWithoutNoise", &Scenario::ToggleDrawWithoutNoise)
		;

	py::class_<AreaArray, std::unique_ptr<AreaArray, py::nodelete>>(m, "AreaArray")
		.def("EnableAreaDrawing", &AreaArray::EnableAreaDrawing)
		.def("EnableAreaGizmo", &AreaArray::EnableAreaGizmo)
		.def("DisableAreaDrawing", &AreaArray::DisableAreaDrawing)
		.def("EnableAreasRendering", &AreaArray::EnableAreasRendering)
		.def("DisableAreasRendering", &AreaArray::DisableAreasRendering)
		.def("RemoveArea", &AreaArray::RemoveArea)
		.def_property_readonly("names", &AreaArray::GetNames)
		;

	py::class_<PlayersArray, std::unique_ptr<PlayersArray, py::nodelete>>(m, "PlayersArray")
		.def_property_readonly("maxAllowedPlayers", &PlayersArray::GetAllowedPlayersNumber)
		.def_property_readonly("players", &PlayersArray::GetPlayersArrayRef)
		.def_property("numberOfPlayers", &PlayersArray::GetEffectiveNumberOfPlayers, &PlayersArray::SetEffectiveNumberOfPlayers)
		;

	py::class_<FileSaveInfo>(m, "FileSaveInfo")
		.def_property_readonly("time", &FileSaveInfo::GetTime)
		.def_property_readonly("difficulty", &FileSaveInfo::GetDifficulty)
		.def_property_readonly("playersNumber", &FileSaveInfo::GetPlayersNumber)
		;

	py::class_<Player, std::shared_ptr<Player>>(m, "Player")
		.def_property_readonly("name", &Player::GetName)
		.def_property_readonly("race", &Player::GetRace)
		.def_property("color", &Player::GetColor, &Player::SetColor)
		;

#pragma region GObjects family attributes
	py::class_<PlayableAttributes, std::shared_ptr<PlayableAttributes>>(m, "PlayableAttributes")
		.def("GetIconName", &PlayableAttributes::GetIconName)
		.def_property_readonly("sight", &PlayableAttributes::GetSight)
		;

	py::class_<BuildingAttributes, std::shared_ptr<BuildingAttributes>>(m, "BuildingAttributes")
		.def_property_readonly("maxHealth", &BuildingAttributes::GetMaxHealth)
		.def_property_readonly("health", &BuildingAttributes::GetHealth)
		;

	py::class_<UnitAttributes, std::shared_ptr<UnitAttributes>>(m, "UnitAttributes")
		.def_property_readonly("pluralName", &UnitAttributes::GetPluralName)
		.def_property_readonly("damageType", &UnitAttributes::GetDamageTypeStr)
		.def_property_readonly("maxFood", &UnitAttributes::GetMaxFood)
		.def_property_readonly("maxHealth", &UnitAttributes::GetMaxHealth)
		.def_property_readonly("maxStamina", &UnitAttributes::GetMaxStamina)
		.def_property_readonly("range", &UnitAttributes::GetRange)

		.def_property("armorPierce", &UnitAttributes::GetArmorPierce, &UnitAttributes::SetArmorPierce)
		.def_property("armorSlash", &UnitAttributes::GetArmorSlash, &UnitAttributes::SetArmorSlash)
		.def_property("food", &UnitAttributes::GetFood, &UnitAttributes::SetFood)
		.def_property("level", &UnitAttributes::GetLevel, &UnitAttributes::SetLevel)
		.def_property("minAttack", &UnitAttributes::GetMinAttack, &UnitAttributes::SetMinAttack)
		.def_property("maxAttack", &UnitAttributes::GetMaxAttack, &UnitAttributes::SetMaxAttack)
		.def_property("speed", &UnitAttributes::GetSpeed, &UnitAttributes::SetSpeed)
		.def_property("stamina", &UnitAttributes::GetStamina, &UnitAttributes::SetStamina)
		;

	py::class_<SettlementAttributes, std::shared_ptr<SettlementAttributes>>(m, "SettlementAttributes")
		.def_property("food", &SettlementAttributes::GetFood, &SettlementAttributes::SetFood)
		.def_property("gold", &SettlementAttributes::GetGold, &SettlementAttributes::SetGold)
		.def_property("loyalty", &SettlementAttributes::GetLoyalty, &SettlementAttributes::SetLoyalty)
		.def_property("population", &SettlementAttributes::GetPopulation, &SettlementAttributes::SetPopulation)
		.def_property("maxPopulation", &SettlementAttributes::GetMaxPopulation, &SettlementAttributes::SetMaxPopulation)
		;
#pragma endregion

#pragma region Classes:
	py::class_<GObject, std::shared_ptr<GObject>>(m, "Object")
		// readonly properties
		.def_property_readonly("className", &GObject::GetClassName)
		.def_property_readonly("id", &GObject::GetUniqueID)
		.def_property_readonly("position", &GObject::GetPosition)

		// methods
		.def("IsBuilding", &GObject::IsBuilding)
		.def("IsDecoration", &GObject::IsDecoration)
		.def("IsDruid", &GObject::IsDruid)
		.def("IsHero", &GObject::IsHero)
		.def("IsSimpleUnit", &GObject::IsSimpleUnit)
		.def("IsUnit", &GObject::IsUnit)
		.def("IsHeirOf", &GObject::IsHeirOf)
		.def("Select", &GObject::Select)
		.def("Deselect", &GObject::Deselect)
		.def("GlobalDeselect", &GObject::GlobalDeselect)
		.def("IsSelected", &GObject::IsSelected)
		.def("DistTo", &GObject::DistTo)
		;

	py::class_<Playable, GObject, PlayableAttributes, std::shared_ptr<Playable>>(m, "Playable")
		// get/set properties
		.def_property("displayedName", &Playable::GetDisplayedName, &Playable::SetDisplayedName)
		.def_property("idName", &Playable::GetIDName, &Playable::SetIDName)
		.def_property("player", &Playable::GetPlayer, &Playable::SetPlayer)

		.def("GetMyTarget", &Playable::GetMyTarget)
		.def("GetDefaultPropertyValue", &Playable::GetDefaultIntAttributesValue)
		.def("GetNumberOfCommands", &Playable::GetNumberOfCommands)
		.def("GetCommandById", &Playable::GetCommandById)
		.def("AsTarget", &Playable::AsTarget)
		;

	py::class_<Building, Playable, BuildingAttributes, std::shared_ptr<Building>>(m, "Building")
		// readonly properties
		.def_property_readonly("exitPoint", &Building::GetExitPoint)
		
		//methods
		.def("Damage", &Building::Damage)
		.def("GetGarrison", &Building::GetGarrison)
		.def("GetSettlement", &Building::GetSettlement)
		.def("HasGarrison", &Building::HasGarrison)
		.def("IsBroken", &Building::IsBroken)
		.def("IsCentralBuilding", &Building::IsCentralBuilding)
		.def("IsFirstBuilding", &Building::IsFirstBuilding)
		.def("Repair", &Building::Repair)
		.def("SetCommandWithTarget", &Building::SetCommandWithTarget)
		;

	py::class_<Unit, Playable, UnitAttributes, std::shared_ptr<Unit>>(m, "Unit")
		// readonly properties
		.def_property_readonly("hostBuilding", &Unit::GetHostBuilding)

		// get/set properties
		.def_property("health", &Unit::GetHealth, &Unit::SetHealth)
		.def_property("percHealth", &UnitAttributes::GetPercentHealth, &Unit::SetHealthByPercHealth)

		// methods
		.def("Damage", &Unit::Damage)
		.def("Detach", &Unit::Detach)
		.def("GetHero", &Unit::GetHero)
		.def("GoTo", &Unit::GoTo)
		.def("GoToApproach", &Unit::GoToApproach)
		.def("GoToAttack", &Unit::GoToAttack)
		.def("GoToConquer", &Unit::GoToConquer)
		.def("GoToEnter", &Unit::GoToEnter)
		.def("IsDead", &Unit::IsDead)
		.def("IsEntering", &Unit::IsEntering)
		.def("IsInHolder", &Unit::IsInHolder)
		.def("IsRanged", &Unit::IsRanged)
		.def("SetCommandWithoutTarget", &Unit::SetCommandWithoutTarget)
		.def("SetCommandWithTarget", &Unit::SetCommandWithTarget)

		//Old - TODO
		.def("IsSelected", &Unit::IsSelected)
		.def("Stop", &Unit::Stop)
		;

	py::class_<Hero, Unit, std::shared_ptr<Hero>>(m, "Hero")
		// methods
		.def("GetArmy", &Hero::GetArmy)
		.def("KeepFormationMoving", &Hero::KeepFormationMoving)
		.def("LeadsUnit", &Hero::LeadsUnit)
		;
#pragma endregion

#pragma region GObjectsSet
	py::class_<Settlement, SettlementAttributes, std::shared_ptr<Settlement>>(m, "Settlement")
		.def_property_readonly("numberOfBuildings", &Settlement::GetNumberOfBuildings)

		// get/set properties
		.def_property("name", &Settlement::GetSettlementName, &Settlement::SetSettlementName)
		.def_property("player", &Settlement::GetPlayer, &Settlement::SetPlayer)

		.def("GetBuildings", &Settlement::GetBuildings)
		.def("GetFirstBuilding", &Settlement::GetFirstBuilding)
		.def("GetDefaultPropertyValue", &Settlement::GetDefaultIntAttributesValue)
		;

	py::class_<OrderedUnitsList, std::shared_ptr<OrderedUnitsList>>(m, "OrderedUnitsList")
		//Methods
		.def("Contains", &OrderedUnitsList::Contains)
		.def("GetTroops", &OrderedUnitsList::GetTroops)
		;

	py::class_<Garrison, OrderedUnitsList, std::shared_ptr<Garrison>>(m, "Garrison")
		//get/set properties
		.def_property_readonly("maxNumberOfUnits", &Garrison::GetMaxNumberOfUnits)
		.def_property_readonly("numberOfUnits", &Garrison::GetNumberOfUnits)
		;

	py::class_<Army, OrderedUnitsList, std::shared_ptr<Army>>(m, "Army")
		//get/set properties
		.def_property_readonly("maxNumberOfUnits", &Army::GetMaxNumberOfUnits)
		.def_property_readonly("numberOfUnits", &Army::GetNumberOfUnits)

		//Methods
		.def("GetTroopsInTheSameBuilding", &Army::GetTroopsInTheSameBuilding)
		.def("IsFull", &Army::IsFull)
		;

	//For now, python doesn't consider SelectedObjects as a child of OrderedUnitsList.
	py::class_<SelectedObjects, std::shared_ptr<SelectedObjects>>(m, "SelectedObjects")
		.def_property_readonly("armorPierce", &SelectedObjects::GetArmorPierce)
		.def_property_readonly("armorSlash", &SelectedObjects::GetArmorSlash)
		.def_property_readonly("food", &SelectedObjects::GetFood)
		.def_property_readonly("health", &SelectedObjects::GetHealth)
		.def_property_readonly("level", &SelectedObjects::GetLevel)
		.def_property_readonly("minAttack", &SelectedObjects::GetMinAttack)
		.def_property_readonly("maxAttack", &SelectedObjects::GetMaxAttack)
		.def_property_readonly("maxFood", &SelectedObjects::GetMaxFood)
		.def_property_readonly("maxHealth", &SelectedObjects::GetMaxHealth)
		.def_property_readonly("maxStamina", &SelectedObjects::GetMaxStamina)
		.def_property_readonly("percHealth", &SelectedObjects::GetPercHealth)
		.def_property_readonly("percStamina", &SelectedObjects::GetPercStamina)
		.def_property_readonly("range", &SelectedObjects::GetRange)
		.def_property_readonly("stamina", &SelectedObjects::GetStamina)
		.def_property_readonly("sight", &SelectedObjects::GetSight)
		.def_property_readonly("speed", &SelectedObjects::GetSpeed)

		.def("IsRanged", &SelectedObjects::IsRanged)
		;

	py::class_<ObjsList, std::shared_ptr<ObjsList>>(m, "ObjsList")
		.def("Contains", &ObjsList::Contains)
		.def("Count", &ObjsList::Count)
		.def("CountByClass", &ObjsList::CountByClass)
		.def("Get", &ObjsList::Get)
		.def("GetNumberOfDifferentClasses", &ObjsList::GetNumberOfDifferentClasses)
		.def("IsEmpty", &ObjsList::IsEmpty)
		.def("Select", &ObjsList::Select)
		.def("SelectByClass", &ObjsList::SelectByClass)
		.def("SelectHero", &ObjsList::SelectHero)
		.def("SetCommandWithTarget", &ObjsList::SetCommandWithTarget)
		.def("SetHealthByPercHealth", &ObjsList::SetHealthByPercHealth)
		.def("SetLevel", &ObjsList::SetLevel)
		.def("SetPlayer", &ObjsList::SetPlayer)
		.def("SetStaminaByPercStamina", &ObjsList::SetStaminaByPercStamina)
		;
#pragma endregion

#pragma region GObjectsStuff
	py::class_<Target, std::shared_ptr<Target>>(m, "Target")
		.def_property_readonly("object", &Target::GetObject)
		.def_property_readonly("point", &Target::GetPoint)
		;

	py::class_<Point, std::shared_ptr<Point>>(m, "Point")
		.def_property("x", &Point::GetX, &Point::SetX)
		.def_property("y", &Point::GetY, &Point::SetY)
		;

	py::class_<Command, std::shared_ptr<Command>>(m, "Command")

		// readonly properties
		.def_property_readonly("staminaCost", &Command::GetStaminaCost)
		.def_property_readonly("goldCost", &Command::GetGoldCost)
		.def_property_readonly("foodCost", &Command::GetFoodCost)
		.def_property_readonly("populationCost", &Command::GetPopulationCost)

		// methods
		.def("IsValid", &Command::IsValid)
		.def("SetRollover", &Command::SetRollover)
		.def("GetRollover", &Command::GetRollover)
		;
#pragma endregion

#pragma region Iframe and (Im)GuiElements
	py::class_<gui::ImGuiElement, std::shared_ptr<gui::ImGuiElement>>(m, "GuiElement")
		// read only properties
		.def_property_readonly("active", &gui::ImGuiElement::IsActive)
		.def_property_readonly("id", &gui::ImGuiElement::GetId)
		.def_property_readonly("text", &gui::ImGuiElement::GetText)
		// properties
		.def_property("enabled", &gui::ImGuiElement::IsEnabled, &gui::ImGuiElement::SetEnableValue) 
		.def_property("hidden", &gui::ImGuiElement::IsHidden, &gui::ImGuiElement::SetHiddenValue)
		.def_property("position", &gui::ImGuiElement::GetPosition_Py, &gui::ImGuiElement::SetPosition_Py)

		// methods
		.def("SetPlaceholder", &gui::ImGuiElement::SetPlaceholder)
		.def("SetText", &gui::ImGuiElement::SetTextWithTranslation)
		;

	py::class_<gui::ImGuiInputInt, gui::ImGuiElement, std::shared_ptr<gui::ImGuiInputInt>>(m, "GuiInputInt")
		// properties
		.def_property("value", &gui::ImGuiInputInt::GetValue, &gui::ImGuiInputInt::SetValue)
		
		// methods
		.def("SetMinMax", &gui::ImGuiInputInt::SetMinMax)
		;

	py::class_<gui::ImGuiImage, gui::ImGuiElement, std::shared_ptr<gui::ImGuiImage>>(m, "GuiImage")
		// methods
		.def("SetImage", &gui::ImGuiImage::SetImage)
		;

	py::class_<gui::ImGuiImageButton, gui::ImGuiElement, std::shared_ptr<gui::ImGuiImageButton>>(m, "GuiButton")
		// methods
		.def("SetImage", &gui::ImGuiImageButton::SetImage)
		;

	py::class_<gui::ImGuiMultipleChoice, gui::ImGuiElement, std::shared_ptr<gui::ImGuiMultipleChoice>>(m, "GuiMultipleChoice")
		// properties
		.def_property("selectedIndex", &gui::ImGuiMultipleChoice::GetSelectedIndex, &gui::ImGuiMultipleChoice::SetSelectedIndex)
		;

	py::class_<gui::ImGuiTextInput, gui::ImGuiElement, std::shared_ptr<gui::ImGuiTextInput>>(m, "GuiTextInput")
		// methods
		.def("Reset", &gui::ImGuiTextInput::Reset)
		;

	py::class_<gui::ImGuiTextInputMultiline, gui::ImGuiTextInput, std::shared_ptr<gui::ImGuiTextInputMultiline>>(m, "GuiTextInputMultiline")
		// properties
		.def_property_readonly("line", &gui::ImGuiTextInputMultiline::GetCursorLine)
		.def_property_readonly("column", &gui::ImGuiTextInputMultiline::GetCursorColumn)
		;

	py::class_<gui::ImGuiElementWithOptions, gui::ImGuiElement, std::shared_ptr<gui::ImGuiElementWithOptions>>(m, "GuiElementWithOptions")
		
		// properties
		.def_property_readonly("selectedText", &gui::ImGuiElementWithOptions::GetSelectedOption)
		.def_property_readonly("selectedIndex", &gui::ImGuiElementWithOptions::GetSelectedOptionIndex)

		// methods
		.def("GetOptions", &gui::ImGuiElementWithOptions::GetListOfStrings)
		.def("ResetSelectedOption", &gui::ImGuiElementWithOptions::ResetSelectedOption)
		.def("ResetOptions", &gui::ImGuiElementWithOptions::ResetOptions)
		.def("UpdateOptions", &gui::ImGuiElementWithOptions::UpdateOptions)
		;
	
	py::class_<gui::ImGuiCheckBox, gui::ImGuiElement, std::shared_ptr<gui::ImGuiCheckBox>>(m, "GuiCheckBox")
		// Properties
		.def_property("checked", &gui::ImGuiCheckBox::IsChecked, &gui::ImGuiCheckBox::SetChecked)
		;

	py::class_<gui::ImGuiComboBox, gui::ImGuiElementWithOptions, std::shared_ptr<gui::ImGuiComboBox>>(m, "GuiComboBox")
		;

	py::class_<gui::ImGuiTextList, gui::ImGuiElementWithOptions, std::shared_ptr<gui::ImGuiTextList>>(m, "GuiTextList")
		;

	py::class_<gui::ImGuiBufferingBar, gui::ImGuiElement, std::shared_ptr<gui::ImGuiBufferingBar>>(m, "GuiBufferingBar")
		// Properties
		.def_property("progress", &gui::ImGuiBufferingBar::GetProgress, &gui::ImGuiBufferingBar::SetProgress)
		;

	py::class_<gui::ImGuiProgressBar, gui::ImGuiElement, std::shared_ptr<gui::ImGuiProgressBar>>(m, "GuiProgressBar")
		// Properties
		.def_property("progress", &gui::ImGuiProgressBar::GetProgress, &gui::ImGuiProgressBar::SetProgress)
		;

	py::class_<gui::ImGuiSlider, gui::ImGuiElement, std::shared_ptr<gui::ImGuiSlider>>(m, "GuiSlider")
		// Properties
		.def_property("minValue", &gui::ImGuiSlider::GetMinValue, &gui::ImGuiSlider::SetMinValue)
		.def_property("maxValue", &gui::ImGuiSlider::GetMaxValue, &gui::ImGuiSlider::SetMaxValue)
		.def_property("value", &gui::ImGuiSlider::GetValue, &gui::ImGuiSlider::SetValue)
		;

	py::class_<gui::Iframe, std::shared_ptr<gui::Iframe>>(m, "Iframe")		
		.def("__getitem__", &gui::Iframe::operator[], py::return_value_policy::reference)  // A slightly slower but stylistically clearer alternative to GetElementByTagAndId
		.def("CheckIfElementExistsByTagAndId", &gui::Iframe::CheckIfElementExistsByTagAndId_Py)
		.def("Close", &gui::Iframe::Close)
		
		.def("GetCurrentElementId", &gui::Iframe::GetCurrentElementId)
		.def("GetElementByTagAndId", &gui::Iframe::GetElementByTagAndId_Py)
		.def("GetId", &gui::Iframe::GetId)
		.def("GetTabByIndex", &gui::Iframe::GetTabByIndex)
		.def("IsOpened", &gui::Iframe::IsOpened)
		.def("Open", &gui::Iframe::Open)
		.def("GetSkinName", &gui::Iframe::GetSkinName)
		
		// The following methods are required to dinamically morph all iframes, according to the game resolution
		.def_property("position", &gui::Iframe::GetPosition_Py, &gui::Iframe::SetPosition_Py)
		.def_property("size", &gui::Iframe::GetSize_Py, &gui::Iframe::SetSize_Py)
		;
#pragma endregion

	// EXCEPTIONS
	py::register_exception<SecurityLevelException>(m, "SecurityLevelException");

#pragma endregion

	m.def("GetEngine", &Engine::GetInstance, py::return_value_policy::reference);
	m.def("GetSettings", &Settings::GetInstance, py::return_value_policy::reference);
	m.def("GetSurfaceSettings", &SurfaceSettings::GetInstance, py::return_value_policy::reference);

	// global constants
	// TODO - creare classe contenente costanti (queste variabili su python non sono constanti!)
	m.attr("MAX_LEVEL_VALUE") = MAX_LEVEL_VALUE;
	m.attr("MAX_ATTACK_VALUE") = MAX_ATTACK_VALUE;
	m.attr("MAX_ARMOR_VALUE") = MAX_ARMOR_VALUE;
	m.attr("MAX_SPEED_VALUE") = MAX_SPEED_VALUE;
	m.attr("MAX_STAMINA_VALUE") = MAX_STAMINA_VALUE;
	m.attr("MAX_MANA_VALUE") = MAX_MANA_VALUE;
	m.attr("MAX_GOLD_CAPACITY") = MAX_GOLD_CAPACITY;
	m.attr("MAX_FOOD_CAPACITY") = MAX_FOOD_CAPACITY;
	m.attr("GOLD_LIMIT") = GOLD_LIMIT;
	m.attr("FOOD_LIMIT") = FOOD_LIMIT;
	m.attr("POPULATION_LIMIT") = POPULATION_LIMIT;
}

#pragma endregion

#pragma region PyService

PyService PyService::instance;
PyService& PyService::GetInstance(void)
{
	return instance;
}

PyService::PyService(void) :
	settings(Settings::GetInstance()),
	adminInterpreter(PyInterpreter(PySecurityLevel::Admin)),
	gameInterpreter(PyInterpreter(PySecurityLevel::Game))
{
	this->guard = new py::scoped_interpreter();
	string init = "import platform\n"
		"import numpy as np\n"
		"from admin import *\n"
		"print('[DEBUG] Python ' + platform.python_version() + ' is working successfully!')";
	adminInterpreter.Evaluate(init);

	string init2 = "__tab__ = 0; __iframe = 0;";
	adminInterpreter.Evaluate(init2);

	/// args initialization
	//-- PLEASE, WRITE ONE KEY PER ROW!
	//-- PLEASE, ORDER KEYS LEXICOGRAPHICALLY.
	this->argsMap[IEnvironment::Environments::e_menu] = "("
		"{"
		"'engine':GetEngine(),"
		"'iframe':__iframe__,"
		"'tab':__tab__,"
		"'iframes':GetEngine().GetEnvironment().iframes,"
		"'menu':GetEngine().GetEnvironment(),"
		"'settings':GetSettings(),"
		"})";

	this->argsMap[IEnvironment::Environments::e_editor] = "("
		"{"
		"'adventure':GetEngine().GetEnvironment().adventure,"
		"'editor':GetEngine().GetEnvironment(),"
		"'editor_ui':GetEngine().GetEnvironment().GetUI(),"
		"'engine':GetEngine(),"
		"'iframe':__iframe__,"
		"'tab':__tab__,"
		"'iframes':GetEngine().GetEnvironment().iframes,"
		"'scenario':GetEngine().GetEnvironment().scenario,"
		"'players':GetEngine().GetEnvironment().scenario.playersArray.players,"
		"'players_settings':GetEngine().GetEnvironment().scenario.playersArray,"
		"'settings':GetSettings(),"
		"})";

	this->argsMap[IEnvironment::Environments::e_match] = "("
		"{"
		"'engine':GetEngine(),"
		"'iframe':__iframe__,"
		"'tab':__tab__,"
		"'match':GetEngine().GetEnvironment(),"
		"'settings':GetSettings(),"
		"})";
}

PyService::~PyService(void)
{
	if (this->guard != nullptr)
	{
		delete this->guard;
		this->guard = nullptr;
	}
}

void PyService::SetSecurityLevel(PySecurityLevel _securityLevel)
{
	this->currentSecurityLevel = _securityLevel;
}

PyInterpreter& PyService::GetInterpreter(PySecurityLevel _securityLevel)
{
	switch (_securityLevel)
	{
	case PySecurityLevel::Admin:
		return this->adminInterpreter;
	case PySecurityLevel::Game:
		return this->gameInterpreter;
	default:
		return this->adminInterpreter;
	}
}

bool PyService::CurrentInterpreterIsAdmin(void) const
{
	return this->currentSecurityLevel == PySecurityLevel::Admin;
}

bool PyService::CurrentInterpreterIsGame(void) const
{
	return this->currentSecurityLevel == PySecurityLevel::Game;
}

std::string PyService::GetArgs(IEnvironment::Environments env) const
{
	return this->argsMap.at(env);
}
#pragma endregion

#pragma region PyInterpreter
PyInterpreter::PyInterpreter(PySecurityLevel _securityLevel) : securityLevel(_securityLevel)
{
}

PyInterpreter::~PyInterpreter(void)
{
}

void PyInterpreter::Evaluate(const std::string& command, PyEvaluationTypes type, void* data)
{
	if (command.empty() == true)
		return;

	this->SetSecurityLevel();
	try
	{
		std::string* stringptr = nullptr;
		bool* boolptr = nullptr;
		int* intptr = nullptr;
		float* floatptr = nullptr;
		std::vector<float>* floatvecptr = nullptr;

		switch (type)
		{
		case PyInterpreter::PyEvaluationTypes::Void:
			py::exec(command);
			break;
		case PyInterpreter::PyEvaluationTypes::String:
			stringptr = (std::string*)data;
			(*stringptr) = py::eval(command).cast<std::string>();
			break;
		case PyInterpreter::PyEvaluationTypes::Boolean:
			boolptr = (bool*)data;
			(*boolptr) = py::eval(command).cast<bool>();
			break;
		case PyInterpreter::PyEvaluationTypes::Integer:
			intptr = (int*)data;
			(*intptr) = py::eval(command).cast<int>();
			break;
		case PyInterpreter::PyEvaluationTypes::Float:
			floatptr = (float*)data;
			(*floatptr) = py::eval(command).cast<float>();
			break;
		case PyInterpreter::PyEvaluationTypes::FloatVector:
			floatvecptr = (std::vector<float>*)data;
			(*floatvecptr) = py::eval(command).cast<std::vector<float>>();
			break;
		}
	}
	catch (const std::exception& ex)
	{
		//TODO
		std::cout << "[DEBUG] " << ex.what() << std::endl;
	}
}

void PyInterpreter::Bind(std::string&& name, PyBindedTypes type, void* data)
{
	try
	{
		py::module_ main = py::module_::import("__main__");
		switch (type)
		{
		case PyInterpreter::PyBindedTypes::String:
			main.attr(name.c_str()) = (std::string*)data;
			break;
		case PyInterpreter::PyBindedTypes::Boolean:
			main.attr(name.c_str()) = (bool*)data;
			break;
		case PyInterpreter::PyBindedTypes::Integer:
			main.attr(name.c_str()) = (int*)data;
			break;
		case PyInterpreter::PyBindedTypes::Float:
			main.attr(name.c_str()) = (float*)data;
			break;
		case PyInterpreter::PyBindedTypes::Engine:
			main.attr(name.c_str()) = (Engine*)data;
			break;
		case PyInterpreter::PyBindedTypes::Iframe:
			main.attr(name.c_str()) = (gui::Iframe*)data;
			break;
		default:
			//ASSERTION: default clause is an error point.
			assert(false == true);
			break;
		}
	}
	catch (const std::exception& ex)
	{
		//TODO
		std::cout << "[DEBUG]" << ex.what() << std::endl;
	}
}

void PyInterpreter::BindSharedPtr(const std::string& name, PyBindedSPTypes type, const std::shared_ptr<void>& data)
{
	try
	{
		py::module_ main = py::module_::import("__main__");
		switch (type)
		{
		case PyInterpreter::PyBindedSPTypes::GObject:
			main.attr(name.c_str()) = std::static_pointer_cast<GObject>(data);
			break;
		case PyInterpreter::PyBindedSPTypes::Settlement:
			main.attr(name.c_str()) = std::static_pointer_cast<Settlement>(data);
			break;
		case PyInterpreter::PyBindedSPTypes::ObjsList:
			main.attr(name.c_str()) = std::static_pointer_cast<ObjsList>(data);
			break;
		case PyInterpreter::PyBindedSPTypes::MultiSelection:
			main.attr(name.c_str()) = std::static_pointer_cast<SelectedObjects>(data);
			break;
		case PyInterpreter::PyBindedSPTypes::Command:
			main.attr(name.c_str()) = std::static_pointer_cast<Command>(data);
			break;
		default:
			//ASSERTION: default clause is an error point.
			assert(false == true);
			break;
		}
	}
	catch (const std::exception& ex)
	{
		//TODO
		std::cout << "[DEBUG]" << ex.what() << std::endl;
	}
}

void PyInterpreter::SetSecurityLevel(void)
{
	PyService::GetInstance().SetSecurityLevel(this->securityLevel);
}
#pragma endregion

#pragma region PyConsole

PyConsole PyConsole::instance;
PyConsole& PyConsole::GetInstance(void)
{
	return instance;
}
void PyConsole::Render(void)
{
	if (keyboard.IsKeyPressed(GLFW_KEY_F2))
	{
		this->bIsActive = !this->bIsActive;
	}
	if (this->bIsActive)
	{
		ImGui::SetNextWindowPos(ImVec2(219, viewport.GetHeight() - 200), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(471, 60), ImGuiCond_Once);
		ImGui::Begin("Python Console", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::PushItemWidth(400);
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		bool enterPressed = ImGui::InputText(inputLabel, inputBuf, IM_ARRAYSIZE(inputBuf), flags);
		ImGui::PopItemWidth();

		ImGui::SameLine(0.0f, 10.0f);
		this->commandToExecute = "";
		if (enterPressed || ImGui::Button("Send", ImVec2(40, 20)))
		{
			this->commandToExecute = inputBuf;
			inputBuf[0] = '\0';
			inputBuf[255] = { 0 };
		}
		ImGui::End();
	}
	else
	{
		std::string cmd = inputBuf;
		if (cmd.size() > 0)
			inputBuf[0] = '\0';
	}
}
void PyConsole::ExecuteCommand(void)
{
	py.Evaluate(this->commandToExecute);
}
PyConsole::PyConsole() : py(PyService::GetInstance().GetInterpreter(PySecurityLevel::Admin)), keyboard(rattlesmake::peripherals::keyboard::get_instance()), viewport(rattlesmake::peripherals::viewport::get_instance())
{
}
PyConsole::~PyConsole()
{
}
#pragma endregion

#pragma region PyFunction

PyParam::Type PyFunction::GetParamType(std::string typeStr, std::string name)
{
	//TODO - Replace this assertion with a throw.
	assert(name.empty() == false && typeStr.empty() == false);

	if (typeStr == "GObject" && name == "this")
		return PyParam::Type::GObject;
	if (typeStr == "string")
		return PyParam::Type::String;
	if (typeStr == "Target" && name == "target")
		return PyParam::Type::Target;
	if (typeStr == "Command" && name == "cmd")
		return PyParam::Type::Command;

	//ASSERTION: Param must be valid. Replace this assertion with a throw.
	assert(true == false);
	return PyParam::Type::Undefined;
}

std::string PyFunction::GetParamValueAsString(PyParameter::Type type, std::string& commandName)
{
	switch (type)
	{
	case PyParam::Type::GObject:
		return OBJECT_BINDING_PYTHON;
	case PyParam::Type::Target:
		return "__o__.GetMyTarget()";
	case PyParam::Type::Command:
		return "__o__.GetCommandById('" + commandName + "')";
	case PyParam::Type::String:
	case PyParam::Type::Undefined:
		assert(true == false);
		break;
	default:
		assert(true == false);
		break;
	}
	return "NA";
}

PyFunction::PyFunction(PyInterpreter::PyEvaluationTypes _type, std::string _name, std::vector<PyParameter>& paramsWithoutValues) : 
	py(PyService::GetInstance().GetInterpreter(PySecurityLevel::Game)),
	type(_type), name(_name)
{
}

std::string PyFunction::GetFunctionSignature(std::vector<PyParameter>& paramsWithValues)
{
	//Creation of args param for a script.
	std::stringstream ss;
	ss << this->name << "({";
	const size_t nParams = paramsWithValues.size();
	for (size_t i = 0; i < nParams; i++)
	{
		if (paramsWithValues[i].value.empty())
			return "";
		ss << "'" << paramsWithValues[i].name << "':" << paramsWithValues[i].value;
		if (i < nParams - 1)
			ss << ", ";
	}
	ss << "})";
	return ss.str();
}
void PyFunction::Execute(const std::string& functionExecutionString, void* data)
{
	if (functionExecutionString.empty())
		return;

	// test this evaluate, if it's ok delete all code below
	py.Evaluate(functionExecutionString, this->type, data);

	//if (this->type == PyInterpreter::PyEvaluationTypes::Void)
	//{
	//	py.Evaluate(functionExecutionString);
	//}
	//else
	//{
	//	py.Evaluate("___temp = " + functionExecutionString);
	//	py.Evaluate("___temp", this->type, data);
	//	py.Evaluate("del ___temp");
	//}
}
bool PyFunction::IsUndefined(void) const
{
	return this->type == PyInterpreter::PyEvaluationTypes::Undefined;
}
std::string PyFunction::GetName(void) const
{
	return this->name;
}
#pragma endregion
