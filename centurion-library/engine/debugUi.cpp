#include "debugUi.h"
#include "engine.h"

#include <mouse.h>
#include <viewport.h>
#include <keyboard.h>

#include <environments/game/igame.h>
#include <services/logservice.h>
#include <services/sqlservice.h>
#include <GLFW/glfw3.h>

DebugUI::DebugUI(void)
{
	this->debuguiIsActive = true;
}

DebugUI::~DebugUI(void)
{
}

void DebugUI::Render(void)
{
	if (rattlesmake::peripherals::keyboard::get_instance().IsKeyPressed(GLFW_KEY_F10))
	{
		this->setStatus(!this->getStatus());
		this->getStatus() ? Logger::Info("Debug UI ON!") : Logger::Info("Debug UI OFF!");
	}

	if (debuguiIsActive == false || Settings::GetInstance().GetGlobalPreferences().IsDebugActive() == false) return;
	
	ImGui::SetNextWindowSize(ImVec2(200, 220), ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2(10, rattlesmake::peripherals::viewport::get_instance().GetHeight() - 232), ImGuiCond_Once);
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::PushFont(ImGui::GetFontByName(SqlService::GetInstance().GetFontByContext("DebugWindow")));
	ImGui::Begin("Debugging information", NULL, ImGuiWindowFlags_NoResize);
	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Press F10 to hide/show");
	
	std::stringstream ssFps;
	ssFps << "FPS: " << Engine::GetInstance().fps.GetFps();
	ImGui::Text(ssFps.str().c_str());
	
	std::stringstream ssMpfs;
	ssMpfs << "MPFS: " << Engine::GetInstance().fps.GetMpfs();
	ImGui::Text(ssMpfs.str().c_str());
	
	std::stringstream ssXWind;
	ssXWind << "X window: " << rattlesmake::peripherals::mouse::get_instance().GetXPosition();
	ImGui::Text(ssXWind.str().c_str());
	
	std::stringstream ssYWind;
	ssYWind << "Y window: " << rattlesmake::peripherals::mouse::get_instance().GetYPosition();
	ImGui::Text(ssYWind.str().c_str());

	std::stringstream ssXMap;
	ssXMap << "X map: " << (int)rattlesmake::peripherals::mouse::get_instance().GetXMapCoordinate();
	ImGui::Text(ssXMap.str().c_str());

	std::stringstream ssYMap;
	ssYMap << "Y map: " << (int)rattlesmake::peripherals::mouse::get_instance().GetYMapCoordinate();
	ImGui::Text(ssYMap.str().c_str());

	/*std::stringstream ssY2DMap;
	ssY2DMap << "Y 2d map: " << (int)rattlesmake::peripherals::mouse::get_instance().GetY2DMapCoordinate();
	ImGui::Text(ssY2DMap.str().c_str());*/
	
	if (Engine::GetInstance().GetEnvironmentId() != IEnvironment::Environments::e_menu)
		ImGui::Text(("GameTime: " + Engine::GetGameTime().GetFullTimeString()).c_str());
	ImGui::PopFont();
	ImGui::End();
}
