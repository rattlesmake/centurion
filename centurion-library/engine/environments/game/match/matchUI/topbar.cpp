#include "topbar.h"
#include <environments/game/match/matchUI/matchUi.h>

// Classes
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <environments/game/classes/objectsSet/selected_objects.h>

// Assets
#include <xml_value.h>
#include <xml_values.h>

// Gui
#include <iframe/imguiProgressBar.h>

// Services
#include <services/sqlservice.h>


#ifndef PATH_SPAWN_BUTTONS
#define PATH_SPAWN_BUTTONS       "game/match/topbar/icons/spawn_buttons/"
#endif // !PATH_SPAWN_BUTTONS

#ifndef PATH_VALUES_ICONS
#define PATH_VALUES_ICONS       "game/match/topbar/icons/values/"
#endif // !PATH_VALUES_ICONS


std::map<MatchUI_TopBar::SpawnButton, MatchUI_TopBar::SpawnButtonInfo> MatchUI_TopBar::spawnButtonsInfo;


#pragma region Constructor and destructor:
MatchUI_TopBar::MatchUI_TopBar(MatchUI& my_creator) : myCreator(my_creator)
{
	MatchUI_TopBar::spawnButtonsInfo.clear();
	MatchUI_TopBar::spawnButtonsInfo.insert({ MatchUI_TopBar::SpawnButton::e_garrison, SpawnButtonInfo{ "garrison" } });
	MatchUI_TopBar::spawnButtonsInfo.insert({ MatchUI_TopBar::SpawnButton::e_technologies, SpawnButtonInfo{"technologies"} });
	MatchUI_TopBar::spawnButtonsInfo.insert({ MatchUI_TopBar::SpawnButton::e_trainings, SpawnButtonInfo{"trainings"} });
}

MatchUI_TopBar::~MatchUI_TopBar(void)
{
}
#pragma endregion

void MatchUI_TopBar::Create(void)
{
	auto topbar = myCreator.GetIframeById("TopBar"); //Topbar iframe.
	assert(topbar.expired() == false); // no topbar

	this->iframe = std::move(topbar);
}

void MatchUI_TopBar::Update(void)
{
	assert(this->iframe.lock() != nullptr);
	//I should be here, if nobody changes the topbar id into match_ui.xml.

	//Reset top bar if it has changed
	this->Reset();

	const uint32_t numberOfSelectedObjects = this->owner.lock()->GetNumberOfSelectedObjects();
	if (numberOfSelectedObjects >= 1)
	{
		///	REMEMBER: if numberOfSelectedObjects is greater than 1, then it is certain that that multiple selection
		/// (VAR_SelectedObjsOL) can include only units.
		/// Otherwise, if VAR_NumberOfSelectedObjects is equal to 1, VAR_SelectedObjsOL can be both a unit and a building.
		/// Therefore, in the second case, you can use Selb() to get the selected building.
		this->UpdateLeftSide();
		this->UpdateRightSide();
	}

	/*
	auto gameSP = this->myCreator->GetOwner();
	if (this->IsTopBarChanged() == false)
	{
		const std::shared_ptr<Building> SELB = gameSP->Selb();
		const bool bIsCurrentPlayer = SELB->GetPlayerRef()->IsCurrentPlayer() == true;

		if (bIsCurrentPlayer)
		{
			if (this->bUnitsProduction == true || this->bTech == true)
			{
				UI_Elements ui;
				float progressCMD = SELB->GetCurrentCmdProgress((this->bUnitsProduction == true) ? CommandTypes::Training : CommandTypes::Technology);
				if (progressCMD >= 0)
				{
					this->iframe.lock()->SetProgressBarValueById(ui.nextProgressBarID, progressCMD);
				}
			}
		}
		return;
	}

*/
}

