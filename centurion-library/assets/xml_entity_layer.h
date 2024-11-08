/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "xml_entity_structs.h"
#include <list>
#include <glm.hpp>

namespace tinyxml2
{
	class XMLElement;
};

namespace centurion
{
	namespace assets
	{
		class xml_entity;
		class xml_entity_layer;
		class xml_entity_shadow;
		class xml_entity_shader;

		/*
			struct for <layer> xml tags.
			very important because it contains the image id and the drawmode
		*/
		class xml_entity_layer
		{
		public:
			xml_entity_layer(tinyxml2::XMLElement* xml_layer);
			virtual void render(const xml_entity* entity_ptr) const;
			const int32_t get_image_id(void) const;
			const int32_t get_mask_id(void) const;
			const bool has_mask(void) const;
			~xml_entity_layer() {}

		protected:
			xml_entity_layer(tinyxml2::XMLElement* xml_layer, const uint32_t child);
			uint32_t id = 0;
			int32_t x = 0, y = 0, z = 0;
			int32_t base_image_id = 0, mask_image_id = -1;
			bool valid;
			xml_entity_shader& xml_ent_shader;
		};

		class xml_entity_shadow : public xml_entity_layer
		{
		public:
			xml_entity_shadow(tinyxml2::XMLElement* xml_shadow);
			void render(const xml_entity* entity_ptr) const override;
		protected:
			bool missing = false;
		};
	};
};
