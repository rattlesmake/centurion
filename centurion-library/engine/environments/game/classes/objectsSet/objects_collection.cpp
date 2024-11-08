#include "objects_collection.h"

// Classes
#include "objects_list.h"
#include <environments/game/classes/playable.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/decoration.h>
#include <environments/game/classes/unit.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/wagon.h>
#include <environments/game/classes/objectsSet/building_settlement_bridge.h>
#include <environments/game/classes/objectsSet/settlement.h>

// Players
#include <players/players_array.h>

// Utility
#include <encode_utils.h>
#include <tinyxml2_utils.h>
#include <bin_data_interpreter.h>

#include <environments/game/adventure/scenario/surface/surface.h>
#include <environments/game/adventure/scenario/minimap.h>

// Assets
#include <xml_class.h>
#include <xml_classes.h>
#include <xml_entity.h>
#include <xml_entity_shader.h>
#include <xml_entity_layer.h>

// hitbox
#include <rectangle_shader.h>


#pragma region Static attributes initialization
std::weak_ptr<PlayersArray> ObjsCollection::playersArrayWeakRef;
uint8_t ObjsCollection::nTabs = 0;
#pragma endregion


#pragma region Constructor and destructor:
ObjsCollection::ObjsCollection(const uint8_t _playerdID) : playerdID(_playerdID)
{
}

ObjsCollection::~ObjsCollection(void)
{
}
#pragma endregion

#pragma region Operators:
std::ostream& operator<<(std::ostream& out, const ObjsCollection& objCollection) noexcept
{
	const string tabs(ObjsCollection::nTabs, '\t');
	GObject::SetTabs(ObjsCollection::nTabs + 1);
	out << tabs << "<Decorations>";
	if (objCollection.objsByClass.contains("decoration") == true)
	{
		// Assertion: only the undefined player MUST have decorations.
		assert(objCollection.playerdID == PlayersArray::UNDEFINED_PLAYER_INDEX);

		objCollection.Serialize(out, "decoration");
	}
	else
		out << '\n';
	out << tabs << "</Decorations>";

	out << '\n' << tabs << "<Buildings>";
	if (objCollection.objsByClass.contains("building") == true)
	{
		// Assertion: only the undefined player MUST NOT have buildings
		assert(objCollection.playerdID != PlayersArray::UNDEFINED_PLAYER_INDEX);

		objCollection.Serialize(out, "building");
	}
	else
		out << '\n';
	out << tabs << "</Buildings>";

	out << '\n' << tabs << "<Units>";
	if (objCollection.objsByClass.contains("unit") == true)
	{
		// Assertion: only the undefined player MUST NOT have units
		assert(objCollection.playerdID != PlayersArray::UNDEFINED_PLAYER_INDEX);

		objCollection.Serialize(out, "unit");
	}
	else
		out << '\n';
	out << tabs << "</Units>";

	return out;
}
#pragma endregion


#pragma region Static members:
std::shared_ptr<GObject> ObjsCollection::CreateGObject(const classesData_t& classesDataSP, const std::string& className, std::list<std::pair<glm::vec2, bool>>& points, uint8_t playerID, const bool bTemporary, const std::shared_ptr<Surface>& surface)
{
	assert(classesDataSP);

	std::shared_ptr<GObject> objSP;
	while (points.empty() == false)
	{
		const bool bIsPointPlaceable = points.front().second == true;
		if (bIsPointPlaceable == true)  // Continue only if the GObject can be placed in this point
		{
			// Create a new GObject.
			auto objDataSP = classesDataSP->get_xml_class(className);
			// Assertion: class must exist.
			assert(objDataSP);
			const glm::vec3 pos{ points.front().first.x, points.front().first.y, 0.f };
			objSP = GObject::Create(playerID, pos, bTemporary, objDataSP, surface, nullptr);  // nullptr means that data must be read neither by XML save file nor by binary data save file.

			if (bTemporary == false)
			{
				assert(ObjsCollection::playersArrayWeakRef.expired() == false);
				const std::shared_ptr<PlayersArray> playersArraySP = ObjsCollection::playersArrayWeakRef.lock();
				if (objSP->IsBuilding() == true)
				{
					auto settlementDataSP = classesDataSP->get_xml_class("settlement");
					assert(settlementDataSP);
					BuildingSettlementBridge::AssignSettlement(std::move(settlementDataSP), std::static_pointer_cast<Building>(objSP), nullptr, playersArraySP);  // nullptr means that data must be read neither by XML save file nor by binary data save file.
				}

				//Add the created object to the ObjsCollection belonging to the correct player (player having ID = playerID).
				playersArraySP->GetPlayerRef(playerID).lock()->GetObjsCollection()->AddObject(objSP);
			}
		}
		points.pop_front();
	}

	return std::shared_ptr<GObject>{objSP};
}

