#include "engine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <filemanager.h>

bool Engine::Initialize(const char* exe_root)
{
	this->root = FileManager::GetFileFolderPath(exe_root) + "\\";
	std::cout << "Exe root: " << this->root << std::endl;

	/* Initialize the library */
	if (!glfwInit())
		return false;

	// size of the window = size of the image
	int imageXSize, imageYSize, n;
	unsigned char* gridImage = 0;// stbi_load("grid.png", &imageXSize, &imageYSize, &n, 0);
	this->WindowWidth = imageXSize * SCALE;
	this->WindowHeight = imageYSize * SCALE;
	//stbi_image_free(gridImage);

	/* Create a windowed mode window and its OpenGL context */
	this->window = glfwCreateWindow(this->WindowWidth, this->WindowHeight, "Centurion Pathfinding", NULL, NULL);
	if (!this->window)
	{
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(this->window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		return false;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(this->window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsLight(); // Setup Dear ImGui style

	this->grid.Initialize();

	return true;
}

int Engine::Launch()
{
	std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Running OpenGL Version " + GetOpenglVersion() << std::endl;
	std::cout << "C++ Compiler Version: " + GetCppVersion() << std::endl;

	while (!glfwWindowShouldClose(this->window))
	{
		/* Poll for and process events */
		glfwPollEvents();

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Clear buffer */
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.f, 1.f, 0.f, 1.f);

		// Render here
		this->grid.Render(this->WindowWidth, this->WindowHeight);

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);
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
