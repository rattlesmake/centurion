#pragma once

namespace rattlesmake
{
	namespace peripherals {
		class viewport;
		class mouse;
		class keyboard;
		class camera;
	};
};

class Terrain;

class user_interface
{
public:
	user_interface();
	~user_interface();
	void render(Terrain* terrain);

private:

	rattlesmake::peripherals::camera& camera;
	rattlesmake::peripherals::keyboard& keyboard;
	rattlesmake::peripherals::viewport& viewport;
};
