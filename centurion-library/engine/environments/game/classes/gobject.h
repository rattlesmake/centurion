/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <centurion_typedef.hpp>

#include <environments/game/classes/objectsAttributes/gobjectAttributes.h>
#include <environments/game/classes/objectsStuff/commandsQueue.h>
#include <environments/game/classes/objectsStuff/objectTarget.h>

#include <environments/game/adventure/scenario/area.h>
#include <environments/game/match/match.h>
#include <players/player.h>

class Unit;
class Building;
class Decoration;
class Hero;
class Wagon;
class Grid;
class Terrain;
class MatchUI;

namespace rattlesmake 
{
	namespace image 
	{ 
		class png; 
	}; 
};

namespace centurion
{
	namespace assets
	{
		class xml_entity;
	};
};

class GObject : public GObjectAttributes
{
public:

	enum class Directions
	{
		right = 0,
		top = 90,
		left = 180,
		bottom = 270,

		top_right = 45,
		top_left = 135,
		bottom_left = 225,
		bottom_right = 315
	};

	enum class PlacementErrors : placementError_t
	{
		//Every time a new element is added/updated here, update even GObject::SendInfoText

		e_no_error = 0,
		e_impassable_point,
		e_no_settlement_around,
		e_another_settlement_around,
		e_enemy_settlement_around,
		e_more_possible_choices, 
		e_settlement_chain_broken,
		e_map_border_limit,
	};

	[[nodiscard]] static std::shared_ptr<GObject> Create(const uint8_t _playerID, glm::vec3 _pos, const bool _bTemporary, const classData_t& objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);
	static void ResetNextId(void) noexcept;
	static void SetNextId(const uniqueID_t nextId);
	[[nodiscard]] static uniqueID_t GetNextId(void) noexcept;
	static void SetTabs(const uint8_t tabs);

	#pragma region Constructors and destructor:
	GObject(const GObject& other) = delete;
	virtual ~GObject(void);
	#pragma endregion

	#pragma region Operators and hash function:
	GObject& operator=(const GObject& other) = delete;
	friend std::ostream& operator<<(std::ostream& out, const GObject& object) noexcept;
	[[nodiscard]] bool operator==(const GObject& other) const noexcept;
	struct hash_GObject
	{
		[[nodiscard]] size_t operator()(const GObject& gobject) const noexcept;
	};
	#pragma endregion

	[[nodiscard]] const std::unordered_set<std::string>& GetParentsSetCRef(void) const;

	static void SetPlayersArrayWRef(std::weak_ptr<PlayersArray> _playersWeakRef);

	/// <summary>
	/// This function checks if the current object is selected, namely if the selection includes the current object.
	/// </summary>
	/// <returns>True if the object is selected; false otherwise.</returns>
	[[nodiscard]] virtual bool IsSelected(void) const noexcept;

	/// <summary>
	/// This function marks the current object as selected.
	/// </summary>
	virtual bool Select(const uint8_t _playerID);

	void GlobalDeselect(void);

	/// <summary>
	/// This function marks the current object as deselected.
	/// </summary>
	virtual bool Deselect(const uint8_t _playerID);