std::shared_ptr<GObject> ObjsCollection::CreateGObjectFromXml(const classesData_t& classesDataSP, tinyxml2::XMLElement* xml, const uint8_t playerID, const std::shared_ptr<Surface>& surface)
{
	assert(xml != nullptr);
	assert(classesDataSP);

	// TODO exception - sostituire asserzioni con try-catch (xml potrebbe essere corrotto)
	const std::string className = tinyxml2::TryParseStrAttribute(xml, "class");

	auto objDataSP = classesDataSP->get_xml_class(className);
	//Assertion: class must exist.
	assert(objDataSP);

	const int x = tinyxml2::TryParseIntAttribute(xml, "xPos");
	const int y = tinyxml2::TryParseIntAttribute(xml, "yPos");
	assert(x >= 0 && y >= 0);

	// Create a new GObject
	const glm::vec3 pos{ x, y, 0.f };
	gobjData_t objXML{ xml };
	std::shared_ptr<GObject> objSP = GObject::Create(playerID, pos, false, objDataSP, surface, &objXML);
	assert(objSP->GetUniqueID() == tinyxml2::TryParseIntAttribute(xml, "id"));

	// Add the created object to the ObjsCollection belonging to the correct player (player having ID = playerID)
	const std::shared_ptr<PlayersArray> playersArraySP = ObjsCollection::playersArrayWeakRef.lock();
	assert(ObjsCollection::playersArrayWeakRef.expired() == false);
	playersArraySP->GetPlayerRef(playerID).lock()->GetObjsCollection()->AddObject(objSP);

	if (objSP->IsBuilding() == true)
	{
		auto settlementDataSP = classesDataSP->get_xml_class("settlement");
		assert(settlementDataSP);
		// Create or assign a settlement to the created building
		gobjData_t settlementSourceData{ xml };
		BuildingSettlementBridge::AssignSettlement(std::move(settlementDataSP), std::static_pointer_cast<Building>(objSP), &settlementSourceData, playersArraySP);
	}
	return objSP;
}

std::shared_ptr<GObject> ObjsCollection::CreateGObjectFromBinData(const classesData_t& classesDataSP, std::vector<byte_t>&& data, const uint8_t playerID, const std::shared_ptr<Surface>& surface)
{
	assert(classesDataSP);

	uint32_t dataOffset = 0;

	// Check if the GObject was saved (in order to understand this situation, see ObjsCollection::SaveAsBinaryFile)
	const bool bWasSaved = BinaryDataInterpreter::ExtractBoolean(data, dataOffset);
	if (bWasSaved == false)
		return std::shared_ptr<GObject>();  // Nothing to read, so return

	const std::string className = BinaryDataInterpreter::ExtractString(data, dataOffset);
	auto objDataSP = classesDataSP->get_xml_class(className);
	if (!objDataSP)
		throw BinaryDeserializerException("Cannot find a class having name " + className);

	const uint32_t xPos = BinaryDataInterpreter::ExtractUInt32(data, dataOffset);
	const uint32_t yPos = BinaryDataInterpreter::ExtractUInt32(data, dataOffset);
	if (xPos <= 0 || yPos <= 0)  // TODO aggiungere anche i limiti superiori, anzi probabilmente bastano solo quelli essendo pos un uint32
		throw BinaryDeserializerException("Creation of a " + className + " --> Bad position (" + std::to_string(xPos) + ", " + std::to_string(yPos) + ")");

	// Create a new GObject.
	gobjData_t objBytes{ std::make_pair(&data, dataOffset) };
	const glm::vec3 pos{ xPos, yPos, 0.f };
	std::shared_ptr<GObject> objSP = GObject::Create(playerID, pos, false, objDataSP, surface, &objBytes);

	// Add the created object to the ObjsCollection belonging to the correct player (player having ID = playerID)
	const std::shared_ptr<PlayersArray> playersArraySP = ObjsCollection::playersArrayWeakRef.lock();
	assert(ObjsCollection::playersArrayWeakRef.expired() == false);
	playersArraySP->GetPlayerRef(playerID).lock()->GetObjsCollection()->AddObject(objSP);

	if (objSP->IsBuilding() == true)
	{
		auto settlementDataSP = classesDataSP->get_xml_class("settlement");
		assert(settlementDataSP);
		BuildingSettlementBridge::AssignSettlement(std::move(settlementDataSP), std::static_pointer_cast<Building>(objSP), &objBytes, playersArraySP);
	}

	return objSP;
}

