#include "players_array.h"
#include "player.h"
#include "colors_array.h"
#include <environments/game/tlv_types.h>

#include <stl_utils.h>
#include <engine.h>
#include <settings.h>
#include <environments/game/editor/editor.h>
#include <environments/game/classes/gobject.h>
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/VFX/vfx.h>
#include <environments/game/classes/objectsStuff/objectPoint.h>
#include <dialogWindows.h>
#include <tinyxml2_utils.h>

#include <math_utils.h>
#include <bin_data_interpreter.h>

#include <mouse.h>
#include <keyboard.h>

// Assets
#include <xml_class_enums.h>
#include <xml_entity_shader.h>

#include <GLFW/glfw3.h>


#pragma region Static attributes initialization:
uint8_t PlayersArray::nTabs = 0;
#pragma endregion


#pragma region Constructors:
PlayersArray::PlayersArray(classesData_t _classesDataSP, const std::shared_ptr<ColorsArray>& colorsArray) :
	classesDataSP(std::move(_classesDataSP))
{
	const uint8_t playersArraySize = this->GetArraySize();

	// Set defult colors for each player.
	for (uint8_t i = 0; i < playersArraySize; i++)
	{
		this->players[i] = std::shared_ptr<Player>(new Player(i, colorsArray->GetDefaultColor(i), (*this)));
	}

	// Set a pointer to the classes data
	CommandsQueue::SetClassesDataRef(this->GetClassesDataSp());
}

PlayersArray::PlayersArray(classesData_t _classesDataSP, const uint8_t nPlayers, const std::list<pair<uint8_t, glm::vec3>>& colorsArray, const std::list<std::string>& racesArray) :
	classesDataSP(std::move(_classesDataSP))
{
	this->SetEffectiveNumberOfPlayers(nPlayers);
	const uint8_t playersArraySize = this->GetArraySize();

	auto colors_iterator = colorsArray.begin();
	auto races_iterator = racesArray.begin();
	uint8_t iterator = 0;

	assert(colorsArray.size() == playersArraySize && racesArray.size() == playersArraySize);

	while (iterator < playersArraySize)
	{
		Color pcolor{ colors_iterator->second };
		std::string prace{ races_iterator->c_str() };
		this->players[iterator] = std::shared_ptr<Player>(new Player(iterator, pcolor, (*this)));
		this->players[iterator]->SetRace(prace);

		colors_iterator++;
		races_iterator++;
		iterator++;
	}

	// Set a pointer to the classes data
	CommandsQueue::SetClassesDataRef(this->GetClassesDataSp());
}

PlayersArray::~PlayersArray(void)
{
	#if CENTURION_DEBUG_MODE
	for (auto const& p : this->players)
		assert(p.use_count() == 1);
	#endif // CENTURION_DEBUG_MODE
}
#pragma endregion


#pragma region Operators:
std::ostream& operator<<(std::ostream& out, const PlayersArray& p) noexcept
{
	const string nTabs(PlayersArray::nTabs, '\t');
	out << nTabs << "<players>";
	Player::SetTabs(PlayersArray::nTabs + 1);
	for (uint8_t playerCounter = 0; playerCounter <= PlayersArray::NUMBER_OF_PLAYERS; playerCounter += 1)
	{
		out << '\n' << (*p.players[playerCounter]);
	}
	out << '\n' << nTabs << "</players>";
	return out;
}

std::shared_ptr<Player> PlayersArray::operator[](const uint32_t index) const noexcept
{
	return (index <= this->players.size()) ? this->players[index] : std::shared_ptr<Player>();
}
#pragma endregion


#pragma region Static members:
void PlayersArray::SetTabs(const uint8_t tabs)
{
	PlayersArray::nTabs = tabs;
}

uint8_t PlayersArray::GetArraySize(void)
{
	return NUMBER_OF_PLAYERS + 1;
}

uint8_t PlayersArray::GetMaxPlayersNumberAllowed(void)
{
	return PlayersArray::NUMBER_OF_PLAYERS;
}
#pragma endregion


#pragma region To Python:
uint8_t PlayersArray::GetAllowedPlayersNumber(void)
{
	return PlayersArray::NUMBER_OF_PLAYERS;
}
#pragma endregion


SettlementsCollection& PlayersArray::GetSettlementsCollectionRef(void)
{
	return this->settlementCollection;
}

