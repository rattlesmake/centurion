#include "loading_screen.h"
#include <fileservice.h>
#include <chrono>

#include <environments/game/adventure/scenario/surface/surface_grid/surface_grid.h>
#include <zipservice.h>

#pragma region Construction and destruction of a singleton instance:
LoadingScreen * LoadingScreen::loadingScreen = nullptr;

LoadingScreen::LoadingScreen(void)
{
	this->Create();
}

LoadingScreen* LoadingScreen::GetInstance(void)
{
	if (LoadingScreen::loadingScreen == nullptr)
	{
		LoadingScreen::loadingScreen = new LoadingScreen();
	}
	return LoadingScreen::loadingScreen;
}

void LoadingScreen::DeleteInstance(void)
{
	if (LoadingScreen::loadingScreen != nullptr)
	{
		delete LoadingScreen::loadingScreen;
		LoadingScreen::loadingScreen = nullptr;
	}
}

#pragma endregion

#pragma region Public members:
bool LoadingScreen::IsLoading(void) const
{
	return this->bIsLoading;
}

void LoadingScreen::Init(std::tuple<std::function<void(bool, string)>, bool, string> f)
{
	if (std::get<0>(f) && this->bIsLoading == false) //If f has a callable function (i.e. std::get<0>(f) is not empty), then
	{
		this->funct = std::move(std::get<0>(f));
		//The execution of the parallel thread will start shortly.
		this->bIsLoading = true;
		this->futureToExecuteInParallel = std::async(std::launch::async, this->funct, std::get<1>(f), std::get<2>(f)); //Function will be executed in parallel in a new thread.
	}
}

void LoadingScreen::Run(void)
{
	this->Render();

	const std::chrono::milliseconds span(1);
	if (this->futureToExecuteInParallel.wait_for(span) == std::future_status::ready)
	{
		this->bIsLoading = false; //The execution of the parallel thread has been finished.
		//Grid::UpdateGrid(); //Update the grid after the loading of the map
	}
}

void LoadingScreen::SetLoadingElementName(string&& name)
{
	//todo this->iframe->UpdateTextById(0, std::move(name));
}

void LoadingScreen::SetTip(string&& tip)
{
	//todo this->iframe->UpdateTextById(1, std::move(tip));
}
#pragma endregion

#pragma region Private members:
void LoadingScreen::Create(void)
{
	string xmlText = rattlesmake::services::zip_service::get_instance().get_text_file("?data.zip", "interface/loading_screen.xml");

	//XML Loading screen reading
	tinyxml2::XMLDocument xmlFile;
	if (xmlFile.Parse(xmlText.c_str()) != tinyxml2::XML_SUCCESS)
		return;

	tinyxml2::XMLElement* loadingScreenXML = xmlFile.FirstChildElement("loadingScreen");
	tinyxml2::XMLElement* frame = nullptr;

	if (loadingScreenXML != nullptr)
	{
		tinyxml2::XMLElement* _iframesXML = loadingScreenXML->FirstChildElement("iframeArray");
		if (_iframesXML != nullptr)
		{
			frame = _iframesXML->FirstChildElement();
		}
	}

	//Creation of the iframe
	if (frame == nullptr)
		return;
	this->iframe = gui::Iframe::CreateIframe(IEnvironment::Environments::e_unknown, frame);
	xmlFile.Clear();
}

void LoadingScreen::Render(void)
{
	this->iframe->Render();
}

#pragma endregion