void MatchUI_TopBar::UpdateLeftSide(void)
{
	auto iframeSP = this->iframe.lock();
	const std::shared_ptr<Match> gameSP = this->owner.lock();
	const uint32_t numberOfSelectedObjects = gameSP->GetNumberOfSelectedObjects();

	const auto selection = gameSP->Selection();
	const std::shared_ptr<ObjsList> selectionOL = selection->GetTroops();

	// ASSERTION: I must have a PlayableGObject, since I can be here only if selection contains at least one PlayableGObject.
	assert(selectionOL->Get(0) && selectionOL->Get(0)->IsPlayableGObject() == true);

	std::shared_ptr<Playable> VAR_Object = std::static_pointer_cast<Playable>(selectionOL->Get(0));
	const uint32_t numberOfDiffClasses = selectionOL->GetNumberOfDifferentClasses();

	///Displayed name and icon
	{
		std::string textDisplayedName;
		std::string iconName;
		if (numberOfDiffClasses == 1)  // Only a type of objects is selected (for example, selection includes only RPraetorian).
		{
			iconName = VAR_Object->GetIconName();
			if (numberOfSelectedObjects == 1)
			{
				textDisplayedName = VAR_Object->GetDisplayedName();
				// If the selected objectes is an hero, display also the number of units attached to him.
				if (VAR_Object->IsHero() == true)
				{
					const auto heroArmy = std::static_pointer_cast<Hero>(VAR_Object)->GetArmy();
					textDisplayedName = textDisplayedName + " --- " + std::to_string(heroArmy->GetNumberOfUnits()) + "/" + std::to_string(heroArmy->GetMaxNumberOfUnits());
				}
				else if (VAR_Object->IsBuilding() == true && std::static_pointer_cast<Building>(VAR_Object)->HasGarrison() == true)
				{
					const auto garrison = std::static_pointer_cast<Building>(VAR_Object)->GetGarrison();
					textDisplayedName = textDisplayedName + " --- " + std::to_string(garrison->GetNumberOfUnits()) + "/" + std::to_string(garrison->GetMaxNumberOfUnits());
				}
			}
			else  // > 1
			{
				// ASSERTION: If selection has more than one PlayableGObject, then they must be units.
				assert(VAR_Object->IsUnit() == true);
				textDisplayedName = std::to_string(numberOfSelectedObjects) + " " + std::static_pointer_cast<Unit>(VAR_Object)->GetPluralName();
			}
		}
		else
		{
			// Use of an appropriate icon and an appropriate text when the selected units belong to different classes (for instance, selection includes two RPraetorian, an RHero and four RArcher).
			iconName = "game/match/topbar/icons/classes/MultipleSelection";
			textDisplayedName = std::to_string(numberOfSelectedObjects) + " " + SqlService::GetInstance().GetTranslation("units", true);
		}

		// Update displayed icon
		std::static_pointer_cast<gui::ImGuiImage>(iframeSP->GetElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_image, 1))->SetImage(std::move(iconName));

		// Update displayed text
		std::static_pointer_cast<gui::ImGuiText>(iframeSP->GetElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_text, 1))->SetTextWithTranslation(std::move(textDisplayedName), false);
	}

	/// Values
	float xValuePos = 75.0;  // to make it responsive
	const valuesList_t& valuesToPrint = VAR_Object->GetValues();

	uint8_t valuesPrinted = 0;
	for (auto const& value_it : valuesToPrint)
	{
		const std::string& valueName = value_it->get_name_cref();
		std::string valueText = Playable::ExecuteValueScript(value_it);
		// Print a value with its icon only if the retrieved text is not an empty string
		if (valueText.empty() == false)
		{
			if (numberOfSelectedObjects > 1)
			{
				std::string firstHalf;
				std::string secondHalf;
				if (std::split_string(valueText, firstHalf, secondHalf, '/') == true && std::is_number(firstHalf) == true && std::is_number(secondHalf) == true)
				{
					const int secondHalfInt = std::stoi(secondHalf);
					if (secondHalfInt != 0)
						valueText = std::to_string(int((float(std::stoi(firstHalf)) / float(secondHalfInt)) * 100.f)) + "%%";
				}
			}
			valuesPrinted += 1;  // A new value is being created.

			std::string valueIcon{ PATH_VALUES_ICONS };
			if (valueName == "attack" && VAR_Object->IsUnit() == true)
			{
				// Attack icon is particular.
				// Engine chooses a proper icon according to the number of selected objects and the damage type.
				DamageTypes damageType = selection->GetDamageType();
				if (damageType == DamageTypes::e_both)
					valueIcon = valueIcon + "mixed_" + value_it->get_icon_path_cref();
				else if (damageType == DamageTypes::e_pierce)
					valueIcon = valueIcon + "piercing_" + value_it->get_icon_path_cref();
				else
					valueIcon = valueIcon + "slash_" + value_it->get_icon_path_cref();
			}
			else
				valueIcon += value_it->get_icon_path_cref();

			// Insertion in the top bar:
			float yValuePos = 45.0;
			if ((valuesPrinted % 2) == 0)
				yValuePos += 30;
			else if (valuesPrinted > 1)
				xValuePos += 90;

			// Add an image and a text to the iframe
			auto valueImgSP = iframeSP->AddImage(xValuePos, yValuePos, 32, 32, std::move(valueIcon), "w_" + valueName);
			auto valueTextSP = iframeSP->AddText(xValuePos + 35, yValuePos - 25, std::move(valueText));

			// Store ID of the created iframe image and text in order to reset them after
			this->valuesIDs.push_back({ valueImgSP->GetId(), valueTextSP->GetId() });
		}
	}
}

