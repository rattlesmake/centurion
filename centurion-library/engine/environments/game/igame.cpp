#include "igame.h"
#include "igameUi.h"
#include "game_pipeline.h"
#include "editor/editor_pipeline.h"
#include "match/match_pipeline.h"

#if CENTURION_DEBUG_MODE
#include <iostream>
#endif

//Engine
#include <engine.h>
#include <settings.h>

//Classes
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/hero.h>

//Players:
#include <players/player.h>

//Terrain and area
#include <environments/game/adventure/scenario/surface/surface.h>

//Others
#include <fileservice.h>
#include <iframe/iframe.h>

#include <mouse.h>

// assets
#include <xml_assets.h>
#include <xml_entity_shader.h>

#include <rectangle_shader.h>
#include <circle_shader.h>
#include <environments/game/adventure/scenario/surface/sea/sea_shader.h>
#include <environments/game/adventure/scenario/surface/clouds/clouds_shader.h>
#include <environments/game/adventure/scenario/surface/terrain/terrainchunk_shader.h>

#pragma region Constructor and destructor:
IGame::IGame(const IEnvironment::Environments type, const IEnvironment::Environments previousEnv, const bool bEmptyForMatch) :
	IEnvironment(type),
	xml_entity_shd(centurion::assets::xml_entity_shader::get_instance()),
	circle_shd(rattlesmake::geometry::circle_shader::get_instance()),
	rectangle_shd(rattlesmake::geometry::rectangle_shader::get_instance()),
	sea_shd(sea_shader::get_instance()),
	clouds_shd(clouds_shader::get_instance()),
	terrain_chunk_shd(terrainchunk_shader::get_instance()),
	xmlAssets(std::shared_ptr<centurion::assets::xml_assets>(new centurion::assets::xml_assets()))
{
	// Reset game_time:
	Engine::GetGameTime().Reset();

	// Initialization of the adventure
	if (type == IEnvironment::Environments::e_editor)
	{
		auto advName = Settings::GetInstance().GetGlobalPreferences().GetLastAdventure();
		this->adventure = std::shared_ptr<Adventure>{ new Adventure{ xmlAssets->get_xml_classes() } };
		if (advName.empty() == true)
			Adventure::InitEmpty(this->adventure, std::move(advName));
		else
			Adventure::InitFromZip(this->adventure, std::move(advName));
	}
	else if (type == IEnvironment::Environments::e_match)
	{
		// Running from editor = open adventure zip
		if (previousEnv == IEnvironment::Environments::e_editor)
		{
			auto advName = Settings::GetInstance().GetGlobalPreferences().GetLastAdventure();
			assert(advName.empty() == false);  // Last adventure name not set before adventure running

			this->adventure = std::shared_ptr<Adventure>{ new Adventure{ xmlAssets->get_xml_classes() } };
			Adventure::InitFromZip(this->adventure, std::move(advName));
		}
		// running from match menu = create random
		else if (previousEnv == IEnvironment::Environments::e_menu)
		{
			this->adventure = std::shared_ptr<Adventure>{ new Adventure{ xmlAssets->get_xml_classes() } };
			if (bEmptyForMatch == false)  // Random adv (theoretically, if here, I'm will create a random match from main menu)
				Adventure::InitRandom(this->adventure);
			else  // Empty adv. Because it should be replaced with a loaded adventure (theoretically, if here, I'm loading a match from main menu or I will run an existing scenario)
				Adventure::InitEmpty(this->adventure, "");
		}
	}
	assert(this->adventure);  // Adventure not instanced during game construction

	// Reset mouse
	rattlesmake::peripherals::mouse::get_instance().ResetAllButtons();
}
IGame::~IGame(void)
{
	#if CENTURION_DEBUG_MODE
	std::cout << "IGame destructor" << std::endl;
	#endif
}
#pragma endregion

void IGame::shaders_begin_frame(const glm::mat4& projection_matrix, const glm::mat4& view_matrix, const float total_seconds, const bool minimap_is_active)
{
	xml_entity_shd.begin_frame_game(projection_matrix, view_matrix, minimap_is_active, true, total_seconds);
	sea_shd.begin_frame_game(projection_matrix, view_matrix, total_seconds);
	clouds_shd.begin_frame_game(projection_matrix, view_matrix, total_seconds);
	terrain_chunk_shd.begin_frame_game(projection_matrix, view_matrix);
	rectangle_shd.begin_frame_game(projection_matrix, view_matrix);
	circle_shd.begin_frame_game(projection_matrix, view_matrix);
}

#pragma region To external scripts:
std::shared_ptr<Adventure> IGame::GetCurrentAdventure(void) const
{
	return this->adventure;
}

