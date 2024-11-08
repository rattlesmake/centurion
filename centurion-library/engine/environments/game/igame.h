/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <environments/ienvironment.h>
#include <environments/game/adventure/adventure.h>

class IGameUI;
class GObject;
class Playable;
class Building;
class Unit;
class Hero;
class ObjsList;
class SelectedObjects;

// shaders
class terrainchunk_shader;
class sea_shader;
class clouds_shader;

namespace rattlesmake
{
	namespace geometry
	{
		class circle_shader;
		class rectangle_shader;
	};
};

namespace gui { class Iframe; };

namespace centurion
{
	namespace assets
	{
		class xml_assets;
		class xml_entity_shader;
	};
	namespace game
	{
		class game_pipeline;
	}
};

class IGame : public IEnvironment
{
	friend class centurion::game::game_pipeline;

public:
	explicit IGame(const IGame& other) = delete;
	IGame& operator = (const IGame& other) = delete;
	virtual ~IGame(void);

#pragma region To external scripts
	[[nodiscard]] std::shared_ptr<Adventure> GetCurrentAdventure(void) const;
	[[nodiscard]] std::shared_ptr<Scenario> GetCurrentScenario(void) const;
	[[nodiscard]] uint32_t GetNumberOfSelectedObjects(void) const;
	[[nodiscard]] std::shared_ptr<GObject> Selo(void) const;
	[[nodiscard]] std::shared_ptr<Playable> Selp(void) const;
	[[nodiscard]] std::shared_ptr<Building> Selb(void) const;
	[[nodiscard]] std::shared_ptr<Settlement> Sels(void) const;
	[[nodiscard]] std::shared_ptr<Unit> Selu(void) const;
	[[nodiscard]] std::shared_ptr<Hero> Selh(void) const;
	[[nodiscard]] std::shared_ptr<ObjsList> Selos(void) const;
	[[nodiscard]] std::shared_ptr<SelectedObjects> Selection(void) const;
	[[nodiscard]] std::shared_ptr<SelectedObjects> SelectedUnits(void) const;
#pragma endregion

	[[nodiscard]] std::shared_ptr<IGameUI> GetUIRef(void) const noexcept;

	[[nodiscard]] std::shared_ptr<centurion::assets::xml_assets> GetXmlAssets(void) const noexcept;

	/// <summary>
	/// Get a reference to entire iframes map of the current UI (editor or match).
	/// </summary>
	std::unordered_map<std::string, std::shared_ptr<gui::Iframe>>& GetIframes(void);

	/// <summary>
	/// Shortcut for checking if a given player id is the current/active one 
	/// </summary>
	[[nodiscard]] bool IsCurrentPlayer(const uint8_t playerId) const noexcept;

	virtual void ExecuteInternalMethods(void);
	void Run(void) override;

	/// <summary>
	/// In Cpp code, prefer this function rather than GetCurrentAdventure.
	/// </summary>
	[[nodiscard]] std::weak_ptr<Adventure> GetCurrentAdventureW(void) const noexcept;

protected:
	IGame(const IEnvironment::Environments type, const IEnvironment::Environments previousEnv, const bool bEmptyForMatch = false);

	/* All the following attributes and members are protected since this class is a simple interface. */

	std::shared_ptr<Adventure> adventure;
	std::shared_ptr<IGameUI> UI;

	std::shared_ptr<centurion::assets::xml_assets> xmlAssets;

	// shaders
	centurion::assets::xml_entity_shader& xml_entity_shd;
	rattlesmake::geometry::circle_shader& circle_shd;
	rattlesmake::geometry::rectangle_shader& rectangle_shd;
	sea_shader& sea_shd;
	clouds_shader& clouds_shd;
	terrainchunk_shader& terrain_chunk_shd;
	void shaders_begin_frame(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const float total_seconds, const bool minimap_is_active);

	bool firstRun = true;
	bool bIsPaused = false;
};
