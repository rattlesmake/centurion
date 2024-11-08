#include "vfx.h"

#include <engine.h>
#include <encode_utils.h>
#include <camera.h>
#include <viewport.h>

#include <xml_assets.h>
#include <xml_class.h>
#include <xml_entity.h>

#pragma region Constructor and destructor:
VFX::VFX(std::string& _className, const VFXTypes _type, const uint32_t startX, const uint32_t startY) :
	className(std::move(_className)), type(_type), position(startX, startY)
{
	// xml entity
	auto xml_classes = std::static_pointer_cast<IGame>(Engine::GetInstance().GetEnvironment())->GetXmlAssets()->get_xml_classes();
	auto environment_id = Engine::GetInstance().GetEnvironmentId();
	auto xml_class = xml_classes->get_xml_class(className);
	this->xml_entity = xml_class->get_entity();
	this->xml_entity->load_textures();

	// Hitbox
	auto hb_size = this->xml_entity->get_entity_size();
	this->HitBox = RectangularArea(glm::ivec2(this->position.x, this->position.y), hb_size.first, hb_size.second, AreaOrigin::Center);

	this->HitBox.SetPosition(glm::vec2(this->position.ToVec3()));

	this->framesAnimVariation = Engine::GetGameTime().GetCurrentFrame();
}

VFX::~VFX(void)
{
}
#pragma endregion


VFXTypes VFX::GetType(void) const
{
	return this->type;
}

void VFX::ApplyGameLogics(void)
{
	// exceptions
	//TODO

	// animation	
	const uint32_t currentGameFrame = Engine::GetGameTime().GetCurrentFrame();
	if (currentGameFrame - this->framesAnimVariation >= this->xml_entity->get_anim_frame_duration(this->currentState, this->currentAnim))
	{
		this->framesAnimVariation = currentGameFrame;
		this->currentFrame++;
		if (this->currentFrame >= this->xml_entity->get_anim_number_of_frames(this->currentState, this->currentAnim))
		{
			this->currentFrame = 0;
			return;
		}
	}
}

const centurion::assets::xml_entity& VFX::GetEntity(void) const
{
	return (*this->xml_entity);
}

const std::string& VFX::GetCurrentAnim(void) const
{
	return this->currentAnim;
}
const std::string& VFX::GetCurrentState(void) const
{
	return this->currentState;
}

void VFX::SetPosition(const uint32_t x, const uint32_t y)
{
	this->position.SetCoordinates(x, y);
	this->HitBox.SetPosition(glm::vec2(this->position.ToVec3()));
}

void VFX::SetPosition(const Point& pt)
{
	this->SetPosition(pt.x, pt.y);
}

void VFX::SetCurrentDirection(const float angle)
{
	this->currentDirection = static_cast<uint8_t>(round(angle / 360 * this->xml_entity->get_number_of_directions()));
}

bool VFX::IsInCameraViewport(void) const
{
	auto& camera = rattlesmake::peripherals::camera::get_instance();
	auto& viewport = rattlesmake::peripherals::viewport::get_instance();
	return this->HitBox.Intersect(glm::ivec2((int)camera.GetXPosition(), (int)camera.GetYPosition()), (uint32_t)viewport.GetWidthZoomed(), (uint32_t)viewport.GetHeightZoomed(), AreaOrigin::BottomLeft);
}
