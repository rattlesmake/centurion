#include "objectCommand.h"
#include <engine.h>
#include <environments/game/igame.h>
#include <environments/game/igameUi.h>
#include <ui.h>
#include <environments/game/classes/objectsSet/objects_list.h>
#include <encode_utils.h>

#include <png_shader.h>
#include <png.h>

// Assets
#include <xml_class_command.h>

const std::string emptyStr;
const std::list<assets::xml_class_command_params> emptyParamsList;


#pragma region Costructors, destructor and operators:
Command::Command(void) : 
	bValid(false), bEnabled(false), id(emptyStr), displayedName(emptyStr), description(emptyStr), method(emptyStr), params(emptyParamsList),
	bottomImageName(emptyStr), topImageName(emptyStr), inheritedCmdLevel(0), xmlReadOrder(0), priority(0), iframeToOpen(emptyStr)
{
}

Command::Command(const assets::xmlClassCommand_t& xmlCommand, const bool _bIsUICommand) :
	bValid(true), bEnabled(true), bIsUICommand(_bIsUICommand),
	id(xmlCommand.GetIdCRef()), displayedName(xmlCommand.GetDisplayedNameCRef()),
	description(xmlCommand.GetDescriptionCRef()), method(xmlCommand.GetMethodCRef()), params(xmlCommand.GetParamsListCRef()),
	bottomImageName(xmlCommand.GetBottombarIconCRef()), topImageName(xmlCommand.GetTopbarIconCRef()),
	staminaCost(xmlCommand.GetStaminaCost()), priority(xmlCommand.GetBottombarPriority()), 
	goldCost(xmlCommand.GetGoldCost()), foodCost(xmlCommand.GetFoodCost()), populationCost(xmlCommand.GetPopulationCost()),
	delay(xmlCommand.GetDelay()), inheritedCmdLevel(xmlCommand.GetInheritedLevel()), xmlReadOrder(xmlCommand.GetXMLReadOrder()),
	iframeToOpen(xmlCommand.GetIframeToOpen()), type(xmlCommand.GetType())
{
}

Command::~Command(void)
{
}

bool Command::operator==(const Command& other) noexcept
{
	return (this->id == other.id);
}

bool operator<(const Command& lhs, const Command& rhs) noexcept
{
	// Try to order by priority bar
	if (lhs.priority < rhs.priority)
		return true;
	if (lhs.priority > rhs.priority)
		return false;

	// Priority bar is the same: try to order by inheritance (inherited commands have have higher priority than a self command with the same priority)
	if (lhs.inheritedCmdLevel > rhs.inheritedCmdLevel)
		return true;
	if (lhs.inheritedCmdLevel < rhs.inheritedCmdLevel)
		return false;

	// Inheritance level is the same: order by read order (a cmd write before in the XML will have an higher priority).
	return (lhs.xmlReadOrder < rhs.xmlReadOrder);
}
#pragma endregion

#pragma region Static members:
std::shared_ptr<Command> Command::CreateCommand(const assets::xmlClassCommand_t& xmlCommand, const bool bIsUICommand)
{
	return std::shared_ptr<Command>(new Command(xmlCommand, bIsUICommand));
}

std::shared_ptr<Command> Command::GetInvalidCommand(void)
{
	static std::shared_ptr<Command> unvalidCmd{ new Command() };
	return unvalidCmd;
}
#pragma endregion


#pragma region To external scripts:
bool Command::IsValid(void) const noexcept
{
	return this->bValid;
}

void Command::SetRollover(const std::string rollover)
{
	// A rollover can be set only if command is valid, is not an UI command
	if (this->bValid == true && this->bIsUICommand == false)
		this->rollover = rollover;
}

std::string Command::GetRollover(void) const noexcept
{
	return this->rollover;
}

void Command::ResetRollover(void) noexcept
{
	this->rollover = "";
}

void Command::SetStaminaCost(const uint8_t value)
{
	if (this->bValid == true)
		this->staminaCost = value;
}

uint8_t Command::GetStaminaCost(void) const noexcept
{
	return this->staminaCost;
}

void Command::SetGoldCost(const uint32_t value)
{
	if (this->bValid == true)
		this->goldCost = value;
}

uint32_t Command::GetGoldCost(void) const noexcept
{
	return this->goldCost;
}

void Command::SetFoodCost(const uint32_t value)
{
	if (this->bValid == true)
		this->foodCost = value;
}

uint32_t Command::GetFoodCost(void) const noexcept
{
	return this->foodCost;
}

void Command::SetPopulationCost(const uint32_t value)
{
	if (this->bValid == true)
		this->populationCost = value;
}

uint32_t Command::GetPopulationCost(void) const noexcept
{
	return this->populationCost;
}
#pragma endregion

bool Command::OpensIframeOnClick(void) const noexcept
{
	return (this->iframeToOpen.empty() == false);
}