void MatchUI_TopBar::UpdateRightSide(void)
{
	UI_Elements ui;
	const std::shared_ptr<Match> gameSP = this->owner.lock();
	//Get the pointer in order to have a faster access to the content of the shared ptr.
	//Obviously, owner must remain locked!.
	auto gamePtr = gameSP.get();

	//Continue only if selected object is a building.
	std::shared_ptr<Building> selb = gamePtr->Selb();
	if (!selb)
	{
		auto selection = gamePtr->Selection();
		auto& selectionMap = selection->GetCounterByEgressTimeMapCRef();
		if (selectionMap.size() >= 2) //There are at least two different type of classes selected (N.B.: more heroes are considered as different classes).
		{
			this->PrintOrderedUnitsList(ui, (*selection.get()), false);
		}
		else if (selectionMap.empty() == false) //i.e. selectionMap.size() == 1
		{
			auto heroSP = selectionMap.begin()->second.heroWRef.lock();
			if (heroSP) //Selected unit is an hero.
				this->PrintOrderedUnitsList(ui, (*heroSP->GetArmy()), false);
		}
	}
	else //Selection contains a building. It's right side is more complex than that of a unit.
	{
		this->UpdateBuildingRightSide(ui, (*selb));
	}
}

void MatchUI_TopBar::UpdateBuildingRightSide(UI_Elements& ui, const Building& selb)
{
	// For each stack, check if it is necessary to create a button that shows/hides it.
	// If the building doesn't belong to the current player, only the units inside stack will be shown (obviously if the current building is an holder).
	const bool bIsCurrentPlayer = this->owner.lock()->IsCurrentPlayer(selb.GetPlayer());

	for (auto& it : MatchUI_TopBar::spawnButtonsInfo)
		it.second.bToConsider = false;

	if (bIsCurrentPlayer)
	{
		if (selb.HasGarrison() == true)
		{
			if (selb.CanTrainUnits() == true || selb.CanReseachTechnologies() == true)
				MatchUI_TopBar::spawnButtonsInfo[SpawnButton::e_garrison].bToConsider = true;
			else
			{
				ui.placedButtons += 1;  // Since any spawn button will be printed.
				this->stackToShow = static_cast<uint8_t>(SpawnButton::e_garrison);
			}
		}
		if (selb.CanTrainUnits() == true)
		{
			if (selb.HasGarrison() == true || selb.CanReseachTechnologies() == true)
				MatchUI_TopBar::spawnButtonsInfo[SpawnButton::e_trainings].bToConsider = true;
			else
			{
				ui.placedButtons += 1;  // Since any spawn button will be printed.
				this->stackToShow = static_cast<uint8_t>(SpawnButton::e_trainings);
			}
		}
		if (selb.CanReseachTechnologies() == true)
		{
			if (selb.HasGarrison() == true || selb.CanTrainUnits() == true)
				MatchUI_TopBar::spawnButtonsInfo[SpawnButton::e_technologies].bToConsider = true;
			else
			{
				ui.placedButtons += 1;  // Since any spawn button will be printed.
				this->stackToShow = static_cast<uint8_t>(SpawnButton::e_technologies);
			}
		}
	}
	else if (selb.HasGarrison() == true)
	{
		ui.placedButtons += 1; //Since any spawn button will be printed.
		this->stackToShow = static_cast<uint8_t>(SpawnButton::e_garrison);
	}

	for (auto const& [key, value] : MatchUI_TopBar::spawnButtonsInfo)
	{
		if (this->stackToShow == 0)
		{
			///Show a button to open a stack
			if (value.bToConsider == true)
			{
				// Create a button that show a proper stack when it will be clicked
				ui.xPos = ui.startXPos + ui.placedButtons * ui.buttonWidth;
				ui.imageName = PATH_SPAWN_BUTTONS + value.pathID + "Unclicked";
				ui.buttonText = "";

				auto topBarBtn = this->iframe.lock()->AddButton(ui.xPos, ui.yPos, ui.buttonWidth, ui.buttonHeight, std::move(ui.buttonText), std::move(ui.imageName), "", false);
				// Onclick fun: open a spawn button
				auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&MatchUI_TopBar::SetStackToShow, this, static_cast<uint8_t>(key)));
				topBarBtn->SetFunctionToRun(std::move(onclickFun));

				ui.placedButtons += 1;
			}
		}
		else if (this->stackToShow == static_cast<uint8_t>(key))
		{
			///Show a stack
			if (value.bToConsider == true)
			{
				// Create a button that close a proper stack when it will be clicked
				ui.xPos = ui.startXPos + ui.placedButtons * ui.buttonWidth;
				ui.imageName = PATH_SPAWN_BUTTONS + value.pathID + "Clicked";
				ui.buttonText = "";

				auto topBarBtn = this->iframe.lock()->AddButton(ui.xPos, ui.yPos, ui.buttonWidth, ui.buttonHeight, ui.buttonText, std::move(ui.imageName), "", false);
				// Onclick fun: close a spawn button
				auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&MatchUI_TopBar::SetStackToShow, this, static_cast<uint8_t>(0)));
				topBarBtn->SetFunctionToRun(std::move(onclickFun));
				
				ui.placedButtons += 1;
			}

			if (this->stackToShow == static_cast<uint8_t>(MatchUI_TopBar::SpawnButton::e_garrison))
				this->PrintOrderedUnitsList(ui, (*selb.GetGarrison().get()), true);
			else if (this->stackToShow == static_cast<uint8_t>(MatchUI_TopBar::SpawnButton::e_technologies))
				this->PrintCommandsQueue(ui, *(selb.GetCommandsQueue()), assets::xml_command_type::Technology);
			else if (this->stackToShow == static_cast<uint8_t>(MatchUI_TopBar::SpawnButton::e_trainings))
				this->PrintCommandsQueue(ui, *(selb.GetCommandsQueue()), assets::xml_command_type::Training);
		}
	}
}

