#include <environments/game/classes/gobject.h>
#include <environments/game/classes/building.h>
#include <environments/game/classes/objectsSet/settlement.h>
#include <environments/game/classes/decoration.h>
#include <environments/game/classes/unit.h>
#include <environments/game/classes/hero.h>
#include <environments/game/classes/wagon.h>
#include <environments/game/classes/objectsStuff/objectGrid.h>

#include <environments/game/editor/editor.h>

#include <encode_utils.h>
#include <math_utils.h>
#include <bin_data_interpreter.h>

#include <environments/game/races/races_interface.h>
#include <players/player.h>
#include <engine.h>
#include <services/sqlservice.h>
#include <services/logservice.h>
#include <fileservice.h>

#include <environments/game/adventure/scenario/surface/surface.h>
#include <camera.h>
#include <png.h>


// Assets
#include <xml_assets.h>
#include <xml_entity.h>
#include <xml_class.h>


#pragma region STATIC
uniqueID_t GObject::NextId = 1;
uint8_t GObject::nTabs = 0;
std::weak_ptr<PlayersArray> GObject::playersWeakRef;

std::shared_ptr<GObject> GObject::Create(const uint8_t _playerID, glm::vec3 _pos, const bool _bTemporary, const classData_t& objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource)
{
	std::shared_ptr<GObject> obj;
	const auto type = objData->get_class_type();
	if (type == centurion::assets::xml_class_type::e_buildingClass)
		obj = std::shared_ptr<GObject>(new Building(_playerID, _pos, _bTemporary, objData, surface, dataSource));
	else if (type == centurion::assets::xml_class_type::e_unitClass)
		obj = std::shared_ptr<GObject>(new Unit(_playerID, _pos, _bTemporary, objData, surface, dataSource));
	else if (type == centurion::assets::xml_class_type::e_heroClass)
		obj = std::shared_ptr<GObject>(new Hero(_playerID, _pos, _bTemporary, objData, surface, dataSource));
	else if (type == centurion::assets::xml_class_type::e_wagonClass)
		obj = std::shared_ptr<GObject>(new Wagon(_playerID, _pos, _bTemporary, objData, surface, dataSource));
	else if (type == centurion::assets::xml_class_type::e_decorationClass)
		obj = std::shared_ptr<GObject>(new Decoration(_pos, _bTemporary, objData, surface, dataSource));

	// Set a weak reference to itself for the created GObject
	obj->me = obj;

	if (obj->IsDecoration() == false)
		std::static_pointer_cast<Playable>(obj)->FinalizeCreation(objData, dataSource, std::static_pointer_cast<Playable>(obj));

	// Grid updating
	if (_bTemporary == false)
		obj->UpdateGrid(true, surface);

	return obj;
}

void GObject::ResetNextId(void) noexcept
{
	GObject::NextId = 1;
}

void GObject::SetNextId(const uniqueID_t nextId)
{
	GObject::NextId = nextId;
}

uniqueID_t GObject::GetNextId(void) noexcept
{
	return GObject::NextId;
}

void GObject::SetTabs(const uint8_t tabs)
{
	GObject::nTabs = tabs;
	SettlementsCollection::SetTabs(tabs + 1);
}
#pragma endregion