#pragma region Serialization and deserialization
///You have to consider also operator<< that's used when saving into an XML file.

void PlayersArray::SaveAsBinaryFile(BinaryFileWriter& bfw)
{
	this->additionalPlayableGObjectsInfo = std::unique_ptr<AdditionalPlayableGObjectsInfo>{ new AdditionalPlayableGObjectsInfo() };
	try
	{
		// Saving GObjects 
		for (auto const& player_it : this->players)
		{
			// For each player, save firstly the number of GObjects belonging to it and then save each GObjects belonging to it.
			auto& playerOC = player_it->GetObjsCollection();
			const uint32_t playerObjectsNumber = playerOC->GetNumberOfObjects();
			bfw.PushNumber(playerObjectsNumber);
			playerOC->SaveAsBinaryFile(bfw);
		}

		{
			/// N.B.: In order to proper restore garrisons during loading, is important save heroes' army before buildings' garrison.
			/// Obviously, PlayersArray::DeserializeFromBinFile must follow the same order during loading.

			// Saving not empty heroes' army:
			bfw.PushNumber(this->additionalPlayableGObjectsInfo->heroesHavingNotEmptyArmy.size());
			for (auto const& heroId_it : this->additionalPlayableGObjectsInfo->heroesHavingNotEmptyArmy)
			{
				std::vector<uint8_t> heroData;
				std::shared_ptr<GObject> hero = this->objsByUniqueId.at(heroId_it).lock();
				assert(hero && hero->IsHero() == true);
				std::static_pointer_cast<Hero>(hero)->GetArmy()->GetBinRepresentation(heroData);
				bfw.PushBinaryData(std::move(heroData));
			}

			// Saving not empty buildings' garrison:
			bfw.PushNumber(this->additionalPlayableGObjectsInfo->buildingsHavingNotEmptyGarrison.size());
			for (auto const& garrisonId_it : this->additionalPlayableGObjectsInfo->buildingsHavingNotEmptyGarrison)
			{
				std::vector<uint8_t> garrisonData;
				std::shared_ptr<GObject> garrison = this->objsByUniqueId.at(garrisonId_it).lock();
				assert(garrison && garrison->IsBuilding() == true);
				std::static_pointer_cast<Building>(garrison)->GetGarrison()->GetBinRepresentation(garrisonData);
				bfw.PushBinaryData(std::move(garrisonData));
			}
		}

		// Saving selections
		for (auto const& player_it : this->players)
		{
			std::vector<uint8_t> selectionData;
			player_it->GetSelection()->GetBinRepresentation(selectionData);
			bfw.PushBinaryData(std::move(selectionData));
		}

		// Saving units' target and their status/behavior
		bfw.PushNumber(this->additionalPlayableGObjectsInfo->unitsHavingAnActiveStatus.size());
		for (auto const& unitId_it : this->additionalPlayableGObjectsInfo->unitsHavingAnActiveStatus)
		{
			std::vector<uint8_t> unitData;
			std::shared_ptr<GObject> unit = this->objsByUniqueId.at(unitId_it).lock();
			assert(unit && unit->IsUnit() == true);
			std::static_pointer_cast<Unit>(unit)->GetTargetAndBehaviorBinRepresentation(unitData);
			bfw.PushBinaryData(std::move(unitData));
		}
	}
	catch (const std::exception& ex)
	{
		// Free additionalGObjectsInfo and re-throw exception
		this->additionalPlayableGObjectsInfo.reset();
		throw ex;
	}
}

