#include "xml_entity_layer.h"
#include "xml_entity_image.h"
#include "xml_entity_shader.h"
#include "xml_entity.h"

// utils
#include <tinyxml2_utils.h>
#include <encode_utils.h>

// stl
#include <assert.h>
#include <iostream>

namespace centurion
{
	namespace assets
	{
		using tinyxml2::TryParseStrAttribute;
		using tinyxml2::TryParseIntAttribute;
		using tinyxml2::TryParseBoolAttribute;


		xml_entity_layer::xml_entity_layer(tinyxml2::XMLElement* xml_layer) :
			id((uint32_t)TryParseIntAttribute(xml_layer, "id", -1)),
			base_image_id(TryParseIntAttribute(xml_layer, "image", -1)),
			mask_image_id(TryParseIntAttribute(xml_layer, "mask_image", -1)),
			x(TryParseIntAttribute(xml_layer, "x")),
			y(TryParseIntAttribute(xml_layer, "y")),
			z(TryParseIntAttribute(xml_layer, "z")),
			valid(id != -1 && base_image_id != -1),
			xml_ent_shader(xml_entity_shader::get_instance())
		{
			assert(valid);
		}
			

		void xml_entity_layer::render(const xml_entity* entity_ptr) const
		{
			glm::uvec2 base_image_size;
			auto base_img = entity_ptr->get_image_by_id(this->base_image_id);
			bool has_mask = this->mask_image_id != -1;

			base_image_size.x = base_img->get_width();
			base_image_size.y = base_img->get_height();
			uint32_t base_img_opengl_id = base_img->get_opengl_texture_id();
			int32_t mask_img_opengl_id = -1;
			if (has_mask)
			{
				mask_img_opengl_id = entity_ptr->get_image_by_id(this->mask_image_id)->get_opengl_texture_id();
			}

			xml_ent_shader.new_layer_draw_data(has_mask, base_image_size, base_img_opengl_id, mask_img_opengl_id);
		}

		xml_entity_layer::xml_entity_layer(tinyxml2::XMLElement* xml_layer, const uint32_t child) :
			id((uint32_t)TryParseIntAttribute(xml_layer, "id", -1)),
			base_image_id(TryParseIntAttribute(xml_layer, "image", -1)),
			x(TryParseIntAttribute(xml_layer, "x")),
			y(TryParseIntAttribute(xml_layer, "y")),
			z(TryParseIntAttribute(xml_layer, "z")),
			valid(id != -1 && base_image_id != -1),
			xml_ent_shader(xml_entity_shader::get_instance())
		{
		}


		xml_entity_shadow::xml_entity_shadow(tinyxml2::XMLElement* xml_shadow) : xml_entity_layer(xml_shadow, 1)
		{
		}

		void xml_entity_shadow::render(const xml_entity* entity_ptr) const
		{
			glm::uvec2 image_size;
			auto img = entity_ptr->get_image_by_id(this->base_image_id);

			image_size.x = img->get_width();
			image_size.y = img->get_height();
			int32_t texture_id = img->get_opengl_texture_id();

			xml_ent_shader.new_shadow_draw_data(image_size, texture_id);
		}

		const int32_t xml_entity_layer::get_image_id(void) const
		{
			return this->base_image_id;
		}

		const int32_t xml_entity_layer::get_mask_id(void) const
		{
			return this->mask_image_id;
		}

		const bool xml_entity_layer::has_mask(void) const
		{
			return this->mask_image_id != -1;
		}

	};
};