	/// <summary>
	/// This function retrieves the unique picking ID of the current object.
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] pickingID_t GetPickingID(void) const noexcept;

	/// <summary>
	/// This function retrieves the unique ID of the current object.
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] uniqueID_t GetUniqueID(void) const noexcept;

	/// <summary>
	/// This function sets the picking ID of the current object.
	/// </summary>
	/// <param name="par_pickingID">The new picking ID.</param>
	void SetPickingID(const pickingID_t par_pickingID);

	/// <summary>
	/// This function returns true if the current object is heir of the specified class
	/// </summary>
	/// <param name="className">The class name</param>
	[[nodiscard]] bool IsHeirOf(const std::string& className) const;

	/// <summary>
	/// This function returns the class name of the current object.
	/// </summary>
	/// <returns>The class name of the current object.</returns>
	[[nodiscard]] std::string GetClassName(void) const;

	[[nodiscard]] const std::string& GetClassNameCRef(void) const;

	/// <summary>
	/// This function sets the class name of the current object.
	/// </summary>
	/// <param name="par_className">The class name.</param>
	void SetClassName(std::string par_className);

	/// <summary>
	/// This function return the type of the current object.
	/// </summary>
	/// <returns>The type of the current object.</returns>
	[[nodiscard]] std::string GetType(void) const noexcept;

	[[nodiscard]] assets::xmlClassTypeInt_t GetTypeInt(void) const noexcept;

	/// <summary>
	/// This function check if the current GObject is a Playable GObject.
	/// </summary>
	/// <returns>True or not.</returns>
	[[nodiscard]] bool IsPlayableGObject(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the building classes family or not.
	/// </summary>
	/// <returns>True or not.</returns>
	[[nodiscard]] bool IsBuilding(void) const noexcept;

	/// <summary>
	/// This function check if the current object is a simple unit, i.e. it must not be an hero, a druid, etc
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsSimpleUnit(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the unit classes family or not.
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsUnit(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the hero classes family or not.
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsHero(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the wagon classes family or not.
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsWagon(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the druid classes family or not.
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsDruid(void) const noexcept;

	/// <summary>
	/// This function check if the current object belong to the building classes family or not.
	/// </summary>
	/// <returns>True or false.</returns>
	[[nodiscard]] bool IsDecoration(void) const noexcept;

	/// <summary>
	/// This function sets the position of the current object.
	/// </summary>
	/// <param name="pos">The position (x and y)</param>
	virtual bool SetPosition(const int x, const int y);

	/// <summary>
	/// This function returns the position of the current object.
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] std::shared_ptr<Point> GetPosition(void) const;

	//[[nodiscard]] glm::ivec2 GetEntityPosition(void) const;

	/// <summary>
	/// This function returns only the x position of the current object.
	/// </summary>
	/// <returns>The x position.</returns>
	[[nodiscard]] uint32_t GetPositionX(void) const;

	/// <summary>
	/// This function return the y position of the current object.
	/// </summary>
	/// <returns>The y position.</returns>
	[[nodiscard]] uint32_t GetPositionY(void) const;

	[[nodiscard]] uint32_t GetPositionY2D(void) const;

	//Grid methods
	void UpdateGrid(const bool bUpdateEvenGrid, const std::shared_ptr<Surface>& surface);
	void ClearGrid(const bool bUpdateEvenGrid, const std::shared_ptr<Surface>& surface);

	//Placeability
	[[nodiscard]] bool IsPlaceable(void) const noexcept;
	virtual void CheckPlaceability(const bool bReturnAnAffermativeAnswer = false) = 0;

	// saved
	void MarkAsSaved(void);
	void MarkAsNotSaved(void);
	[[nodiscard]] bool IsSaved(void) const noexcept;

	void GetBinSignature(std::vector<uint8_t>& data) const;
	virtual void GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const;

	// entity
	void ChangeAnim(const std::string& anim);
	void SetCurrentFrame(const uint32_t frame);
	void SetCurrentDirection(const uint8_t dir);
	void SetCurrentAnim(const std::string& name);
	void SetCurrentState(const std::string& name);
	[[nodiscard]] const std::string& GetCurrentAnim(void) const noexcept;
	[[nodiscard]] const std::string& GetCurrentState(void) const noexcept;
	[[nodiscard]] uint32_t GetCurrentFrame(void) const noexcept;
	[[nodiscard]] uint8_t GetCurrentDir(void) const noexcept;

	/// <summary>
	/// This abstract performs the current object's game logic checks. It must be override.
	/// </summary>
	virtual void ApplyGameLogics(void) = 0;

	/// <summary>
	/// This function calculates the distance beetwen the current object and an other object.
	/// </summary>
	/// <param name="other">The second object</param>
	/// <returns>The distance between the two objects ot -1.0 if other is nullptr.</returns>
	[[nodiscard]] double DistTo(const std::shared_ptr<GObject>& other) const;

	void RenderOrigin(void);
	void RenderHitbox(void);

	RectangularArea& GetHitBox(void);

	void SetAsTemporary();
	void SetAsTemporaryMultiple(void);

	//[[nodiscard]] std::shared_ptr<Entity> GetEntity(void) const;
	[[nodiscard]] std::shared_ptr<assets::xml_entity> GetXmlEntity(void) const noexcept;
	[[nodiscard]] double GetClickedTime(void) const noexcept;
	[[nodiscard]] bool IsHovering(void) const noexcept;
	[[nodiscard]] bool IsInCameraViewport(void) const noexcept;
protected:
	GObject(glm::vec3&& _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource);

	virtual std::ostream& Serialize(std::ostream& out, const bool calledByChild) const;
	virtual void SetAttrs(const classData_t& objData, gobjData_t* dataSource);

	/// <summary>
	/// This function sends to editor a message that will shown in the top left of the screen.
	/// </summary>
	void SendInfoText(void) const;

	bool IsTemporary(void) const noexcept;
	bool bIsTemporary = false;
	bool bisTemporaryMultiple = false;

	std::weak_ptr<GObject> me;

	// entity methods and variables
	std::shared_ptr<assets::xml_entity> xml_entity;

	uint32_t currentFrame = 0;
	uint8_t currentDirection = 0;
	std::string currentAnim = "idle";
	std::string currentState = "default";
	uint32_t framesAnimVariation = 0;

	// Grid
	bool bIsPlaceable = true;
	std::shared_ptr<Grid> grid;

	double clickedTime = 0.f;

	// hitbox	
	RectangularArea HitBox;

	/// <summary>
	/// This function sets an error to show during the moving of an object.
	/// </summary>
	/// <param name="_placementError">The error code.</param>
	void SetPlacementError(const PlacementErrors _placementError);
	PlacementErrors placementError = PlacementErrors::e_no_error; //No error during creation. 

	std::string className;
	bool bIsSaved = false;
	classData_t xmlData;

	std::shared_ptr<rattlesmake::image::png> origin;
	glm::ivec2 visibleMapSize{ 0,0 };
	
	// shortcuts
	std::weak_ptr<Terrain> terrain;
	std::weak_ptr<MatchUI> matchUI;

	//A set containg the IDs of the players the selected this GObject.
	std::unordered_set<uint8_t> playersThatSelectedMe;

	static uint8_t nTabs;
	static std::weak_ptr<PlayersArray> playersWeakRef;
private:
	/// ************************************************************************************************
	/// * An attribute must be private instead of protected when it's set method has some side effect. *
	/// ************************************************************************************************

	std::shared_ptr<Point> position;

	pickingID_t pickingID = 0;
	uniqueID_t uniqueID = 0;
	static uniqueID_t NextId;
};