void ObjsCollection::DestroyGObject(const std::shared_ptr<GObject>& objSP)
{
	#if CENTURION_DEBUG_MODE
	if (objSP->IsBuilding() == true || objSP->IsDecoration() == true)
		assert(Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor);  // Buildings and decorations can be destroyed only in the editor enviroment.
	#endif // CENTURION_DEBUG_MODE

	const std::shared_ptr<PlayersArray> playersArraySP = ObjsCollection::playersArrayWeakRef.lock();
	if (Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_editor)
	{
		// In editor, it's important to deselect the GObject before destroy it (in match, a unit is deselected automatically when it dies).
		objSP->GlobalDeselect();
		// It's important to remove building from a settlement before destroy it. It is important to do here for the cancellation of the entire settlement to work.
		if (objSP->IsBuilding() == true)
			std::static_pointer_cast<Building>(objSP)->GetSettlement()->RemoveBuildingFromSettlement(std::static_pointer_cast<Building>(objSP));
	}

	assert(ObjsCollection::playersArrayWeakRef.expired() == false);
	const uint8_t playerID = (objSP->IsPlayableGObject() == true) ? std::static_pointer_cast<Playable>(objSP)->GetPlayer() : PlayersArray::UNDEFINED_PLAYER_INDEX;
	playersArraySP->GetPlayerRef(playerID).lock()->GetObjsCollection()->DestroyObject(objSP);
}

void ObjsCollection::SetPlayersArrayWeakRef(const std::weak_ptr<PlayersArray>& weakRef)
{
	ObjsCollection::playersArrayWeakRef = weakRef;
	ObjsList::SetPlayersArrayWeakRef(weakRef);
}

void ObjsCollection::RenderTemporaryObject(std::shared_ptr<GObject> obj, bool applyTerrainHeight, std::list<std::pair<glm::vec2, bool>>& offsets)
{
	if (offsets.size() == 0)
		return;

	auto& ent_shader = centurion::assets::xml_entity_shader::get_instance();

	for (auto& offset : offsets)
	{
		obj->SetPosition(offset.first.x, offset.first.y);
		obj->CheckPlaceability();
		// Store placeability result for this offset
		offset.second = obj->IsPlaceable();

		auto xml_entity = obj->GetXmlEntity();
		glm::vec3 player_color{ 0.f };
		if (obj->IsPlayableGObject())
			player_color = std::static_pointer_cast<Playable>(obj)->GetPlayerRef()->GetColor().ToGlmVec3() * 255.f;

		ent_shader.new_object_draw_data(
			obj->GetCurrentFrame(),
			obj->GetCurrentDir(),
			player_color,
			1, // level
			false,
			obj->IsPlaceable(),
			obj->IsUnit()
		);
		xml_entity->render(obj->GetCurrentState(), obj->GetCurrentAnim(), obj->GetPositionX(), obj->GetPositionY());
	}
}

void ObjsCollection::SetTabs(const uint8_t tabs)
{
	ObjsCollection::nTabs = tabs;
}
#pragma endregion


