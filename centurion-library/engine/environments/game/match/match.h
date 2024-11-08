/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once


#include <memory>
#include <optional>
#include <string>

#include <environments/game/match/matchUI/matchUi.h>
#include <environments/game/match/pathfinding/pathfinder.h>
#include <environments/game/igame.h>
#include <environments/game/classes/VFX/vfxCollection.h>

#include <environments/file_save_info.h>
#include <binary_file_reader.h>
#include <binary_file_writer.h>


class GObject;
namespace centurion
{
	namespace game
	{
		class game_pipeline;
		class match_pipeline;
	}
};

class Match : public IGame
{
	friend class centurion::game::match_pipeline;

public:
	explicit Match(const Match& other) = delete;
	Match& operator = (const Match& other) = delete;
	virtual ~Match(void);

	[[nodiscard]] static std::shared_ptr<Match> CreateMatch(const IEnvironment::Environments currentEnv, std::string fileToLoad);

	[[nodiscard]] std::optional<std::weak_ptr<MatchUI>> GetMatchUI(void) const;
	[[nodiscard]] VFXCollection& GetVFXCollection(void);

	#pragma region To scripts members:
	[[nodiscard]] bool IsLoading(void) const noexcept;
	void Load(std::string fileName);
	void Save(void);
	void Quit(void) override;
	std::shared_ptr<GObject> PlaceObject(std::string className, const uint32_t x, const uint32_t y, const uint8_t playerID) const;

	[[nodiscard]] FileSaveInfo GetFileSaveInfo(std::string fileName);
	#pragma endregion
private:
	Match(const IEnvironment::Environments currentEnv, const bool bEmpty = false); //bEmpty = false --> generate random; bEmpty = true -> generate empty

	void CheckSaveLoadMatch(void);
	void SaveMatch(void);
	bool LoadMatch(const IEnvironment::Environments env = IEnvironment::Environments::e_match);
	
	bool bIsCreated = false;
	bool bFromEditor = false;

	std::optional<std::string> fileToSave;
	std::string fileToLoad;

	VFXCollection vfxCollection;
	//An object in charge of finding the path between two points on a grid
	Pathfinder pathfinder;

	//A service used for read a binary file
	BinaryFileReader bfr;

	//A service used for write a binary file
	BinaryFileWriter bfw;

	static Match* currentMatch;
};
