/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <viewport.h>
#include <iframe/iframe.h>

#include <centurion_typedef.hpp>

class Match;
class MatchUI;
class Command;

///This class controls the bottom bar during matches.
///The background depends on the player's race.
///Icons, texts and buttons depend on selected objects.
///A list of default commands is displayed if nothing is selected.
class MatchUI_BottomBar
{
public:
	//A reference to the owner of the UI.
	std::weak_ptr<Match> owner;

	explicit MatchUI_BottomBar(MatchUI& my_creator);
	MatchUI_BottomBar(const MatchUI_BottomBar& other) = delete;
	MatchUI_BottomBar& operator=(const MatchUI_BottomBar& other) = delete;
	~MatchUI_BottomBar(void);

	void Create(tinyxml2::XMLElement* commandsXML);
	void Update(void);

	// TODO perché è pubblica?
	std::unordered_map<uint16_t, std::shared_ptr<Command>> commandsMapByPriority;
private:
	const static uint8_t MAX_NUMBER_OF_CMDS = 14;
	std::weak_ptr<gui::Iframe> iframe;
	struct UI_Elements
	{
		uint8_t nButtons = 0;
		float buttonSize = rattlesmake::peripherals::viewport::get_instance().GetHeight() * 0.10f;
		std::shared_ptr<Command> cmd;
		ImVec2 pos;
	};
	MatchUI& myCreator;
	std::list<assets::xmlCmdUP_t> commandsUI;
};