#pragma region CREATION, TEMPORARINES AND DESTRUCTION
GObject::GObject(glm::vec3&& _pos, const bool _bTemporary, const classData_t& _objData, const std::shared_ptr<Surface>& surface, gobjData_t* dataSource):
	xmlData(_objData), position(std::shared_ptr<Point>(new Point)), className(_objData->get_class_name_cref()), bIsTemporary(_bTemporary)
{
	// WARNING: parentsSetRef, methodsMap and repeatedMethodsMap share the same address of the respective collections, associated to the same class, inside ClassesData.

	// xml entity
	this->xml_entity = _objData->get_entity();
	auto environment_id = Engine::GetInstance().GetEnvironmentId();
	if (environment_id == IEnvironment::Environments::e_editor)
	{
		this->xml_entity->load_only_idle_textures(this->currentState);
	}
	else if (environment_id == IEnvironment::Environments::e_match)
	{
		this->xml_entity->load_textures();
	}

	// Try to read grid
	std::optional<std::string> grid_path = _objData->get_attribute("grid_path");
	if (grid_path.has_value() == true)
	{
		auto grid = Grid::GetOrReadGrid(grid_path.value(), this->className);
		if (grid)
			this->grid = std::move(grid);
	}

	// Set terrain and surface shortcuts
	this->visibleMapSize = glm::ivec2(surface->GetVisibleMapSize());
	this->terrain = surface->GetTerrainW();

	if (this->bIsTemporary == true)
		return;

	// Set id
	if (dataSource == nullptr)
	{
		this->uniqueID = GObject::NextId;
		GObject::NextId += 1;
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		tinyxml2::XMLElement* xml = std::get<tinyxml2::XMLElement*>(*dataSource);
		this->uniqueID = tinyxml2::TryParseIntAttribute(xml, "id");
		assert(this->uniqueID >= 1);
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		gobjBinData_t& binData = std::get<gobjBinData_t>(*dataSource);
		assert(binData.first != nullptr);
		this->uniqueID = BinaryDataInterpreter::ExtractUInt32((*binData.first), binData.second);
	}
	
	auto hb_size = this->xml_entity->get_entity_size();
	this->HitBox = RectangularArea(glm::ivec2(this->position->x, this->position->y), hb_size.first, hb_size.second, AreaOrigin::Center);

	// origin
	std::string origin_str{ "game/match/origin" };
	auto origin_opt = rattlesmake::image::png::create(origin_str, rattlesmake::image::png_flags_None, 1.f);
	if (origin_opt)
	{
		this->origin = std::move(origin_opt);
	}
	
	this->framesAnimVariation = Engine::GetGameTime().GetCurrentFrame();
}

bool GObject::IsTemporary(void) const noexcept
{
	return this->bIsTemporary;
}

void GObject::SetAsTemporary(void)
{
	this->bIsTemporary = true;
	this->bisTemporaryMultiple = false;
}

void GObject::SetAsTemporaryMultiple(void)
{
	this->bIsTemporary = true;
	this->bisTemporaryMultiple = true;
}

GObject::~GObject(void)
{
}
#pragma endregion



#pragma region PICKING, CLICKING AND SELECTION
void GObject::SetPickingID(const pickingID_t par_pickingID)
{
	this->pickingID = par_pickingID;
	int r = (par_pickingID & 0x000000FF) >> 0;
	int g = (par_pickingID & 0x0000FF00) >> 8;
	int b = (par_pickingID & 0x00FF0000) >> 16;
}

pickingID_t GObject::GetPickingID(void) const noexcept
{
	return this->pickingID;
}

double GObject::GetClickedTime(void) const noexcept
{
	return this->clickedTime;
}

bool GObject::Select(const uint8_t _playerID)
{
	// Don't perform multiple selections of the same GObject.
	if (this->playersThatSelectedMe.contains(_playerID) == true)
		return false;

	assert(GObject::playersWeakRef.expired() == false);
	const auto player = GObject::playersWeakRef.lock()->GetPlayerRef(_playerID);
	assert(player.expired() == false && this->me.expired() == false);
	player.lock()->GetSelection()->Add(this->me.lock());

	this->playersThatSelectedMe.insert(_playerID);
	this->clickedTime = Engine::GetGameTime().GetTotalSeconds();

	return true;
}

bool GObject::IsSelected(void) const noexcept
{
	return (this->playersThatSelectedMe.empty() == false);
}

void GObject::GlobalDeselect(void)
{
	auto playersArray = GObject::playersWeakRef.lock();
	while (this->playersThatSelectedMe.empty() == false)
	{
		const uint8_t playerID = *(this->playersThatSelectedMe.begin());
		#if CENTURION_DEBUG_MODE
		const size_t assertVar = this->playersThatSelectedMe.size();
		#endif  // CENTURION_DEBUG_MODE
		this->Deselect(playerID); 
		// ASSERTION: after deselect, attributes playersThatSelectedMe must not contain playerID.
		assert(assertVar == this->playersThatSelectedMe.size() + 1);
	}
}

bool GObject::Deselect(const uint8_t _playerID)
{
	// GObject must be previously selected by the player
	if (this->playersThatSelectedMe.contains(_playerID) == false)
		return false;

	assert(GObject::playersWeakRef.expired() == false);
	const auto player = GObject::playersWeakRef.lock()->GetPlayerRef(_playerID);
	assert(player.expired() == false && this->me.expired() == false);
	player.lock()->GetSelection()->Remove(this->me.lock());

	this->playersThatSelectedMe.erase(_playerID);

	return true;
}
#pragma endregion

