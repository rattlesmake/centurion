/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>
#include <environments/ienvironment.h>
#include <functional>

namespace gui
{
	void NewQuestionWindow(std::string&& message, std::string&& functionYes, std::string&& functionNo, IEnvironment::Environments env);

	void NewQuestionWindow(std::string&& message, std::function<void()>& functionYes, std::function<void()>& functionNo, IEnvironment::Environments env);
	
	void NewInfoWindow(std::string&& message, IEnvironment::Environments env);

	void RenderDialogWindows(void);

	bool IsAnyDialogWindowActive(void);

	bool IsQuestionWindowActive(void);
};
