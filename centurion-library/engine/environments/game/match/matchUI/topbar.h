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
#include <environments/game/classes/objectsStuff/commandsQueue.h>

#include <xml_class_enums.h>

class OrderedUnitsList;
class Building;
class Match;
class MatchUI;

///This class controls the top bar during matches.
///The background depends on the player's race.
///Icons, texts and buttons depend on selected objects.
class MatchUI_TopBar
{
public:
	//A reference to the owner of the UI.
	std::weak_ptr<Match> owner;

	explicit MatchUI_TopBar(MatchUI& my_creator);
	MatchUI_TopBar(const MatchUI_TopBar& other) = delete;
	MatchUI_TopBar& operator=(const MatchUI_TopBar& other) = delete;
	~MatchUI_TopBar(void);

	void Create(void);
	void Update(void);
private:
	enum class SpawnButton
	{
		e_garrison = 1,
		e_technologies,
		e_trainings,
	};

	struct SpawnButtonInfo
	{
		SpawnButtonInfo(void) = default;
		SpawnButtonInfo(std::string _pathID) : pathID(std::move(_pathID)), bToConsider(false) { }
		SpawnButtonInfo(SpawnButtonInfo&& other) noexcept : pathID(std::move(other.pathID)), bToConsider(other.bToConsider) { }
		SpawnButtonInfo(const SpawnButtonInfo& other) = delete;
		SpawnButtonInfo& operator=(SpawnButtonInfo& other) = delete;
		std::string pathID;
		bool bToConsider = false;
	};
	static std::map<SpawnButton, SpawnButtonInfo> spawnButtonsInfo;

	struct UI_Elements
	{
		uint8_t placedButtons = 0;
		float buttonWidth = 90; //rattlesmake::peripherals::viewport::get_instance().GetWidth() * 0.075f;
		float buttonHeight = 78; // rattlesmake::peripherals::viewport::get_instance().GetHeight() * 0.12f;
		float startXPos = rattlesmake::peripherals::viewport::get_instance().GetWidth() / 2.5f;
		float xPos = startXPos;
		float yPos = 0;
		string pythonOnClickCMD;
		string imageName;
		string buttonText;
		uint8_t barScaling = 10;
	};
	uint8_t stackToShow = 0;

	void UpdateLeftSide(void);
	void UpdateRightSide(void);
	void UpdateBuildingRightSide(UI_Elements& ui, const Building& selb);
	void SetStackToShow(const uint8_t idSpawnBtn);
	void Reset(void);
	void PrintOrderedUnitsList(UI_Elements& ui, const OrderedUnitsList& orderedUnitsList, const bool bIsGarrisonStack);
	void PrintCommandsQueue(UI_Elements& ui, CommandsQueue& cmdsQueue, const assets::xml_command_type type);

	std::list<std::pair<gui::ImGuiElement::elementID_t, gui::ImGuiElement::elementID_t>> valuesIDs;

	std::weak_ptr<gui::Iframe> iframe;
	MatchUI& myCreator;
};
