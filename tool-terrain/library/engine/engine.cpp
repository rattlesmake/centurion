#include "engine.h"
#include "user_interface.h"
#include "terrain.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <fileservice.h>

#include <camera.h>
#include <keyboard.h>
#include <viewport.h>
#include <mouse.h>

Engine::Engine() :
	camera(rattlesmake::peripherals::camera::get_instance()),
	keyboard(rattlesmake::peripherals::keyboard::get_instance()),
	viewport(rattlesmake::peripherals::viewport::get_instance()),
	mouse(rattlesmake::peripherals::mouse::get_instance())
{
}

bool Engine::Initialize(const char* exe_root)
{
	rattlesmake::services::file_service::get_instance().initialize(exe_root, { });

	// Initialize GLFW (important to be the second operation)
	if (glfwInit() != GLFW_TRUE)
		return false;
	
	viewport.initialize(WindowWidth, WindowHeight, false, "Centurion Terrain Tool");
	camera.initialize(10, 10, -2000.f, -2000.f);
	keyboard.initialize();
	this->terrain = std::shared_ptr<Terrain>(new Terrain());
	glPatchParameteri(GL_PATCH_VERTICES, 4);

	this->ui = std::shared_ptr<user_interface>(new user_interface());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(viewport.GetGlfwWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsLight(); // Setup Dear ImGui style

	return true;
}

int Engine::Launch()
{
	std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Running OpenGL Version " + GetOpenglVersion() << std::endl;
	std::cout << "C++ Compiler Version: " + GetCppVersion() << std::endl;

	int frames = 0;

	auto map_size = glm::vec2(30000, 30000);
	camera.new_game(map_size, 0, 0);
	camera.set_position(glm::vec2(-200, -200));

	while (viewport.GetShouldClose() == false)
	{
		viewport.begin_engine_frame(ImGui::GetIO().WantCaptureMouse);
		mouse.begin_engine_frame(frames);
		camera.begin_engine_frame(frames);

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		camera.begin_game_frame(false, false, ImGui::GetIO().WantCaptureMouse == false);

		// Render here
		this->terrain->Draw(camera.GetProjectionMatrix(), camera.GetViewMatrix());

		this->ui->render(this->terrain.get());

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		viewport.end_engine_frame();
		keyboard.end_engine_frame();
		mouse.end_engine_frame();

		frames++;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 1;
}

std::string Engine::GetCppVersion()
{
	std::string version;
	if (__cplusplus > 201703L) version = "C++20";
	else if (__cplusplus == 201703L) version = "C++17";
	else if (__cplusplus == 201402L) version = "C++14";
	else if (__cplusplus == 201103L) version = "C++11";
	else if (__cplusplus == 199711L) version = "C++98";
	else version = "pre-standard C++";
	return version;
}

std::string Engine::GetOpenglVersion()
{
	std::ostringstream ss;
	ss << glGetString(GL_VERSION);
	return ss.str();
}