void ObjsCollection::RenderAndPicking(std::unordered_map<pickingID_t, uniqueID_t>& objsByPickingId, uint32_t& counter)
{
	const auto& scenario = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario();

	// Perform cycle of rendering for picking.
	for (std::shared_ptr<GObject>& obj_it : this->objs)
	{
		counter += 1;
		obj_it->SetPickingID(counter);  // It updates the picking ID (necessary for instance when an object has been erased).
		if (obj_it->IsHovering() == true && obj_it->IsInCameraViewport() == true)
		{
			// new render function
			auto xml_entity = obj_it->GetXmlEntity();

			centurion::assets::xml_entity_shader::get_instance().new_object_picking_draw_data(
				obj_it->GetCurrentFrame(),
				obj_it->GetCurrentDir(),
				obj_it->GetPickingID()
			);

			float yPosition = obj_it->GetPositionY2D();
			if (scenario->IsDrawWithoutNoiseActive() == true) yPosition = obj_it->GetPositionY();

			xml_entity->render(obj_it->GetCurrentState(), obj_it->GetCurrentAnim(), obj_it->GetPositionX(), yPosition);

		}
		objsByPickingId[counter] = obj_it->GetUniqueID();
	}
}

void ObjsCollection::RenderAndLogic(const bool bOnlyLogic, const bool bMinimapIsActive)
{
	if (this->objs.size() == 0)
		return;

	auto& ent_shader = centurion::assets::xml_entity_shader::get_instance();
	auto& rectShader = rattlesmake::geometry::rectangle_shader::get_instance();

	const auto& scenario = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario();
	assert(scenario);

	std::list<std::shared_ptr<GObject>>::iterator objsIt = this->objs.begin();
	while (objsIt != this->objs.end())
	{
		std::shared_ptr<GObject> obj = (*objsIt);
		objsIt++;

		/*
			LOGIC
		*/
		{
			obj->ApplyGameLogics();
			const bool bDeletionCondition = (obj->IsUnit() == true && std::static_pointer_cast<Unit>(obj)->IsToBeErased() == true);

			// PlayableGObject has changed player. Maybe current ObjsCollection now is sad :/
			if (obj->IsPlayableGObject() == true && std::static_pointer_cast<Playable>(obj)->GetPlayer() != this->playerdID)
			{
				this->MoveObject(std::static_pointer_cast<Playable>(obj)->GetPlayer(), std::static_pointer_cast<Playable>(obj));
			}
			if (bDeletionCondition == true) // Remove element.
			{
				ObjsCollection::DestroyGObject(obj);
				uint32_t counter = obj.use_count();
				assert(counter == 1);
				continue;
				// Here obj should be destroyed.
			}
			if (bOnlyLogic == true)
			{
				continue;
			}
		}
		// end of logic scope


		/*
			DRAW
		*/
		{
			if (bMinimapIsActive == false && obj->IsInCameraViewport() == false)
			{
				continue;
			}

			auto xml_entity = obj->GetXmlEntity();
			glm::vec3 player_color = obj->IsPlayableGObject() ? std::static_pointer_cast<Playable>(obj)->GetPlayerRef()->GetColor().ToGlmVec3() * 255.f : glm::vec3{ 0.f };
			uint16_t level = obj->IsUnit() ? std::static_pointer_cast<Unit>(obj)->GetLevel() : 1;

			ent_shader.new_object_draw_data(
				obj->GetCurrentFrame(),
				obj->GetCurrentDir(),
				player_color,
				level, // level
				obj->IsSelected(),
				obj->IsPlaceable(),
				obj->IsUnit()
			);

			float yPosition = obj->GetPositionY2D();
			if (scenario->IsDrawWithoutNoiseActive() == true) yPosition = obj->GetPositionY();

			xml_entity->render(obj->GetCurrentState(), obj->GetCurrentAnim(), obj->GetPositionX(), yPosition);

			if (scenario->IsHitboxRenderingActive() == true)
			{
				obj->RenderHitbox();
			}
		}
		// end of draw scope
	}
}

