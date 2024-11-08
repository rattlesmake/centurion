#include "bottombar.h"
#include <environments/game/match/matchUI/matchUi.h>
#include <environments/game/classes/playable.h>
#include <environments/game/classes/objectsSet/selected_objects.h>

// Assets
#include <xml_class_command.h>


MatchUI_BottomBar::MatchUI_BottomBar(MatchUI& my_creator) :
	myCreator(my_creator)
{
}

MatchUI_BottomBar::~MatchUI_BottomBar(void)
{
}

void MatchUI_BottomBar::Create(tinyxml2::XMLElement* commandsXML)
{
	auto bottombar = myCreator.GetIframeById("BottomBar");  // Bottombar iframe.
	assert(bottombar.expired() == false);  // no bottombar - TODO exception - mettere eccezione

	this->iframe = std::move(bottombar);  // Bottombar iframe.
	/// COMMANDS UI - i.e. commands when nothing is selected
	// Commands reading
	tinyxml2::XMLElement* commandsXml = commandsXML->FirstChildElement("commands");
	assert(commandsXml != nullptr);
	uint16_t readOrder = 0;
	for (tinyxml2::XMLElement* child = commandsXml->FirstChildElement(); child != nullptr; child = child->NextSiblingElement(), readOrder++)
	{
		// Create the command
		this->commandsUI.push_back(assets::xmlCmdUP_t{ new assets::xml_class_command { child, readOrder, true } });
		std::shared_ptr<Command> cmd = Command::CreateCommand(*this->commandsUI.back(), true);
		// Save the command
		this->commandsMapByPriority.insert({ static_cast<uint16_t>(cmd->GetPriority()) , std::move(cmd) });
	}
}

void MatchUI_BottomBar::Update(void)
{
	assert(this->iframe.lock() != nullptr);
	// I should am here, if nobody changes the bottombar id into match_ui.xml.

	//if (this->IsBottomBarChanged() == false)
		//return;

	const std::shared_ptr<Match> gameSP = this->owner.lock();
	const uint32_t numberOfSelectedObjects = gameSP->GetNumberOfSelectedObjects();

	const std::shared_ptr<Playable> selp = gameSP->Selp();
	const std::shared_ptr<SelectedObjects> selection = gameSP->Selection();
	const uint8_t player_id = (selp) ? selp->GetPlayer() : PlayersArray::UNDEFINED_PLAYER_INDEX;
	const bool bIsCurrentPlayer = (selp) ? Engine::GetInstance().GetEnvironment()->AsIGame()->IsCurrentPlayer(player_id) : false;

	UI_Elements ui;
	ui.pos.y = 0;

	if (numberOfSelectedObjects > 1)
		ui.nButtons = selection->GetNumberOfCommands();
	else if (numberOfSelectedObjects == 1 && bIsCurrentPlayer)
		ui.nButtons = selp->GetNumberOfCommands();
	else
		ui.nButtons = myCreator.GetNumberOfCommands();

	assert(ui.nButtons <= MatchUI_BottomBar::MAX_NUMBER_OF_CMDS);
	auto iframeSP = this->iframe.lock();
	uint8_t printedButtons = 0;  // It is used since not each button is always printed (for instance, a button is not printed if it's disabled).
	for (uint8_t i = 0; i < MatchUI_BottomBar::MAX_NUMBER_OF_CMDS; ++i)
	{
		auto cmdButton = std::static_pointer_cast<gui::ImGuiImageButton>(iframeSP->GetElementByTagAndId(gui::ImGuiElement::ClassesTypes::e_imageButton, i));
		assert(cmdButton);

		// Hide a cmd button (avoids to display old cmds when the number of commands changes).
		cmdButton->Hide();  // Hidden value = false --> hide

		// Reset function to run when button is clicked in order to avoid memory leakage of the binded params
		// Remember these buttons are always in RAM; infact when a command button is not displayed is because it's just hidden
		cmdButton->ResetFunctionToRun();

		cmdButton->ResetFunctionToGetTooltipInfo();

		if (i < ui.nButtons)
		{
			// Get next command to show:
			if (numberOfSelectedObjects > 1)
				ui.cmd = selection->GetCommand(i);
			else if (numberOfSelectedObjects == 1 && bIsCurrentPlayer)
				ui.cmd = selp->GetCommand(i);
			else  // Nothing is selected. Get UI commands.
				ui.cmd = myCreator.GetCommandByPriority(i);

			// Update cmd button:
			if (ui.cmd->IsValid() == true && ui.cmd->IsDisabled() == false)
			{
				// Rollover is computed only when a specific command tooltip will be rendered :)

				ui.pos.x = ui.buttonSize * printedButtons + rattlesmake::peripherals::viewport::get_instance().GetWidth() * 0.5f - ui.nButtons * ui.buttonSize * 0.5f;
				
				// Set pos
				cmdButton->SetPosition(std::move(ui.pos));

				// Set size (square button)
				cmdButton->SetSize(ImVec2(ui.buttonSize, ui.buttonSize));

				// Set image
				cmdButton->SetImage(ui.cmd->GetBottombarImageNameCRef());

				// Set onclick fun: execute command
				auto onclickFun = gui::CreateImGuiImageButtonFunctWrapper(std::bind(&Command::Execute, ui.cmd));
				cmdButton->SetFunctionToRun(std::move(onclickFun));

				// Set fun to execute in order to get a tooltip
				cmdButton->SetFunctionToGetTooltipInfo(std::bind(&Command::GetTooltipInfo, ui.cmd));

				// Show button
				cmdButton->Show();

				// A button was printed
				printedButtons++;
			}
		}
	}
}