#pragma region TYPE OF OBJECT, CASTING AND RELATIONSHIP
std::string GObject::GetType(void) const noexcept
{
	return assets::xml_class_types_str.at(this->xmlData->get_class_type());
}

assets::xmlClassTypeInt_t GObject::GetTypeInt(void) const noexcept
{
	return static_cast<assets::xmlClassTypeInt_t>(this->xmlData->get_class_type());
}

bool GObject::IsPlayableGObject(void) const noexcept
{
	return (this->xmlData->get_class_type() != assets::xml_class_type::e_decorationClass);
}

bool GObject::IsBuilding(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_buildingClass);
}

bool GObject::IsSimpleUnit(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_unitClass);
}

bool GObject::IsUnit(void) const noexcept
{
	const assets::xml_class_type type = this->xmlData->get_class_type();
	return (type == assets::xml_class_type::e_unitClass
		|| type == assets::xml_class_type::e_heroClass
		|| type == assets::xml_class_type::e_wagonClass
		|| type == assets::xml_class_type::e_druidClass);
}

bool GObject::IsHero(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_heroClass);
}

bool GObject::IsWagon(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_wagonClass);
}

bool GObject::IsDruid(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_druidClass);
}

bool GObject::IsDecoration(void) const noexcept
{
	return (this->xmlData->get_class_type() == assets::xml_class_type::e_decorationClass);
}

bool GObject::IsHeirOf(const std::string& className) const
{
	std::string _class{ className };
	Encode::ToLowercase(&_class);
	return (this->xmlData->get_parent_classes_set_cref().contains(_class));
}
#pragma endregion



#pragma region PLACEABILITY, GRID AND SHIFTING
bool GObject::IsPlaceable(void) const noexcept
{
	return this->bIsPlaceable;
}

void GObject::SetPlacementError(const PlacementErrors _placementError)
{
	this->placementError = _placementError;
}

void GObject::SendInfoText(void) const
{
	auto ui = std::static_pointer_cast<Editor>(Engine::GetInstance().GetEnvironment())->GetUIRef();
	std::string infoText;
	switch (this->placementError)
	{
		case GObject::PlacementErrors::e_no_error:
			{
			// bIsTemporary == true means adding; otherwise means shifting
			const string str = (this->bIsTemporary == true) ? "e_text_can_add_object" : "e_text_can_move_object";
			infoText = SqlService::GetInstance().GetTranslation(str, false);
			}
			break;
		case GObject::PlacementErrors::e_impassable_point:
			infoText = SqlService::GetInstance().GetTranslation("e_text_impassable_point", false);
			break;
		case GObject::PlacementErrors::e_no_settlement_around:
			infoText = SqlService::GetInstance().GetTranslation("e_text_no_settlements_around", false);
			break;
		case GObject::PlacementErrors::e_another_settlement_around:
			infoText = SqlService::GetInstance().GetTranslation("e_text_another_settlement_around", false);
			break;
		case GObject::PlacementErrors::e_enemy_settlement_around:
			infoText = SqlService::GetInstance().GetTranslation("e_text_enemy_settlement_around", false);
			break;
		case GObject::PlacementErrors::e_more_possible_choices:
			infoText = SqlService::GetInstance().GetTranslation("e_text_more_possible_choices", false);
			break;
		case GObject::PlacementErrors::e_settlement_chain_broken:
			infoText = SqlService::GetInstance().GetTranslation("e_text_too_far_away", false);
			break;
		case GObject::PlacementErrors::e_map_border_limit:
			infoText = SqlService::GetInstance().GetTranslation("e_text_map_border_limit", false);
			break;
		default:
			Logger::Warn("Error code " + std::to_string(static_cast<uint8_t>(this->placementError)) + " - generated while placing a GObject; Reason (ID = " + std::to_string(this->uniqueID) + " ) is not valid!");
			break;
	}
	ui->UpdateInfoText(infoText);
}

void GObject::UpdateGrid(const bool bUpdateEvenGrid, const std::shared_ptr<Surface>& surface)
{
	if (this->grid == nullptr)
		return;

	const std::shared_ptr<SurfaceGrid> surfaceGrid = surface->GetSurfaceGrid().lock();
	surfaceGrid->UpdateObjectGrid(this->grid, (*this->position), GridOperations::E_GRID_OVERLAP);
	if (bUpdateEvenGrid == true)
		surfaceGrid->UpdateGrid();
}