void MatchUI_TopBar::SetStackToShow(const uint8_t idSpawnBtn)
{
	this->stackToShow = idSpawnBtn;
}

void MatchUI_TopBar::Reset(void)
{
	auto iframeSP = this->iframe.lock();

	// Clear values icons and text
	assert(this->valuesIDs.size() <= assets::xml_values::MAX_NUMBER_OF_VALUES_PER_CLASS);
	while (this->valuesIDs.empty() == false)
	{
		auto& ids = this->valuesIDs.front();
		iframeSP->ClearElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_image, ids.first);
		iframeSP->ClearElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_text, ids.second);
		this->valuesIDs.pop_front();
	}

	// Clear GObject's icon
	std::static_pointer_cast<gui::ImGuiImage>(iframeSP->GetElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_image, 1))->SetImage("");

	// Clear GOobject's displayed name
	std::static_pointer_cast<gui::ImGuiText>(iframeSP->GetElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_text, 1))->SetText("");

	
	// Clear each progressBar
	iframeSP->ClearElementsByTag(gui::ImGuiElement::ClassesTypes::e_progressBar);
	// Clear each button.
	iframeSP->ClearElementsByTag(gui::ImGuiElement::ClassesTypes::e_imageButton);

	// Spawn btn:
	const std::shared_ptr<Match> game = this->owner.lock();
	const uint32_t numberOfSelectedObjects = game->GetNumberOfSelectedObjects();
	if (numberOfSelectedObjects == 1)
	{
		const auto selo = game->Selo();
		auto flag = iframeSP->GetFlag("obj");
		// Clear spawn button only if selection has changed.
		if (flag.has_value() == false || flag != selo->GetUniqueID())
		{
			iframeSP->SetFlag("obj", selo->GetUniqueID());
			this->stackToShow = 0;  // Reset stack to show if selected object is changed.
		}
	}
	else
	{
		iframeSP->ClearFlag("obj");
	}
}