void ObjsCollection::SaveAsBinaryFile(BinaryFileWriter& bfw) const
{
	/*
		The first byte of the vector objBinData contains a boolen variable that indicates if the GObject is saved.
		The additional bytes contain the binary representation of a GObject.
		A Gobject is stored into a binary save file only if either it's NOT a unit or if it's a unit and it is NOT into a "TO_ERASE" state.
		There is no point in saving an unit that is into a "TO_ERASE" state, because it will be deleted from the memory in the frame after loading.
		However, it's very important to store the information when a GObject (or better a Unit) is saved, at least.
		This because the function PlayersArray::SaveAsBinaryFile, before being called, saves GObjects' number inside this ObjsCollection.
		PlayersArray::DeserializeFromBinFile expects to read an equal amount, because it would treat the binary as invalid, otherwise.
	*/

	std::vector<uint8_t> objBinData;
	for (auto const& obj_it : this->objs)
	{
		assert(obj_it);
		const bool bObjectToSave = (obj_it->IsUnit() == false) || (std::static_pointer_cast<Unit>(obj_it)->IsToBeErased() == false);
		if (bObjectToSave)
		{
			BinaryDataInterpreter::PushBoolean(objBinData, true);  // This boolean will indicate to ObjsCollection::CreateGObjectFromBinData that the GObject was previously FULLY saved.
			obj_it->GetBinRepresentation(objBinData, false);
		}
		else
		{
			BinaryDataInterpreter::PushBoolean(objBinData, false);  // This boolean will indicate to ObjsCollection::CreateGObjectFromBinData that the UNIT was NOT saved.
		}
		bfw.PushBinaryData(std::move(objBinData));
	}
}

void ObjsCollection::DeserializeFromXML(const classesData_t& classesDataSP, const std::map<assets::xml_class_type, tinyxml2::XMLElement*>& objectsTags, const std::shared_ptr<Surface>& surface)
{
	for (auto const& it_tag : objectsTags)
	{
		for (tinyxml2::XMLElement* tag = it_tag.second->FirstChildElement(); tag != nullptr; tag = tag->NextSiblingElement())
		{
			auto obj = ObjsCollection::CreateGObjectFromXml(classesDataSP, tag, this->playerdID, surface);
			#if CENTURION_DEBUG_MODE
			// ASSERTION: tag inside the xml must be consistent (e.g. Units tag must contains only units).
			// When assertion will be disabled, in this part of code, maybe, we can substituite it with an AdventureIntegrityCheck = false.
			switch (it_tag.first)
			{
			case assets::xml_class_type::e_decorationClass:
				assert(obj->IsDecoration() == true);
				break;
			case assets::xml_class_type::e_buildingClass:
				assert(obj->IsBuilding() == true);
				break;
			case assets::xml_class_type::e_unitClass:
				assert(obj->IsUnit() == true);
				break;
			default:
				assert(true == false);
				break;
			}
			#endif
		}
	}
}

bool ObjsCollection::IsAnyoneWaitingForATarget(void) const
{
	return (this->objsWaitingForATarget >= 1);
}

void ObjsCollection::AddGObjWaitingForATarget(const uniqueID_t objUniqueID)
{
	// ASSERTION: ObjsCollection must have this ID.
	assert(this->objsInfoById.contains(objUniqueID) == true);
	// ASSERTION: obj must not already waiting for a target:
	assert(this->objsInfoById.at(objUniqueID).bWaitingForCapturedTarget == false);
	this->objsInfoById.at(objUniqueID).bWaitingForCapturedTarget = true;
	// A new PlayableGObject inside this collection is waiting for a target.
	this->objsWaitingForATarget += 1;
}

void ObjsCollection::RemoveGObjWaitingForATarget(const uniqueID_t objUniqueID)
{
	// ASSERTION: ObjsCollection must have this ID
	assert(this->objsInfoById.contains(objUniqueID) == true);
	// ASSERTION: obj must waiting for a target:
	assert(this->objsInfoById.at(objUniqueID).bWaitingForCapturedTarget == true);
	this->objsInfoById.at(objUniqueID).bWaitingForCapturedTarget = false;
	// A PlayableGObject inside this collection is not waiting for a target anymore
	this->objsWaitingForATarget -= 1;
	this->objsWaitingWhenGotTarget--;

	// If none in my collection is waiting for the last captured target, remove the reference to the target
	if (this->objsWaitingWhenGotTarget == 0)
		this->capturedTarget = std::shared_ptr<Target>();
}

void ObjsCollection::SetCapturedTarget(const std::shared_ptr<Target>& _capturedTarget, const bool _bRightClick)
{
	this->capturedTarget = _capturedTarget;
	if (_bRightClick == false)
		this->objsWaitingWhenGotTarget = this->objsWaitingForATarget;
	// Sets the click with which the target was captured
	this->bRightClick = _bRightClick;
}

