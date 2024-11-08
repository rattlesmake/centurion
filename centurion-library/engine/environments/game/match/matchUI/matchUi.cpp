#include "matchUi.h"
#include "engine.h"
#include <fileservice.h>
#include <zipservice.h>

#include <mouse.h>
#include <png_shader.h>

#include <GLFW/glfw3.h>

MatchUI::MatchUI(void)
{
	this->topBar = std::unique_ptr<MatchUI_TopBar>(new MatchUI_TopBar{ (*this) });
	this->bottomBar = std::unique_ptr<MatchUI_BottomBar>(new MatchUI_BottomBar{ (*this) });
	this->env = IEnvironment::Environments::e_match;

	// read images
	rattlesmake::image::png_shader::get_instance().clear_cache();

	std::string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "interface/match_ui.xml");

	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
		throw std::runtime_error("MatchUI XML corrupted");  // TODO exception - aggiungere apposita eccezione
	tinyxml2::XMLElement* matchUiXml = xmlFile.FirstChildElement("matchUi");

	this->EvaluatePythonScripts("?data.zip", "interface/match_ui.py");
	this->CreateFromXML(matchUiXml);

	// Create Topbar and Bottombar:
	this->topBar->Create();
	this->bottomBar->Create(matchUiXml);
}

MatchUI::~MatchUI(void)
{
}

void MatchUI::SetOwnerWeakRef(const std::shared_ptr<Match>& _owner)
{
	assert(_owner);
	this->topBar->owner = _owner;
	this->bottomBar->owner = _owner;
}

std::shared_ptr<Command> MatchUI::GetCommandByPriority(const uint8_t priority) const
{
	if (this->bottomBar->commandsMapByPriority.contains(priority) == false)
		return Command::GetInvalidCommand();
	return this->bottomBar->commandsMapByPriority.at(priority);
}

uint8_t MatchUI::GetNumberOfCommands(void)
{
	return static_cast<uint8_t>(this->bottomBar->commandsMapByPriority.size());
}

std::weak_ptr<gui::Iframe> MatchUI::GetIframeById(const std::string& id) const
{
	return (this->iframesMap.contains(id) == true) ? this->iframesMap.at(id) : std::weak_ptr<gui::Iframe>();
}

void MatchUI::Render(void)
{	
	//Try to update topbar
	this->topBar->Update();	

	//Try to update bottombar
	this->bottomBar->Update();

	// Rendering
	this->RenderUIElements(true, true, true);
}

void MatchUI::Clear(void)
{
	// clear commands
	this->bottomBar->commandsMapByPriority.clear();
}

bool MatchUI::IsHovering(void) const
{
	return (rattlesmake::peripherals::mouse::get_instance().GetYPosition() <= this->BottomBarHeight || rattlesmake::peripherals::mouse::get_instance().GetYPosition() >= rattlesmake::peripherals::viewport::get_instance().GetHeight() - this->TopBarHeight);
}