void PlayersArray::DeserializeFromBinFile(BinaryFileReader& bfr, const std::shared_ptr<Surface>& surface)
{
	this->additionalPlayableGObjectsInfo = std::unique_ptr<AdditionalPlayableGObjectsInfo>{ new AdditionalPlayableGObjectsInfo() };

	try
	{
		// Loading GObjects
		for (uint8_t playerID = 0; playerID <= PlayersArray::NUMBER_OF_PLAYERS; playerID += 1)
		{
			auto const& player = this->players[playerID];
			// For each player, read firstly the number of GObjects belonging to it and then load each GObjects belonging to it.
			bfr.ReadNumber();
			const uint32_t playerObjectsNumber = static_cast<uint32_t>(bfr.GetNumber());
			bfr.ReadBinaryData(playerObjectsNumber);
			for (uint32_t n = 0; n < playerObjectsNumber; ++n)
			{
				std::vector<byte_t> objData = bfr.GetBinData();
				ObjsCollection::CreateGObjectFromBinData(this->classesDataSP, std::move(objData), playerID, surface);
			}
		}

		// Loading heroes' army and then buildings' garrison 
		for (uint8_t i = 0; i <= 1; i++) //i == 0 --> heroes' army; i == 1 --> buildings' garrison
		{
			const std::string msg = (i == 0) ? "n army" : " garrison";
			bfr.ReadNumber();
			const uint32_t toRead = static_cast<uint32_t>(bfr.GetNumber());
			bfr.ReadBinaryData(toRead);
			for (uint32_t n = 0; n < toRead; ++n)
			{
				std::vector<byte_t> data = bfr.GetBinData();
				uint32_t dataOffset = 0;
				const uniqueID_t ID = BinaryDataInterpreter::ExtractUInt32(data, dataOffset); //Get hero ID.
				if (this->objsByUniqueId.contains(ID) == false)
					throw BinaryDeserializerException("Invalid owner ID while loading a" + msg);
				std::shared_ptr<GObject> gobj = this->objsByUniqueId.at(ID).lock();
				if (i == 0 && gobj->IsHero() == true && this->additionalPlayableGObjectsInfo->heroesHavingNotEmptyArmy.contains(ID) == true)
					std::static_pointer_cast<Hero>(gobj)->GetArmy()->InitByBinData(std::move(data), dataOffset, this->objsByUniqueId);
				else if (i == 1 && gobj->IsBuilding() == true && this->additionalPlayableGObjectsInfo->buildingsHavingNotEmptyGarrison.contains(ID) == true)
					std::static_pointer_cast<Building>(gobj)->GetGarrison()->InitByBinData(std::move(data), dataOffset, this->objsByUniqueId);
				else
					throw BinaryDeserializerException("Invalid owner ID while loading a" + msg);
			}
		}

		// Loading selections
		{
			bfr.ReadBinaryData(static_cast<uint32_t>(this->players.size()));
			for (auto const& player_it : this->players)
			{
				std::vector<byte_t> data = bfr.GetBinData();
				player_it->GetSelection()->InitByBinData(std::move(data), 0, this->objsByUniqueId);
			}
		}

		// Loading units' target and their status/behavior
		{
			bfr.ReadNumber();
			const uint32_t toRead = static_cast<uint32_t>(bfr.GetNumber());
			if (this->additionalPlayableGObjectsInfo->unitsHavingAnActiveStatus.size() != toRead)
				throw BinaryDeserializerException("Invalid info about units' target and their status/behavior");
			bfr.ReadBinaryData(toRead);
			for (auto const& unitId_it : this->additionalPlayableGObjectsInfo->unitsHavingAnActiveStatus)
			{
				std::vector<byte_t> data = bfr.GetBinData();
				std::shared_ptr<GObject> unit;
				if (this->objsByUniqueId.contains(unitId_it) == false || (unit = this->objsByUniqueId.at(unitId_it).lock())->IsUnit() == false)
					throw BinaryDeserializerException("Invalid unit ID (" + std::to_string(unitId_it) + ") while loading its target");
				std::static_pointer_cast<Unit>(unit)->SetTargetAndBehaviorByBinData(std::move(data), this->objsByUniqueId);
			}
		}
	}
	catch (const std::exception& ex)
	{
		// Free additionalGObjectsInfo and re-throw exception
		this->additionalPlayableGObjectsInfo.reset();
		throw ex;
	}
}

void PlayersArray::DeserializeFromXML(tinyxml2::XMLElement& playersTag, const std::shared_ptr<Surface>& surface)
{
	std::unordered_set<uint8_t> playersDeserialized;
	for (tinyxml2::XMLElement* playerTag = playersTag.FirstChildElement(); playerTag != nullptr; playerTag = playerTag->NextSiblingElement())
	{
		const int playerID = tinyxml2::TryParseIntAttribute(playerTag, "id");

		// Assertion: check ID
		assert(playerID >= 0 && playerID <= PlayersArray::NUMBER_OF_PLAYERS);
		// Assertion: An ID cannot be repeated:
		assert(playersDeserialized.contains(playerID) == false);

		playersDeserialized.insert(playerID);
		this->players[playerID]->DeserializeFromXML(this->classesDataSP, (*playerTag), surface);
	}
}

