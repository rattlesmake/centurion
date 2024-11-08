/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <vector>
#include <string>
#include <tuple>

class AddObjectWindow
{
public:
	void Initialize(const std::vector<std::pair<std::string, std::string>>& source);
	void CheckIfOpen();
	bool IsOpened(void);
	void Render();
private:
	
	struct ObjSum // summary
	{
		std::string className = "";
		bool selected = false;
	};
	std::vector<ObjSum> values;

	float posX = 0.f, posY = 0.f, sizeX = 250.f, sizeY_listBox = 100.f;
	std::string filter = "";
	char input_buf[256] = { '\0' };
	bool bIsOpened = false;
	bool bIsOpening = false;

	bool bIsInserting = false;
};