void GObject::ClearGrid(const bool bUpdateEvenGrid, const std::shared_ptr<Surface>& surface)
{
	if (!this->grid)
		return;

	const std::shared_ptr<SurfaceGrid> surfaceGrid = surface->GetSurfaceGrid().lock();
	surfaceGrid->UpdateObjectGrid(this->grid, (*this->position), GridOperations::E_GRID_CLEAR);
	if (bUpdateEvenGrid == true)
		surfaceGrid->UpdateGrid();
}
#pragma endregion

#pragma region ENTITY, ANIMATION
std::shared_ptr<centurion::assets::xml_entity> GObject::GetXmlEntity(void) const noexcept
{
	return this->xml_entity;
}

void GObject::ChangeAnim(const std::string& anim)
{
	//Change animation and reset frame if a new animation is started:
	if (anim != this->currentAnim)
	{
		this->SetCurrentAnim(anim);	
		this->SetCurrentFrame(0);
	}
}

void GObject::SetCurrentAnim(const std::string& name)
{
	this->currentAnim = name;
}
void GObject::SetCurrentState(const std::string& name)
{
	this->currentState = name;
}

const std::string& GObject::GetCurrentAnim(void) const noexcept
{
	return this->currentAnim;
}
const std::string& GObject::GetCurrentState(void) const noexcept
{
	return this->currentState;
}

void GObject::SetCurrentFrame(const uint32_t frame)
{
	this->currentFrame = frame;
}

uint32_t GObject::GetCurrentFrame(void) const noexcept
{
	return this->currentFrame;
}

void GObject::SetCurrentDirection(const uint8_t dir)
{
	this->currentDirection = dir;
}

uint8_t GObject::GetCurrentDir(void) const noexcept
{
	return this->currentDirection;
}

#pragma endregion



#pragma region POSITION, CAMERA AND HITBOX
bool GObject::SetPosition(const int x, const int y)
{
	//Check if point has valid coordinates:
	if (x < 0 || y < 0)
		return false;

	// origin position
	this->position->SetCoordinates(x, y);

	// entity position
	this->position->z = terrain.lock()->GetNoiseEstimate(position->x, position->y);
	
	// hitbox
	auto ent_origin_offsets = this->xml_entity->get_origin_offsets();
	this->HitBox.SetPosition(position->x + ent_origin_offsets.first, position->y + ent_origin_offsets.second + this->position->z);
	
	return true;
}

std::shared_ptr<Point> GObject::GetPosition(void) const
{
	return this->position;
}

uint32_t GObject::GetPositionX(void) const
{
	return this->position->x;
}

uint32_t GObject::GetPositionY(void) const
{
	return this->position->y;
}

uint32_t GObject::GetPositionY2D(void) const
{
	return this->position->y + this->position->z;
}

bool GObject::IsInCameraViewport(void) const noexcept
{
	const bool bIsNotInHolder = (this->IsUnit() == true) ? (static_cast<const Unit*>(this)->IsInHolder() == false) : true;
	auto& camera = rattlesmake::peripherals::camera::get_instance();
	auto& viewport = rattlesmake::peripherals::viewport::get_instance();
	return this->HitBox.Intersect(glm::ivec2((int)camera.GetXPosition(), (int)camera.GetYPosition()), (uint32_t)viewport.GetWidthZoomed(), (uint32_t)viewport.GetHeightZoomed(), AreaOrigin::BottomLeft) && bIsNotInHolder;
}

void GObject::RenderOrigin(void)
{
	if (this->origin && rattlesmake::peripherals::camera::get_instance().IsPointInCameraView(glm::vec2(position->x, position->y)) == true)
	{
		auto X = (int)rattlesmake::peripherals::camera::get_instance().GetViewportXCoord(this->position->x);
		auto Y = (int)rattlesmake::peripherals::camera::get_instance().GetViewportYCoord(this->position->y);
		this->origin->render(X - 3, Y - 3, rattlesmake::image::png_flags_::png_flags_NoRepeat, RIL_IMAGE_DEFAULT_VAL, RIL_IMAGE_DEFAULT_VAL);
	}
}

void GObject::RenderHitbox(void)
{
	this->IsSelected() ? this->HitBox.SetBorderColor(glm::vec4(255.f, 255.f, 0.f, 255.f)) : this->HitBox.SetBorderColor(glm::vec4(255.f));

	this->HitBox.AddToDrawQueue();
	if (this->IsBuilding() == true && ((Building*)this)->IsCentralBuilding() == true)
	{
		((Building*)this)->GetSettlement()->GetHitbox().AddToDrawQueue();
	}
}