void PlayersArray::AddBuildingHavingNotEmptyGarrison(const Building& b)
{
	// ASSERTION: This function MUST be called only during a binary save/loading
	assert(this->additionalPlayableGObjectsInfo);
	this->additionalPlayableGObjectsInfo->buildingsHavingNotEmptyGarrison.insert(b.GetUniqueID());
}

void PlayersArray::AddHeroHavingNotEmptyArmy(const Hero& h)
{
	// ASSERTION: This function MUST be called only during a binary save/loading
	assert(this->additionalPlayableGObjectsInfo);
	this->additionalPlayableGObjectsInfo->heroesHavingNotEmptyArmy.insert(h.GetUniqueID());
}

void PlayersArray::AddUnitHavingAnActiveBehavior(const Unit& u)
{
	// ASSERTION: This function MUST be called only during a binary save/loading
	assert(this->additionalPlayableGObjectsInfo);
	this->additionalPlayableGObjectsInfo->unitsHavingAnActiveStatus.insert(u.GetUniqueID());
}
#pragma endregion

void PlayersArray::OpenObjPropsIframe(const GObject& focusedObj) const
{
	auto env = Engine::GetInstance().GetEnvironment();
	if (focusedObj.IsSelected() == true && env->GetType() == IEnvironment::Environments::e_editor)
	{
		const Editor& editor_ref = static_cast<const Editor&>(*env);
		if (editor_ref.IsShiftingObject() == false)  // Do NOT open any iframe while shifting a GObject (avoid opening when shifiting ends if right button was pressed)
		{
			if (editor_ref.GetNumberOfSelectedObjects() == 1)
			{
				const assets::xmlClassTypeInt_t objType = focusedObj.GetTypeInt();
				std::string iframeToOpen;

				switch (objType)
				{
				case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_unitClass):
					iframeToOpen = "__unitProps__";
					break;
				case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_heroClass):
					iframeToOpen = "__heroProps__";
					break;
				case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_druidClass):
					iframeToOpen = "__druidProps__";
					break;
				case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_wagonClass):
					iframeToOpen = "__wagonProps__";
					break;
				case static_cast<assets::xmlClassTypeInt_t>(assets::xml_class_type::e_buildingClass):
					iframeToOpen = "__buildingProps__";
					break;
				}
				editor_ref.GetUIRef()->OpenIframe(iframeToOpen);
			}
			else
			{
				std::string iframeToOpen{ "__multiProps__" };
				editor_ref.GetUIRef()->OpenIframe(iframeToOpen);
			}

			// It avoids that, together with the opening of the properties iframe, the GObject (if is an unit) changes orientation (due to the invocation of the method Unit::ManageRightClick)
			rattlesmake::peripherals::mouse::get_instance().RightClick = false;
		}
	}
}

void PlayersArray::Picking(void)
{
	uint32_t counter = 0;
	for (auto const& player : this->players)
	{
		player->GetObjsCollection()->RenderAndPicking(this->objsByPickingId, counter);
	}
	/// Manage picking result.
	auto colorOpt = rattlesmake::peripherals::mouse::get_instance().GetColorOfClickedPixel();
	// ASSERTION: I must have a color, since PlayersArray::HasPickingToDo() retrieved true
	assert(colorOpt.has_value() == true);
	const uint32_t clickingID = Math::ConvertColorToId(colorOpt.value());
	this->HandleClickForCurrentPlayer(clickingID);
}

void PlayersArray::Play(const bool bOnlyLogic, const bool bMinimapIsActive)
{
	// VFXs
	if (bMinimapIsActive == false)
	{
		auto curEnv = Engine::GetInstance().GetEnvironment();
		if (curEnv->GetType() == IEnvironment::Environments::e_match)
		{
			curEnv->AsMatch()->GetVFXCollection().RenderVFXs();
		}
	}

	// GObjects
	for (auto const& player : this->players)
	{
		player->GetSelection()->ResetAttributes();
		player->GetObjsCollection()->RenderAndLogic(bOnlyLogic, bMinimapIsActive);
	}
}

uint8_t PlayersArray::GetCurrentPlayerID(void) const
{
	return this->currentPlayer;
}

