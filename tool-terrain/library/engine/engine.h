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
#include <memory>

struct GLFWwindow;

class Terrain;
class user_interface;

namespace rattlesmake
{
	namespace peripherals {
		class viewport;
		class mouse;
		class keyboard;
		class camera;
	};
};

class Engine
{
public:
	Engine();
	~Engine() {}
	bool Initialize(const char* exe_root);
	int Launch();

	

private:
	std::string GetCppVersion();
	std::string GetOpenglVersion();

	std::string root;
	unsigned int WindowWidth = 1280;
	unsigned int WindowHeight = 720;

	std::shared_ptr<Terrain> terrain;
	std::shared_ptr<user_interface> ui;
	
	rattlesmake::peripherals::camera& camera;
	rattlesmake::peripherals::keyboard& keyboard;
	rattlesmake::peripherals::viewport& viewport;
	rattlesmake::peripherals::mouse& mouse;
};
