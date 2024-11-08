/*
* ---------------------------
* CLASS READER AND EDITOR FOR CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iostream>
#include <sstream>

#include "grid_graphic.h"

struct GLFWwindow;

class Engine
{
public:
	Engine() {}
	~Engine() {}
	bool Initialize(const char* exe_root);
	int Launch();
private:
	std::string GetCppVersion();
	std::string GetOpenglVersion();
	GLFWwindow* window;
	GridGraphic grid;
	std::string root;
	unsigned int WindowWidth;// 864;//640;
	unsigned int WindowHeight;

	// change this parameter to fix the windows size
	float SCALE = 0.7f; 
};
