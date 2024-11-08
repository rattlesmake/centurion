/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <header.h>

#include <centurion_typedef.hpp>

#include "color.h"

#include <environments/game/races/race.h>
#include <environments/game/classes/objectsSet/objects_collection.h>
#include <environments/game/classes/objectsSet/selected_objects.h>

class Surface;

///SEE ALSO PlayerSettings (TODO)

class Player
{
public:
	#pragma region Constructors and destructor:
	Player(const uint8_t _playerdID, const Color& _defaultColor, const PlayersArray& ref);
	Player(const Player& other) = delete;
	~Player(void);
	#pragma endregion

	#pragma region Operators:
	Player& operator=(const Player& other) = delete;
	friend std::ostream& operator<<(std::ostream& out, const Player& player) noexcept;
	#pragma endregion

	static void SetTabs(const uint8_t tabs);

	void DeserializeFromXML(const classesData_t& classesDataSP, tinyxml2::XMLElement& playerTag, const std::shared_ptr<Surface>& surface);
	[[nodiscard]] bool IsCurrentPlayer(void) const;
	[[nodiscard]] std::shared_ptr<SelectedObjects> GetSelection(void) const;

	//TODO - move maybe
	void SetName(std::string name);
	[[nodiscard]] std::string GetName(void) const;
	void SetRace(std::string raceName);
	[[nodiscard]] std::string GetRace(void) const;
	void SetColor(Color& color);
	[[nodiscard]] Color& GetColor(void);
	void SetBonus(const uint8_t bonus);
	[[nodiscard]] uint8_t GetBonus(void) const;
	void SetStartingPoint(glm::vec2 point);
	[[nodiscard]] glm::vec2 GetStartingPoint(void) const;

	[[nodiscard]] const std::unique_ptr<ObjsCollection>& GetObjsCollection(void) const;
private:
	//TODO
	bool bIsHuman = false;
	bool bIsAnotherPlayer = true;

	//The objs belonging to the current player.
	std::unique_ptr<ObjsCollection> playerObjs;

	//The GObjects selected by this player.
	std::shared_ptr<SelectedObjects> selection;

	//The selected object (it can be both a unit and a building).
	std::weak_ptr<GObject> SELO;

	//The selected unit.
	std::weak_ptr<Unit> SELU;

	//The selected building.
	std::weak_ptr<Building> SELB;

	//Tabs to use during serialization (operator<<)
	static uint8_t nTabs;

	//TODO - Forse muovere da un'altra parte.
	uint8_t bonus = 0;
	uint8_t id = 0;
	uint8_t type = 0;
	std::string name;
	std::weak_ptr<Race> race;
	Color color;
	glm::vec2 startingPoint{ 0,0 };

	const PlayersArray& playersRef;
};