std::array<std::shared_ptr<Player>, PlayersArray::NUMBER_OF_PLAYERS + 1>& PlayersArray::GetPlayersArrayRef(void)
{
	return this->players;
}

void PlayersArray::ApplyPlayersSettings(PlayersSettings& playersSettings)
{
	uint8_t nPlayers = 0;
	for (uint8_t i = 0; i < 8; i++)
	{
		if (playersSettings[i].Active)
		{
			this->players[nPlayers + static_cast<uint64_t>(1)]->SetRace(playersSettings[i].Race);
			this->players[nPlayers + static_cast<uint64_t>(1)]->SetName(playersSettings[i].Name);
			nPlayers++;
		}
	}
	this->SetEffectiveNumberOfPlayers(nPlayers);
}

void PlayersArray::SetEffectiveNumberOfPlayers(const uint8_t numberOfPlayers)
{
	this->effectiveNumberOfPlayers = numberOfPlayers;
}

uint8_t PlayersArray::GetEffectiveNumberOfPlayers(void) const
{
	return this->effectiveNumberOfPlayers;
}

std::weak_ptr<Player> PlayersArray::GetPlayerRef(const uint8_t id)
{
	return this->players[id];
}

std::list<pair<uint8_t,glm::vec3>> PlayersArray::GetPlayersColorsList(void) const
{
	std::list<pair<uint8_t, glm::vec3>> colors;
	for (uint8_t playerCounter = 0; playerCounter < PlayersArray::NUMBER_OF_PLAYERS + 1; playerCounter += 1)
		colors.push_back({ playerCounter, this->players[playerCounter]->GetColor().ToGlmVec3() });
	return colors;
}

std::list<std::string> PlayersArray::GetPlayersRacesList(void) const
{
	std::list<std::string> races;
	for (uint8_t playerCounter = 0; playerCounter < PlayersArray::NUMBER_OF_PLAYERS + 1; playerCounter += 1)
		races.push_back(this->players[playerCounter]->GetRace());

	return races;
}

void PlayersArray::AddGObject(std::shared_ptr<GObject> gobj)
{
	const uniqueID_t gobjID = gobj->GetUniqueID();

	if (gobjID == 0 || this->objsByUniqueId.contains(gobjID) == true)
	{
		#if CENTURION_DEBUG_MODE
		std::cout << "[DEBUG] Duplicatd GObject ID " << std::to_string(gobjID) << std::endl;
		#endif
		throw std::runtime_error("Duplicated GObject ID " + std::to_string(gobjID));  //TODO - creare eccezione apposita
	}

	// Add ID
	this->objsByUniqueId.insert({ gobj->GetUniqueID(), std::move(gobj) });

	// Update next ID
	if (gobjID >= GObject::GetNextId())
		GObject::SetNextId(gobjID + 1);
}

bool PlayersArray::UpdateGObjectScriptId(const std::shared_ptr<Playable>& gobj, scriptIdName_t newScriptId)
{
	// Script ID name must be unique, so don't insert it again if it's already present.
	if (this->objsByIdName.contains(newScriptId) == true)
		return false;

	// If GObject had already a script ID, make the latter available again 
	this->objsByIdName.erase(gobj->GetIDName());

	// If newScriptId is a NOT empty string, then add it associating it to the GObject
	if (newScriptId.empty() == false)
		this->objsByIdName.insert({ std::move(newScriptId), gobj->GetUniqueID() });

	return true;
}

void PlayersArray::RemoveGObject(const std::shared_ptr<GObject>& gobj)
{
	assert(this->objsByUniqueId.contains(gobj->GetUniqueID()) == true);
	this->objsByUniqueId.erase(gobj->GetUniqueID());
	if (gobj->IsPlayableGObject() == true)
		this->objsByIdName.erase(std::static_pointer_cast<Playable>(gobj)->GetIDName());
}

classesData_t PlayersArray::GetClassesDataSp(void) const noexcept
{
	assert(this->classesDataSP);
	return this->classesDataSP;
}