RectangularArea& GObject::GetHitBox(void)
{
	return this->HitBox;
}

bool GObject::IsHovering(void) const noexcept
{
	return this->HitBox.IsHovered();
}
#pragma endregion



#pragma region SERIALIZATION
void GObject::GetBinSignature(std::vector<uint8_t>& data) const
{
	// Firstly, before push always class, then xPos and then yPos
	BinaryDataInterpreter::PushString(data, this->className);
	BinaryDataInterpreter::PushUInt32(data, this->position->x);
	BinaryDataInterpreter::PushUInt32(data, this->position->y);

	// Then push unique ID
	BinaryDataInterpreter::PushUInt32(data, this->uniqueID);

	// Push other attributes in GObject::GetBinRepresentation
}

void GObject::GetBinRepresentation(std::vector<uint8_t>& data, const bool calledByChild) const
{
	///The attributes needed to create a GObject are saved by GObject::GetBinSignature

	this->GObjectAttributes::GetXmlAttributesAsBinaryData(data);
}

std::ostream& GObject::Serialize(std::ostream& out, const bool calledByChild) const
{
	//Assertion: this function MUST NOT be called directly.
	assert(calledByChild == true);

	// check if this object is inside some areas
	{
		auto area_array = Engine::GetInstance().GetEnvironment()->AsIGame()->GetCurrentScenario()->GetAreaArrayPtr();
		assert(area_array);

		auto areas_list = area_array->GetAreasListPtr();
		auto area_iterator = areas_list->begin();
		while (area_iterator != areas_list->end())
		{
			auto area_ptr = (*area_iterator).lock();
			if (area_ptr && area_ptr->IsPointInArea(glm::vec2(this->GetPositionX(), this->GetPositionY())))
			{
				area_ptr->AddObjectInside(this);
			}
			area_iterator++;
		}
	}

	return out;
}

void GObject::SetAttrs(const classData_t& objData, gobjData_t* dataSource)
{
	this->GObjectAttributes::SetAttributes(objData, dataSource, this->bIsTemporary);

	if (dataSource == nullptr)
	{
		;  // Nothing to save for now
	}
	else if (std::holds_alternative<tinyxml2::XMLElement*>(*dataSource) == true)
	{
		;  // Nothing to save for now
	}
	else if (std::holds_alternative<gobjBinData_t>(*dataSource) == true)
	{
		;  // Nothing to save for now
	}
}

std::ostream& operator<<(std::ostream& out, const GObject& object) noexcept
{
	return object.Serialize(out, false);
}

bool GObject::IsSaved(void) const noexcept
{
	return this->bIsSaved;
}

void GObject::MarkAsSaved(void)
{
	this->bIsSaved = true;
}

void GObject::MarkAsNotSaved(void)
{
	this->bIsSaved = false;
}
#pragma endregion


#pragma region UNIQUE_ID, CLASS, 
uniqueID_t GObject::GetUniqueID(void) const noexcept
{
	return this->uniqueID;
}

const std::unordered_set<std::string>& GObject::GetParentsSetCRef(void) const
{
	return this->xmlData->get_parent_classes_set_cref();
}

void GObject::SetClassName(std::string parClassName)
{
	this->className = std::move(parClassName);
}

std::string GObject::GetClassName(void) const
{
	return this->className;
}

const std::string& GObject::GetClassNameCRef(void) const
{
	return this->className;
}

void GObject::SetPlayersArrayWRef(std::weak_ptr<PlayersArray> _playersWeakRef)
{
	GObject::playersWeakRef = std::move(_playersWeakRef);
}
#pragma endregion


#pragma region OTHER MEMBERS:
double GObject::DistTo(const std::shared_ptr<GObject>& other) const
{
	if (!other)
		return -1.0f;
	return Math::euclidean_distance(this->position->x, this->position->y, other->position->x, other->position->y);
}
#pragma endregion


bool GObject::operator==(const GObject& other) const noexcept
{
	return (this->uniqueID == other.uniqueID);
}

size_t GObject::hash_GObject::operator()(const GObject& gobject) const noexcept
{
	return (std::hash<uint32_t>()(gobject.uniqueID));
}