std::shared_ptr<Scenario> IGame::GetCurrentScenario(void) const
{
	return this->adventure->GetScenario();
}

uint32_t IGame::GetNumberOfSelectedObjects(void) const
{
	const auto players_array = this->adventure->GetScenario()->GetPlayersArrayW();
	assert(players_array.expired() == false);
	const auto currentPlayer = players_array.lock()->GetPlayerRef(players_array.lock()->GetCurrentPlayerID());
	assert(currentPlayer.expired() == false);
	return currentPlayer.lock()->GetSelection()->Count();
}

std::shared_ptr<GObject> IGame::Selo(void) const
{
	const auto players_array = this->adventure->GetScenario()->GetPlayersArrayW();
	assert(players_array.expired() == false);
	const auto currentPlayer = players_array.lock()->GetPlayerRef(players_array.lock()->GetCurrentPlayerID());
	assert(currentPlayer.expired() == false);
	return currentPlayer.lock()->GetSelection()->GetSelectedObject();
}

std::shared_ptr<Playable> IGame::Selp(void) const
{
	auto seloSP = this->Selo();
	return (seloSP && seloSP->IsPlayableGObject() == true) ? std::static_pointer_cast<Playable>(seloSP) : std::shared_ptr<Playable>();
}

std::shared_ptr<Building> IGame::Selb(void) const
{
	auto seloSP = this->Selo();
	return (seloSP && seloSP->IsBuilding() == true) ? std::static_pointer_cast<Building>(seloSP) : std::shared_ptr<Building>();
}

std::shared_ptr<Settlement> IGame::Sels(void) const
{
	auto seloSP = this->Selo();
	return (seloSP && seloSP->IsBuilding() == true) ? std::static_pointer_cast<Building>(seloSP)->GetSettlement() : std::shared_ptr<Settlement>();
}

std::shared_ptr<Unit> IGame::Selu(void) const
{
	auto seloSP = this->Selo();
	return (seloSP && seloSP->IsUnit() == true) ? std::static_pointer_cast<Unit>(seloSP) : std::shared_ptr<Unit>();
}

std::shared_ptr<Hero> IGame::Selh(void) const
{
	auto seloSP = this->Selo();
	return (seloSP && seloSP->IsHero() == true) ? std::static_pointer_cast<Hero>(seloSP) : std::shared_ptr<Hero>();
}

std::shared_ptr<ObjsList> IGame::Selos(void) const
{
	const auto players_array = this->adventure->GetScenario()->GetPlayersArrayW();
	assert(players_array.expired() == false);
	const auto currentPlayer = players_array.lock()->GetPlayerRef(players_array.lock()->GetCurrentPlayerID());
	assert(currentPlayer.expired() == false);
	return currentPlayer.lock()->GetSelection()->GetTroops();
}

std::shared_ptr<SelectedObjects> IGame::Selection(void) const
{
	const auto players_array = this->adventure->GetScenario()->GetPlayersArrayW();
	assert(players_array.expired() == false);
	const auto currentPlayer = players_array.lock()->GetPlayerRef(players_array.lock()->GetCurrentPlayerID());
	assert(currentPlayer.expired() == false);
	return currentPlayer.lock()->GetSelection();
}

std::shared_ptr<SelectedObjects> IGame::SelectedUnits(void) const
{
	auto selection = this->Selection();
	return (selection && selection->ContainsOnlyUnits() == true) ? selection : std::shared_ptr<SelectedObjects>();
}
#pragma endregion


std::weak_ptr<Adventure> IGame::GetCurrentAdventureW(void) const noexcept
{
	return this->adventure;
}

std::shared_ptr<IGameUI> IGame::GetUIRef(void) const noexcept
{
	return this->UI;
}

std::shared_ptr<centurion::assets::xml_assets> IGame::GetXmlAssets(void) const noexcept
{
	return this->xmlAssets;
}

std::unordered_map<std::string, std::shared_ptr<gui::Iframe>>& IGame::GetIframes(void)
{
	return this->UI->GetIframesMap();
}

bool IGame::IsCurrentPlayer(const uint8_t playerId) const noexcept
{
	const auto players_array = this->adventure->GetScenario()->GetPlayersArrayW();
	assert(players_array.expired() == false);
	return players_array.lock()->GetCurrentPlayerID() == playerId;
}

void IGame::ExecuteInternalMethods(void)
{
}

void IGame::Run(void)
{
	centurion::game::game_pipeline* pipeline = nullptr;

	if (this->IsEditor())
	{
		pipeline = new centurion::game::editor_pipeline(this);
	}
	else if (this->IsMatch())
	{
		pipeline = new centurion::game::match_pipeline(this);
	}
	if (pipeline != nullptr)
	{
		pipeline->execute();
		delete pipeline;
	}
}