std::shared_ptr<Target> ObjsCollection::TryToGetCapturedTarget(const bool bRightClick) const
{
	if (this->bRightClick != bRightClick)
		return std::shared_ptr<Target>();
	return this->capturedTarget;
}

void ObjsCollection::AddObject(const std::shared_ptr<GObject>& objSP)
{
	static std::unordered_set<std::string> set{ "decoration" };

	// Firstly, add a reference to the GObject inside the PlayersArray
	this->playersArrayWeakRef.lock()->AddGObject(objSP);

	#if CENTURION_DEBUG_MODE
	// Assertion: check type
	if (objSP->IsDecoration() == false)
		assert(this->playerdID != PlayersArray::UNDEFINED_PLAYER_INDEX);
	else
		assert(this->playerdID == PlayersArray::UNDEFINED_PLAYER_INDEX);
	#endif 

	// Add the GObject inside a list.
	this->objs.push_back(objSP);

	const uniqueID_t ID = objSP->GetUniqueID();
	this->objsInfoById.insert({ ID, GObjectInfo() });
	// Get a reference to the element just added to the map.
	auto& objInfo = objsInfoById.at(ID);
	// Store a pointer to the list position in which there is the inserted object.
	{
		iteratorToObj_t it = this->objs.end();
		it--;
		objInfo.referenceToListPos = it;
	}
	// Add a reference to the object.
	objInfo.objById = objSP;

	#pragma region Classes association
	/// Add the GObject in an ObjsList containing all the GObjects belonging to a specific class.
	/// We use both the current class and the parents classes.
	const bool bDecoration = objSP->IsDecoration();
	// Get the hierarchy. If the GObject is a decoration, consider only "decoration".
	auto& parentsSetCRef = (bDecoration == false) ? objSP->GetParentsSetCRef() : set;
	const uint32_t nClasses = static_cast<uint32_t>(parentsSetCRef.size()) + (bDecoration == false);
	// Cycle on the classes 
	std::unordered_set<std::string>::const_iterator classes_it = parentsSetCRef.cbegin();
	for (uint32_t i = 0; i < nClasses; ++i)
	{
		// Get next class name from which to add the GObject
		const std::string& _class = (classes_it != parentsSetCRef.cend()) ? (*classes_it++) : objSP->GetClassName();

		if (this->objsByClass.contains(_class) == true)  // The ObjsCollection contains already GObjects having the same class of the current GObject
		{
			// Get a reference of the corresponding ObjsList and add an other element inside it
			auto& olOfElementsBeloingigToAClassRef = this->objsByClass.at(_class);
			olOfElementsBeloingigToAClassRef->Insert(objSP);
		}
		else  // The ObjsCollection doesn't contains GObjects having the same class of the current GObject yet
		{
			// Create a new ObjsList (which can't be modified by external scripts) and add its first element
			std::shared_ptr<ObjsList> objListSP = std::shared_ptr<ObjsList>{ new ObjsList(false) };
			objListSP->Insert(objSP);
			this->objsByClass[_class] = std::move(objListSP);
		}
	}
	#pragma endregion
}

void ObjsCollection::RemoveObject(const std::shared_ptr<GObject>& objSP)
{
	static std::unordered_set<std::string> set{ "decoration" };

	const uniqueID_t ID = objSP->GetUniqueID();
	// Assertion: an ObjsCollection MUST have the ID.
	assert(this->objsInfoById.contains(ID) == true);

	#pragma region Classes de-association
	/// Remove the GObject in an ObjsList containing all the GObjects belonging to a specific class.
	/// We use both the current class and the parents classes.
	const bool bDecoration = objSP->IsDecoration();
	// Get the hierarchy. If the object is a decoration, consider only "decoration"
	auto& parentsSetCRef = (bDecoration == false) ? objSP->GetParentsSetCRef() : set;
	const uint32_t nClasses = static_cast<uint32_t>(parentsSetCRef.size()) + (bDecoration == false);
	// Cycle on the classes 
	std::unordered_set<std::string>::const_iterator classes_it = parentsSetCRef.cbegin();
	for (uint32_t i = 0; i < nClasses; ++i)
	{
		// Get next class name from which to add the GObject
		const std::string& _class = (classes_it != parentsSetCRef.cend()) ? (*classes_it++) : objSP->GetClassName();
		// ASSERTION: Class must be present
		assert(this->objsByClass.contains(_class) == true);
		// Get ObjsList associated to the class
		auto& olOfElementsBeloingigToAClassRef = this->objsByClass.at(_class);
		olOfElementsBeloingigToAClassRef->GetOut(objSP);
	}
	#pragma endregion

	// Delete GObject references from the PlayersArray (i.e. uniqueID and scriptID)
	this->playersArrayWeakRef.lock()->RemoveGObject(objSP);

	// Lastly, remove the GObject and all the info associated to it
	iteratorToObj_t it = this->objsInfoById.at(ID).referenceToListPos;
	this->objsInfoById.erase(ID);
	this->objs.erase(it);
}

