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
#include <binary_file_writer.h>

#include <xml_class_enums.h>

class GObject;
class Playable;
class Building;
class Unit;
class ObjsList;
class EntityShader;
class PlayersArray;
class Target;
class Surface;

class ObjsCollection
{
public:
	#pragma region Constructors and destructor:
	explicit ObjsCollection(const uint8_t _playerdID);
	ObjsCollection(const ObjsCollection& other) = delete;
	~ObjsCollection(void);
	#pragma endregion


	#pragma region Operators:
	ObjsCollection& operator=(const ObjsCollection& other) = delete;
	friend std::ostream& operator<<(std::ostream& out, const ObjsCollection& objCollection) noexcept;
	#pragma endregion


	#pragma region Static members
	static std::shared_ptr<GObject> CreateGObject(const classesData_t& classesDataSP, const std::string& className, std::list<std::pair<glm::vec2, bool>>& points, uint8_t playerID, const bool bTemporary, const std::shared_ptr<Surface>& surface);
	static std::shared_ptr<GObject> CreateGObjectFromXml(const classesData_t& classesDataSP, tinyxml2::XMLElement* xml, const uint8_t playerID, const std::shared_ptr<Surface>& surface);
	static std::shared_ptr<GObject> CreateGObjectFromBinData(const classesData_t& classesDataSP, std::vector<byte_t>&& data, const uint8_t playerID, const std::shared_ptr<Surface>& surface);
	static void DestroyGObject(const std::shared_ptr<GObject>& objSP);
	static void SetPlayersArrayWeakRef(const std::weak_ptr<PlayersArray>& weakRef);
	static void RenderTemporaryObject(std::shared_ptr<GObject> obj, bool applyTerrainHeight, std::list<std::pair<glm::vec2,bool>>& offsets);
	static void SetTabs(const uint8_t tabs);
	#pragma endregion


	void RenderAndPicking(std::unordered_map<pickingID_t, uniqueID_t>& objsByPickingId, uint32_t& counter);
	void RenderAndLogic(const bool bOnlyLogic, const bool bMinimapIsActive);
	void RenderAlphaObjects(void);

	void SaveAsBinaryFile(BinaryFileWriter& bfw) const;
	void DeserializeFromXML(const classesData_t& classesDataSP, const std::map<assets::xml_class_type, tinyxml2::XMLElement*>& objectsTags, const std::shared_ptr<Surface>& surface);

	[[nodiscard]] bool IsAnyoneWaitingForATarget(void) const;
	void AddGObjWaitingForATarget(const uniqueID_t objUniqueID);
	void RemoveGObjWaitingForATarget(const uniqueID_t objUniqueID);
	void SetCapturedTarget(const std::shared_ptr<Target>& _capturedTarget, const bool _bRightClick);
	[[nodiscard]] std::shared_ptr<Target> TryToGetCapturedTarget(const bool bRightClick) const;


	#pragma region Function used by external scripts
	/// <summary>
	/// This function returns the total number of objects (building, units and decoration) contained in this ObjectsCollection.
	/// </summary>
	/// <returns>The total number of objects.</returns>
	[[nodiscard]] uint32_t GetNumberOfObjects(void) const;

	/// <summary>
	/// This function returns the total number of buildings contained in this ObjectsCollection.
	/// </summary>
	/// <returns>The total number of buildings.</returns>
	[[nodiscard]] uint32_t GetNumberOfBuildings(void) const;

	/// <summary>
	/// This function returns the total number of units contained in this ObjectsCollection.
	/// </summary>
	/// <returns>The total number of units.</returns>
	[[nodiscard]] uint32_t GetNumberOfUnits(void) const;

	/// <summary>
	/// This function returns the total number of decorations contained in this ObjectsCollection.
	/// </summary>
	/// <returns>The total number of decorations.</returns>
	[[nodiscard]] uint32_t GetNumberOfDecorations(void) const;

	/// <summary>
	/// This function return the total number of objects belonging to a specific class contained in this ObjectsCollection.
	/// <param name="className">A wanted class.</param>
	/// </summary>
	/// <returns>The total number of decorations.</returns>
	[[nodiscard]] uint32_t GetNumberOfElementsByClass(string& className) const;

	/// <summary>
	/// <param name="_className">The class of objects of which you want to obtain an ObjsList.</param>
	/// <returns>If the ObjectsCollection contains objects belonging to _className, then an unchangeable ObjsList containg them; otherwise a changeable empty ObjsList will be returned.</returns>
	/// </summary>
	[[nodiscard]] std::shared_ptr<ObjsList> GetObjectsList(std::string& className) const;
	#pragma endregion
private:
	void AddObject(const std::shared_ptr<GObject>& objSP);
	void RemoveObject(const std::shared_ptr<GObject>& objSP);
	void DestroyObject(const std::shared_ptr<GObject>& objSP);
	void MoveObject(const uint8_t newPlayer, const std::shared_ptr<Playable> objSP);

	void Serialize(std::ostream& out, const std::string type) const;

	typedef std::list<std::shared_ptr<GObject>>::iterator iteratorToObj_t;

	struct GObjectInfo
	{
		std::weak_ptr<GObject> objById;
		iteratorToObj_t referenceToListPos;  // A pointer to the position of the GObject into the objs list.
		bool bWaitingForCapturedTarget = false;
	};

	// List containing all the objects inside the collection.
	std::list<std::shared_ptr<GObject>> objs;

	// List containing all buildings and decorations to render with alpha.
	// This list is cleared every frame.
	std::list<std::weak_ptr<GObject>> objsAlpha;

	// A map containing an ObjsList of all the object inside the colletion that belong to a specific class.
	std::unordered_map<std::string, std::shared_ptr<ObjsList>> objsByClass;

	// A map that provides some information about a GObject given its unique ID.
	std::unordered_map<uniqueID_t, GObjectInfo> objsInfoById;

	// The number of GObjects inside the collection that are waiting for a target.
	uint32_t objsWaitingForATarget = 0;

	//The captured target.
	std::shared_ptr<Target> capturedTarget;

	// A boolean indicating if the target was captured via right click (true) or via left click (false)
	bool bRightClick = false;

	// The number of Gobject waiting for a target when a target was setted.
	uint32_t objsWaitingWhenGotTarget = 0;

	//The ID of the player who has this ObjsCollection.
	uint8_t playerdID = 0;

	//Tabs to use during serialization (operator<<)
	static uint8_t nTabs;

	//A weak reference to the array of players.
	static std::weak_ptr<PlayersArray> playersArrayWeakRef;
};
