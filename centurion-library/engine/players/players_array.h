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

#include <binary_file_reader.h>
#include <binary_file_writer.h>

#include <environments/game/classes/objectsSet/settlement_collection.h>

class Player;
class ColorsArray;
class GObject;
class Playable;
class Building;
class Unit;
class Hero;
class PlayersSettings;
class Surface;

class PlayersArray
{
public:
	//The index of the player that owns all the GObjects without a player (i.e. the decorations).
	const static uint8_t UNDEFINED_PLAYER_INDEX = 0;
	const static uint8_t NUMBER_OF_PLAYERS = 16;

	#pragma region Constructors and destructor:
	PlayersArray(classesData_t _classesDataSP, const std::shared_ptr<ColorsArray>& colorsArray);
	PlayersArray(classesData_t _classesDataSP, const uint8_t nPlayers, const std::list<pair<uint8_t, glm::vec3>>& colorsArray, const std::list<std::string>& racesArray);
	PlayersArray(const PlayersArray& other) = delete;
	~PlayersArray(void);
	#pragma endregion

	#pragma region Operators:
	PlayersArray& operator=(const PlayersArray& other) = delete;
	friend std::ostream& operator<<(std::ostream& out, const PlayersArray& m) noexcept;
	[[nodiscard]] std::shared_ptr<Player> operator[](const uint32_t idx) const noexcept;
	#pragma endregion

	static void SetTabs(const uint8_t tabs);
	[[nodiscard]] static uint8_t GetArraySize(void);
	[[nodiscard]] static uint8_t GetMaxPlayersNumberAllowed(void);

	#pragma region To Python:
	uint8_t GetAllowedPlayersNumber(void);
	#pragma endregion

	[[nodiscard]] SettlementsCollection& GetSettlementsCollectionRef(void);

	#pragma region Serialization and deserialization
	void SaveAsBinaryFile(BinaryFileWriter& bfw);
	void DeserializeFromBinFile(BinaryFileReader& bfr, const std::shared_ptr<Surface>& surface);
	void DeserializeFromXML(tinyxml2::XMLElement& playersTag, const std::shared_ptr<Surface>& surface);

	void AddBuildingHavingNotEmptyGarrison(const Building& b);
	void AddHeroHavingNotEmptyArmy(const Hero& h);
	void AddUnitHavingAnActiveBehavior(const Unit& u);  // Behavior aka status
	#pragma endregion

	void OpenObjPropsIframe(const GObject& focusedObj) const;
	void Play(const bool onlyLogic, const bool bMinimapIsActive);
	void Picking(void);
	[[nodiscard]] uint8_t GetCurrentPlayerID(void) const;

	[[nodiscard]] std::array<std::shared_ptr<Player>, NUMBER_OF_PLAYERS + 1>& GetPlayersArrayRef(void);

	void ApplyPlayersSettings(PlayersSettings& playersSettings);
	void SetEffectiveNumberOfPlayers(const uint8_t numberOfPlayers);
	[[nodiscard]] uint8_t GetEffectiveNumberOfPlayers(void) const;
	[[nodiscard]] std::weak_ptr<Player> GetPlayerRef(const uint8_t id);
	[[nodiscard]] std::list<pair<uint8_t, glm::vec3>> GetPlayersColorsList(void) const;
	[[nodiscard]] std::list<std::string> GetPlayersRacesList(void) const;

	void AddGObject(std::shared_ptr<GObject> gobj);
	bool UpdateGObjectScriptId(const std::shared_ptr<Playable>& gobj, scriptIdName_t newScriptId);
	void RemoveGObject(const std::shared_ptr<GObject>& gobj);

	[[nodiscard]] classesData_t GetClassesDataSp(void) const noexcept;
private:
	void HandleClickForCurrentPlayer(const pickingID_t clickingID) const;
	void HandleSelectionForCurrentPlayer(Player& currentPlayer, const bool bClickedOnTerrain, std::shared_ptr<GObject> focusedObj) const;

	// A shared pointer to the classes info previous read from xml files
	classesData_t classesDataSP;

	// The current PC player's ID.
	uint8_t currentPlayer = 1;  // TODO: settare il valore corretto.

	// An array containing all the players. We have an additional element because the index 0 rapresents the player who contains GObjects without a player (i.e. decorations).
	std::array<std::shared_ptr<Player>, NUMBER_OF_PLAYERS + 1> players;

	// The players number in a match.
	uint8_t effectiveNumberOfPlayers = 0;

	// A map (for all the players) that associates a GObject with a STATIC ID (i.e. unique ID).
	std::unordered_map<uniqueID_t, std::weak_ptr<GObject>> objsByUniqueId;

	// A map (for all the players) that associates a GObject with a DYNAMIC ID (i.e. picking ID).
	std::unordered_map<pickingID_t, uniqueID_t> objsByPickingId;

	// A map (for all the players) that associates a GObject with a SCRIPT ID.
	std::unordered_map<scriptIdName_t, uniqueID_t> objsByIdName;

	// A collection of all the settlements of the scenario.
	SettlementsCollection settlementCollection;

	static uint8_t nTabs;

	// This struct contains some info to use in order to proper save heroes' army and buildings' garrison.
	// For this reason, we have an unique_ptr that should be allocated only inside SaveAsBinaryFile and DeserializeFromBinFile.
	// The two aforementioned methods, when completed or in the event of an exception, should deallocate the unique_ptr.
	struct AdditionalPlayableGObjectsInfo
	{
		// An ORDERED set containt the unique IDs of all the heroes having at least one unit attached.
		std::set<uniqueID_t> heroesHavingNotEmptyArmy;
		// An ORDERED set containt the unique IDs of all the buildings having at least one unit inside.
		std::set<uniqueID_t> buildingsHavingNotEmptyGarrison;
		// An ORDERED set of units that have an ACTIVE status (i.e. status like moving, fighting; otherwise, for instance, IDLE isn't an active status)
		std::set<uniqueID_t> unitsHavingAnActiveStatus;
	};
	std::unique_ptr<AdditionalPlayableGObjectsInfo> additionalPlayableGObjectsInfo;
};
