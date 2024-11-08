#include "editorUi.h"
#include "editor.h"
#include "addObjectWindow.h"

#include <engine.h>
#include <fileservice.h>
#include <services/pyservice.h>
#include <zipservice.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainBrush.h>

#include <dialogWindows.h>

#include <png_shader.h>

EditorUI::EditorUI(const std::vector<std::pair<std::string, std::string>>& editorTreeSource)
{
	this->env = IEnvironment::Environments::e_editor;

	// read images
	rattlesmake::image::png_shader::get_instance().clear_cache();

	string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "interface/editor_ui.xml");

	tinyxml2::XMLDocument xmlFile;
	auto error = xmlFile.Parse(xmlText.c_str());
	if (error != tinyxml2::XML_SUCCESS)
		throw std::exception("Editor UI XML file is corrupted.");

	tinyxml2::XMLElement* editorUiXml = xmlFile.FirstChildElement("editorUi");
	this->EvaluatePythonScripts("?data.zip", "interface/editor_ui.py");
	this->CreateFromXML(editorUiXml);
	this->InitializeTree(editorTreeSource);
	this->addObjectWindow = std::shared_ptr<AddObjectWindow>(new AddObjectWindow());
	this->addObjectWindow->Initialize(editorTreeSource);
	this->terrainBrush = std::shared_ptr<TerrainBrush>(new TerrainBrush());
}
EditorUI::~EditorUI(void)
{
	#if CENTURION_DEBUG_MODE
	std::cout << "EditorUI destructor" << std::endl;
	#endif
}

void EditorUI::InitializeTree(const std::vector<std::pair<std::string, std::string>>& source)
{
	assert(this->iframesMap.size() > 0);
	std::function<void(std::string par1, unsigned int par2)> fun = [](std::string par1, unsigned int par2) {
		Engine::GetInstance().GetEnvironment()->AsEditor()->InsertObject(par1, par2);
	};
	this->iframesMap["__tree__"]->AddEditorTree(source, fun);
	this->tree = this->iframesMap["__tree__"];
}

void EditorUI::Render(void)
{
	bool bAdvOpened = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentAdventure()->IsOpened();

	this->addObjectWindow->Render();

	this->terrainBrush->Render();

	this->RenderUIElements(true, true, bAdvOpened);
}