#pragma region Private members:
void PlayersArray::HandleClickForCurrentPlayer(const pickingID_t clickingID) const
{
	bool bClickedOnTerrain = (this->objsByPickingId.contains(clickingID) == false);

	// Get the clicked GObject. Empty if the click was on terrain.
	auto focusedObj = (bClickedOnTerrain == false) ? this->objsByUniqueId.at(this->objsByPickingId.at(clickingID)).lock() : std::shared_ptr<GObject>();


	if (bClickedOnTerrain == false && focusedObj->IsDecoration() == true && Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match)
	{
		// A click on a decoration is considered as a click on the terrain
		bClickedOnTerrain = true;
	}

	// Manage selection or provide a captured target.
	Player& currentPlayer = (*this->players.at(this->currentPlayer));
	if (currentPlayer.GetObjsCollection()->IsAnyoneWaitingForATarget() == true || rattlesmake::peripherals::mouse::get_instance().RightClick == true) //Waiting for a target provided by the player...
	{
		// ... provide target.
		std::shared_ptr<Target> capturedTarget = Target::CreateTarget();
		if (bClickedOnTerrain == true)
			capturedTarget->SetPoint(Point::GetClickedPoint());
		else
			capturedTarget->SetObject(std::static_pointer_cast<Playable>(focusedObj));
		// Provide a captured target to the current player.
		currentPlayer.GetObjsCollection()->SetCapturedTarget(capturedTarget, rattlesmake::peripherals::mouse::get_instance().RightClick);

		// Try to open obj props iframe
		if (rattlesmake::peripherals::mouse::get_instance().RightClick == true && bClickedOnTerrain == false)
			this->OpenObjPropsIframe((*focusedObj));
	}
	else  // Selection
	{
		this->HandleSelectionForCurrentPlayer(currentPlayer, bClickedOnTerrain, focusedObj);
	}
}

void PlayersArray::HandleSelectionForCurrentPlayer(Player& currentPlayer, const bool bClickedOnTerrain, std::shared_ptr<GObject> focusedObj) const
{
	// Get selection of the current player.
	std::shared_ptr<SelectedObjects> curSelection = currentPlayer.GetSelection();
	if (bClickedOnTerrain == false)
	{
		if (rattlesmake::peripherals::mouse::get_instance().RightClick == false) //Right click doesn't perform selection.
		{
			// Check if shift is pressed
			const bool bShiftPressed = (rattlesmake::peripherals::keyboard::get_instance().IsKeyNotReleased(GLFW_KEY_LEFT_SHIFT) == true);

			// Check if the focused GObject is selected or not
			const bool bIsSelected = focusedObj->IsSelected();

			if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor)
			{
				std::static_pointer_cast<Editor>(Engine::GetInstance().GetEnvironment())->SetFocusedObject(focusedObj);
				if (bIsSelected == true)
					return;  // In editor, a click on a GObject already selected does NOT influence selection. This in order to do not reset multiselection.
			}

			// Choosing a proper function
			std::shared_ptr<Playable> focusedPlayableGObject = std::dynamic_pointer_cast<Playable>(focusedObj);  // This will returns nullptr if the fouced GObject is a not Playable GObject (i.e. if it's a decoration)
			bool (GObject:: * ptr2GObjectMember)(const uint8_t) = (bShiftPressed == true && bIsSelected == true) ? &GObject::Deselect : &GObject::Select;
			if (bShiftPressed == false  // || focusedObj->IsBuilding() == true || curSelection->HasBuilding() == true (N.B.: these two condition are commented because SelectedObjects::Add already handle them)
				|| ( ( (focusedPlayableGObject == nullptr || focusedPlayableGObject->GetPlayer() != this->currentPlayer) || curSelection->ContainsOnlyItsOwnGObjects() == false) && Engine::GetInstance().GetEnvironmentId() != IEnvironment::Environments::e_editor) )
			{
				// When the current player clicked on a GObject, there are some conditions for reset selection:
				// 1) the click was done without pressing the SHIFT key too;
				// 2) the clicked GObject is a building (condition handled directly by SelectedObjects::Add);
				// 3) the selection contains already other buildings (condition handled directly by SelectedObjects::Add);
				// 4) the clicked GObject is belongs to a player other than the current one and the environment is not the editor (if it is NOT a PlayableGObject, this condition is always true);
				// 5) the selection doesn't contains only GObjects belonging to the current player and the environment is not the editor;
				curSelection->Reset();
			}
			// Select/deselect object.
			(*focusedObj.*ptr2GObjectMember)(this->currentPlayer);
		}
	}
	else
	{
		curSelection->Reset();
	}
}
#pragma endregion

