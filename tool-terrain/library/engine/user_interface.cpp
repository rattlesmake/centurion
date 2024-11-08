#include "user_interface.h"
#include "terrain.h"

#include <camera.h>
#include <keyboard.h>
#include <viewport.h>

#include <imgui.h>

user_interface::user_interface() :
	camera(rattlesmake::peripherals::camera::get_instance()),
	keyboard(rattlesmake::peripherals::keyboard::get_instance()),
	viewport(rattlesmake::peripherals::viewport::get_instance())
{
}
user_interface::~user_interface()
{
}

void user_interface::render(Terrain* terrain)
{
	ImGui::SetNextWindowPos(ImVec2(viewport.GetWidth() - 300, 0));
	ImGui::SetNextWindowSize(ImVec2(300, 500));
	ImGui::Begin("Terrain Tool");
	{
		ImGui::Text(std::string("Camera Position: [" + std::to_string((int)camera.GetXPosition()) + "; " + std::to_string((int)camera.GetYPosition()) + "]").c_str());
		ImGui::Text(std::string("Current Zoom: " + std::to_string(camera.GetCurrentZoom())).c_str());

		ImGui::SliderInt("Outer", &terrain->tessellation_value_outer, 1, 16);
		ImGui::SliderInt("Inner", &terrain->tessellation_value_inner, 1, 16);
		ImGui::SliderInt("Vertices Gap Px", &terrain->vertices_gap_px, 1, 400);

		ImGui::Checkbox("Wireframe", &terrain->wireframe);
	}
	ImGui::End();
}
