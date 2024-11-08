/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include "../igameUi.h"

class AddObjectWindow;
class TerrainBrush;

class EditorUI : public IGameUI
{
	friend class Editor;

public:
	~EditorUI(void);
	explicit EditorUI(const EditorUI& other) = delete;
	EditorUI& operator=(const EditorUI& other) = delete;

	// ui methods
	void Render(void) override;
private:
	EditorUI(const std::vector<std::pair<std::string, std::string>>& editorTreeSource);
	
	// editor tree
	void InitializeTree(const std::vector<std::pair<std::string, std::string>>& source);

	std::weak_ptr<gui::Iframe> tree;

	std::string infoText;
	uint8_t luaScriptsCounter = 0;
	std::shared_ptr<AddObjectWindow> addObjectWindow;
	std::shared_ptr<TerrainBrush> terrainBrush;
};