void MatchUI_TopBar::PrintOrderedUnitsList(MatchUI_TopBar::UI_Elements& ui, const OrderedUnitsList& orderedUnitsList, const bool bIsGarrisonStack)
{
	const auto& unitsStackMap = orderedUnitsList.GetCounterByEgressTimeMapCRef();
	auto troops = orderedUnitsList.GetTroops();
	for (auto const& element : unitsStackMap)
	{
		auto& info = element.second;

		//Icon of the button and number of elements
		ui.imageName = info.iconName;
		const bool bIsHero = info.heroWRef.expired() == false;
		if (bIsHero == true)
		{
			auto heroArmy = info.heroWRef.lock()->GetArmy();
			uint32_t numberOfTroops = (bIsGarrisonStack == true) ? heroArmy->GetTroopsInTheSameBuilding()->Count() : heroArmy->GetTroops()->Count();
			ui.buttonText = (numberOfTroops > 0) ? std::to_string(numberOfTroops) : "";
		}
		else
			ui.buttonText = std::to_string(info.counter);

		//Set position
		ui.xPos = ui.startXPos + ui.placedButtons * ui.buttonWidth;

		auto iframeSP = this->iframe.lock();
		//Add button
		if (bIsHero == false)
		{
			auto topBarBtn = this->iframe.lock()->AddButton(ui.xPos, ui.yPos, ui.buttonWidth, ui.buttonHeight, std::move(ui.buttonText), std::move(ui.imageName), "", false);
			// Onclick function: select an ObjsList.
			auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&ObjsList::SelectByClass, troops, 1, info.id));
			topBarBtn->SetFunctionToRun(std::move(onclickFun));
		}
		else
		{
			auto topBarBtn = this->iframe.lock()->AddButton(ui.xPos, ui.yPos, ui.buttonWidth, ui.buttonHeight, std::move(ui.buttonText), std::move(ui.imageName), "", false);
			// Onclick function: select an hero
			auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&ObjsList::SelectHero, troops, 1, info.heroWRef.lock()));
			topBarBtn->SetFunctionToRun(std::move(onclickFun));
		}
		ui.placedButtons += 1;

		// Show an average health bar
		float percHealth = -1.0;
		if (bIsHero == true)
		{
			if (bIsGarrisonStack == true)
				percHealth = info.heroWRef.lock()->GetArmy()->GetPercHealthUnitsInTheSameGarrison("");
			else
				percHealth = info.heroWRef.lock()->GetArmy()->GetPercHealth("");
		}
		else
			percHealth = orderedUnitsList.GetPercHealth(info.id);
		if (percHealth >= 0)
			 iframeSP->AddProgressBar(ui.xPos, ui.yPos + ui.buttonHeight - floor(ui.buttonHeight / ui.barScaling), ui.buttonWidth, floor(ui.buttonHeight / ui.barScaling), percHealth, true);
	}
}

void MatchUI_TopBar::PrintCommandsQueue(UI_Elements& ui, CommandsQueue& cmdsQueue, assets::xml_command_type type)
{
	uint32_t index = 0;
	const CommandsQueue::cmdsQueue_t& q = cmdsQueue.GetQueueByType(type);
	for (auto const& element : q)
	{
		// Icon of the button and number of elements
		ui.imageName = element.cmd.lock()->GetTopbarImageNameCRef();
		ui.buttonText = std::to_string(element.counter);

		// Set position
		ui.xPos = ui.startXPos + ui.placedButtons * ui.buttonWidth;

		// Add button
		auto topBarBtn = this->iframe.lock()->AddButton(ui.xPos, ui.yPos, ui.buttonWidth, ui.buttonHeight, std::move(ui.buttonText), std::move(ui.imageName), "", false);
		// Onclick fun: remove a command from a command queue
		auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&CommandsQueue::RemoveElement, &cmdsQueue, type, index));
		topBarBtn->SetFunctionToRun(std::move(onclickFun));
		ui.placedButtons += 1;

		// Show a progress bar for the first command into the queue (it shows the the completion status):
		if (element.completionPerc >= 0)
			this->iframe.lock()->AddProgressBar(ui.xPos, ui.yPos + ui.buttonHeight - floor(ui.buttonHeight / ui.barScaling), ui.buttonWidth, floor(ui.buttonHeight / ui.barScaling), element.completionPerc);
		index++;
	}
}