bool Command::ExecuteMethodOnClick(void) const noexcept
{
	return (this->method.empty() == false);
}

const string& Command::GetIdCRef(void) const noexcept
{
	return this->id;
}

const string& Command::GetDisplayedNameCRef(void) const noexcept
{
	return this->displayedName;
}
 
const string& Command::GetDescriptionCRef(void) const noexcept
{
	return this->description;
}

assets::xml_command_type Command::GetType(void) const noexcept
{
	return this->type;
}

uint16_t Command::GetPriority(void) const noexcept
{
	return this->priority;
}

uint32_t Command::GetDelay(void) const noexcept
{
	return this->delay;
}

const string& Command::GetTopbarImageNameCRef(void) const noexcept
{
	return this->topImageName;
}

const string& Command::GetBottombarImageNameCRef(void) const noexcept
{
	return this->bottomImageName;
}

const string& Command::GetMethodCRef(void) const noexcept
{ 
	return this->method;
}

const assets::providedCmdParams_t& Command::GetParamsList(void) const noexcept
{
	return this->params;
}

void Command::SetEnableness(const bool _bIsEnabled)
{
	this->bEnabled = _bIsEnabled;
}

bool Command::IsDisabled(void) const noexcept
{
	return (this->bEnabled == false);
}

gui::ImGuiTooltip::tooltip_t Command::GetTooltipInfo(void)
{
	gui::ImGuiTooltip::tooltip_t tooltipInfo;

	static std::string gold_image_path = Encode::FixImageName("game\\match\\topbar\\icons\\values\\gold");
	static std::string food_image_path = Encode::FixImageName("game\\match\\topbar\\icons\\values\\food");
	static std::string population_image_path = Encode::FixImageName("game\\match\\topbar\\icons\\values\\population");
	static std::string stamina_image_path = Encode::FixImageName("game\\match\\topbar\\icons\\values\\stamina");

	// Get an image service instance
	//ImageService& imageService = ImageService::GetInstance(); 
	auto& imageShader = rattlesmake::image::png_shader::get_instance();

	// Test command condition. Needed to get command rollover or clean it
	// It has effect only for command such for which a target is not required or for commands not belonging to UI.
	// TODO - può dare problemi di lentezza per la multiselezione, in quanto effettua il test su tutti i GObjects selezionati.
	this->CatchRolloverForBottomBar();

	// Decide what translate
	if (this->displayedName.empty() == false)

	if (this->displayedName.empty() == false)
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ this->displayedName });
	if (this->description.empty() == false)
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ this->description });
	if (this->rollover.empty() == false)
	{
		gui::ImGuiTooltip::TooltipElementColor_s rolloverColor{ 255, 0, 0, 255 };  // Set red as color for rollover
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ this->rollover, std::move(rolloverColor) });
	}

	// icon size and text color
	const gui::ImGuiTooltip::TooltipElementImageSize_s iconSize{ 30, 30 };
	const gui::ImGuiTooltip::TooltipElementColor_s iconTextColor{ 255, 255, 255, 255 }; 

	if (this->goldCost > 0)
	{
		auto goldImage = imageShader.get_ui_image_data(gold_image_path);
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ std::to_string(this->goldCost), iconTextColor, std::move(goldImage), iconSize });
	}
	if (this->foodCost > 0)
	{
		auto foodImage = imageShader.get_ui_image_data(food_image_path);
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ std::to_string(this->foodCost), iconTextColor, std::move(foodImage), iconSize });
	}
	if (this->populationCost > 0)
	{
		auto populationImage = imageShader.get_ui_image_data(population_image_path);
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ std::to_string(this->populationCost), iconTextColor, std::move(populationImage), iconSize });
	}
	if (this->staminaCost > 0)
	{
		auto staminaImage = imageShader.get_ui_image_data(stamina_image_path);
		tooltipInfo.push_back(gui::ImGuiTooltip::TooltipElement_s{ std::to_string(this->staminaCost), iconTextColor, std::move(staminaImage), iconSize });
	}
	
	return tooltipInfo;
}

void Command::CatchRolloverForBottomBar(void)
{
	assert(this->bValid == true);
	this->ResetRollover();
	if (this->bIsUICommand == false)
	{
		static Engine& engine = Engine::GetInstance();
		auto igame = engine.GetEnvironment()->AsIGame();
		igame->Selos()->ExecuteCommand(this->id, true);
	}
}

void Command::Execute(void)
{
	assert(this->bValid == true);
	static Engine& engine = Engine::GetInstance();
	auto igame = engine.GetEnvironment()->AsIGame();

	if (this->ExecuteMethodOnClick() == true)
	{
		igame->Selos()->ExecuteCommand(this->id, false);
	}
	else if (this->OpensIframeOnClick() == true)
	{
		auto ui = igame->GetUIRef();
		assert(ui);
		ui->OpenIframe(this->iframeToOpen);
	}
}