void ObjsCollection::DestroyObject(const std::shared_ptr<GObject>& objSP)
{
	if ((Engine::GetInstance().GetEnvironmentId() == IEnvironment::Environments::e_match) &&
		(objSP->IsUnit() == true && std::static_pointer_cast<Unit>(objSP)->IsDead() == false))
	{
		// Lets die the unit, before destroy it.
		// It can be strange. you might ask yourself: why should a unit be erased in a match if it isn't die yet?
		// Well, I don't know accurately an answer but it is better do it in order to be safe. Death behavior is extremely complex to manage and to forecast.
		std::static_pointer_cast<Unit>(objSP)->SetHealth(0);
	}
	// Clear grid:
	objSP->ClearGrid(true, Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetSurface());

	// Remove GObject from the collection
	this->RemoveObject(objSP);
}

void ObjsCollection::MoveObject(const uint8_t newPlayer, const std::shared_ptr<Playable> objSP)
{
	// Sell the property of the GObject
	this->RemoveObject(objSP);
	ObjsCollection::playersArrayWeakRef.lock()->GetPlayerRef(newPlayer).lock()->GetObjsCollection()->AddObject(objSP);
}

void ObjsCollection::Serialize(std::ostream& out, const std::string type) const
{
	out << '\n';
	auto& units = this->objsByClass.at(type);
	ObjsList::objs_const_iterator it_ol = units.get()->cbegin();
	ObjsList::objs_const_iterator it_ol_end = units.get()->cend();
	while (it_ol != it_ol_end)  // Iterate over the ObjsList
	{
		// Assertion: an ObjsCollection MUST NOT have expired GObjects
		assert((*it_ol).expired() == false);

		const uniqueID_t id = (*it_ol).lock()->GetUniqueID();

		// Assertion: ObjsList MUST NOT HAVE invalid IDs
		assert(this->objsInfoById.contains(id));

		out << (*(*it_ol).lock()) << '\n';
		it_ol++;  // Go to the next ID inside the ObjsList
	}
}

#pragma region Function used by external scripts
uint32_t ObjsCollection::GetNumberOfObjects(void) const
{
	return static_cast<uint32_t>(this->objs.size());
}

uint32_t ObjsCollection::GetNumberOfBuildings(void) const
{
	std::string className{ "building" };
	return this->GetNumberOfElementsByClass(className);
}

uint32_t ObjsCollection::GetNumberOfUnits(void) const
{
	std::string className{ "unit" };
	return this->GetNumberOfElementsByClass(className);
}

uint32_t ObjsCollection::GetNumberOfDecorations(void) const
{
	std::string className{ "decoration" };
	return this->GetNumberOfElementsByClass(className);
}

uint32_t ObjsCollection::GetNumberOfElementsByClass(string& className) const
{
	Encode::ToLowerCase(className);
	return (this->objsByClass.contains(className) == true) ? (uint32_t)this->objsByClass.at(className)->Count() : 0;
}

std::shared_ptr<ObjsList> ObjsCollection::GetObjectsList(std::string& className) const
{
	Encode::ToLowerCase(className);
	const bool bCondition = (this->objsByClass.contains(className) == true);
	return (bCondition) ? this->objsByClass.at(className) : std::shared_ptr<ObjsList>(new ObjsList(true));
}
#pragma endregion 
