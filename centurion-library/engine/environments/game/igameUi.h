/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <ui.h>

class EditorUI;
class MatchUI;

class IGameUI : public gui::UI
{
public:
	IGameUI(void);
	IGameUI(const IGameUI& other) = delete;
	IGameUI& operator=(const IGameUI& other) = delete;
	~IGameUI(void);

	EditorUI* AsEditorUI(void);
	MatchUI* AsMatchUI(void);

	[[nodiscard]] std::string GetInfoText(void) const;
	void UpdateInfoText(std::string& infoText);

protected:
	std::string infoText;
};
