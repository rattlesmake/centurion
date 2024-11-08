/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/
#pragma once

#include <header.h>
#include <engine.h>
#include <environments/game/igame.h>
#include "editorUi.h"

class GObject;
class ObjsList;

namespace centurion
{
	namespace game
	{
		class editor_pipeline;
	}
};

enum class editor_state
{
	idle = 0,

	inserting_object = 1,
	shifting_object = 2,
	changing_terrain = 3,
	drawing_areas = 4,
	using_area_gizmo = 5
};

class Editor : public IGame
{
	friend class centurion::game::editor_pipeline;

public:
	explicit Editor(const Editor& other) = delete;
	Editor& operator = (const Editor& other) = delete;
	virtual ~Editor(void);

	[[nodiscard]] static std::shared_ptr<Editor> CreateEditor(const IEnvironment::Environments currentEnv);

	void ExecuteInternalMethods(void) override;

	static void AddEditorObjectTreeElement(const std::string& path, const std::string& className);
	static void AddEditorTerrainTreeElement(const std::string& filter1, const std::string& filter2);

	#pragma region PYTHON API
	void Quit(void) override;
	void NewAdventure(std::string& advName);
	void OpenAdventure(std::string& advName);
	void SaveAdventureAs(std::string& advName);
	void SaveAdventureCopy(std::string& advName);
	void DeleteAdventure(std::string& advName);
	void RunAdventure(void);
	void InsertObject(std::string className, const uint32_t playerID);
	void ChangeTerrainType(const int size, std::string type, std::string textureName, const float heightValue);
	[[nodiscard]] std::vector<std::string> GetEditorTerrainTreeList1(void);
	[[nodiscard]] std::vector<std::string> GetEditorTerrainTreeList2(const std::string filter1);
	[[nodiscard]] bool CheckOpenObjectPropertiesCondition(void) const;
	void OpenObjectProperties(void) const;
	#pragma endregion

	void SetFocusedObject(const std::shared_ptr<GObject>& focusedObject);

	// state booleans 
	[[nodiscard]] bool IsShiftingObject(void) const;
	[[nodiscard]] bool IsChangingTerrain(void) const;
	[[nodiscard]] bool IsInsertingObject(void) const;
	[[nodiscard]] bool IsIdle(void) const;

	editor_state GetState(void);
	void SetState_Idle(void);
	void SetState_InsertingObject(void);
	void SetState_ShiftingObject(void);
	void SetState_ChangingTerrain(void);

private:
	explicit Editor(const IEnvironment::Environments currentEnv);

	#pragma region Internal methods and data
	static void NewAdventure_Internal(std::string advName, const bool alreadyExists);
	struct NewAdventure_Internal_Data
	{
		std::string advName;
		bool bAlreadyExists = false;
		bool bActive = false;
	};
	static NewAdventure_Internal_Data sNewAdventure_Internal;

	static void OpenAdventure_Internal(std::string advName);
	struct OpenAdventure_Internal_Data
	{
		std::string advName;
		bool bActive = false;
	};
	static OpenAdventure_Internal_Data sOpenAdventure_Internal;
	#pragma endregion

	static void SaveAdventure_Internal(void);
	static void SaveAdventureAs_Internal(std::string advName, const bool alreadyExists);
	static void SaveAdventureCopy_Internal(std::string advName, const bool alreadyExists);
	static void DeleteAdventure_Internal(std::string advName, const bool createEmpty);
	static void RunAdventure_Internal(void);
	
	#pragma region Moving/inserting Object:

	struct MovingObject
	{
		std::weak_ptr<GObject> focusedObject;
		std::weak_ptr<ObjsList> objectsToShift;
		uint32_t StartXMouse = 0;
		uint32_t StartYMouse = 0;
		uint32_t startObjectXPos = 0;
		uint32_t startObjectYPos = 0;
	} movingObject;

	struct InsertingObject
	{
		std::list<std::pair<glm::vec2, bool>> objectOffsets;
		glm::vec2 startPos{ 0,0 };
		std::string className;
		std::optional<std::shared_ptr<GObject>> objectToInsert;
		uint8_t playerId = 0;
	} insertingObject;

	#pragma endregion

	std::shared_ptr<TerrainBrush> GetTerrainBrushPtr(void) const;

	static std::vector<std::array<std::string, 2>> editorTerrainTree;
	static std::vector<std::pair<std::string, std::string>> editorTreeSource;

	editor_state current_state = editor_state::idle;

	static Editor* currentEditor;
};
