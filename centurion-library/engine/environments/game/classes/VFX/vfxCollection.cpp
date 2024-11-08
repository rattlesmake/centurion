#include "vfxCollection.h"
#include <environments/game/classes/VFX/vfx.h>
#include <engine.h>
#include <environments/game/adventure/scenario/minimap.h>

#include <xml_entity.h>
#include <xml_entity_shader.h>
#include <xml_entity_layer.h>

VFXCollection::VFXCollection(void)
{
}

VFXCollection::~VFXCollection(void)
{
}

void VFXCollection::RenderVFXs(void)
{
	return; // TODO - RenderVFXs
	static Engine& engine = Engine::GetInstance();

	auto& entity_shader = centurion::assets::xml_entity_shader::get_instance();
	entity_shader.initialize_draw_data();

	std::list<std::unique_ptr<VFX>>::iterator vfx_it = this->vfxList.begin();
	while (vfx_it != this->vfxList.end())
	{
		auto& vfx = (*vfx_it);
		vfx->ApplyGameLogics();

		if (vfx->bToErase == false)
		{
			// drawing logics
			if (VFXCollection::CheckRenderingConditions((*vfx)) == false)
			{
				vfx_it++;
				continue;
			}
				
			auto& xml_entity = vfx->GetEntity();
			glm::vec3 player_color{ 0.f };

			entity_shader.new_object_draw_data(
				vfx->currentFrame,
				vfx->currentDirection,
				player_color,
				0,
				false,
				true,
				false
			);
			xml_entity.render(vfx->currentState, vfx->currentAnim, vfx->position.x, vfx->position.y);

			vfx_it++; //Go to the next element.
		}
		else
		{
			vfx_it = this->vfxList.erase(vfx_it); //Remove an element from the list and go to the next.
			//Here destructor will be invoked.
		}
	}
}

void VFXCollection::AddVFX(std::unique_ptr<VFX>&& vfx)
{
	this->vfxList.push_back(std::move(vfx));
}

bool VFXCollection::CheckRenderingConditions(const VFX& vfx)
{
	if (vfx.IsInCameraViewport() == false)
		return false;

	auto& entity = vfx.xml_entity;
	if (entity == nullptr)
		return false;
	
	return entity->check_if_animation_exists(vfx.currentState, vfx.currentAnim);
}
