#include "xml_entity.h"
#include "xml_entity_image.h"
#include "xml_entity_layer.h"
#include "xml_entity_shader.h"

// services
#include <zipservice.h>
#include <fileservice.h>

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
		std::unordered_map<std::string, int> xml_entity::static_points_map{
			{ "fire", 0 },
			{ "exit", 1 }
		};

		using tinyxml2::TryParseStrAttribute;
		using tinyxml2::TryParseIntAttribute;
		using tinyxml2::TryParseFloatAttribute;
		using tinyxml2::TryParseBoolAttribute;

		xmlEntitySP_t xml_entity::create(const std::string& ent_path)
		{
			return xmlEntitySP_t{ new xml_entity{ent_path} };
		}

		void xml_entity::load_textures()
		{
			for (auto& [image_id, image_ptr] : images_map)
			{
				image_ptr->read_image_from_path();
			}
		}

		void xml_entity::load_only_idle_textures(const std::string& state_name)
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			assert(animations_map.contains("idle"));
			auto& idle_anim = animations_map.at("idle");
			for (auto& layer : idle_anim.layers)
			{
				auto base_image_id = layer->get_image_id();
				assert(this->images_map.contains(base_image_id));
				this->images_map.at(base_image_id)->read_image_from_path();

				if (layer->has_mask())
				{
					auto mask_image_id = layer->get_mask_id();
					assert(this->images_map.contains(mask_image_id));
					this->images_map.at(mask_image_id)->read_image_from_path();
				}
			}

			if (idle_anim.has_shadow() == true)
			{
				const auto img_id = idle_anim.shadow->get_image_id();
				assert(this->images_map.contains(img_id));
				this->images_map.at(img_id)->read_image_from_path();
			}
		}

		void xml_entity::render(const std::string& state_name, const std::string& anim_name, const uint32_t x_pos, const uint32_t y_pos) const
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			assert(animations_map.contains(anim_name));
			auto& current_anim = animations_map.at(anim_name);
			
			auto& entity_shader = xml_entity_shader::get_instance();
			entity_shader.new_entity_draw_data(x_pos + origin_offset_x, y_pos + origin_offset_y, current_anim.get_number_of_frames(), this->directions);
			
			// render shadow
			if (entity_shader.picking == false && current_anim.has_shadow() == true)
			{
				current_anim.shadow->render(this);
				entity_shader.push_current_draw_data();
			}

			// render other layers
			for (const auto& layer : current_anim.layers)
			{
				layer->render(this);
				if (entity_shader.picking == false)
				{
					entity_shader.push_current_draw_data();
				}
				else
				{
					entity_shader.render_current_draw_data();
				}
			}
		}

		std::pair<uint32_t, uint32_t> xml_entity::get_entity_size(void) const noexcept
		{
			return { hitbox_width, hitbox_height };
		}

		std::pair<int32_t, int32_t> xml_entity::get_origin_offsets(void) const noexcept
		{
			return { origin_offset_x, origin_offset_y };
		}

		size_t xml_entity::get_anim_number_of_frames(const std::string& state_name, const std::string& anim_name) const noexcept
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			assert(animations_map.contains(anim_name));
			return animations_map.at(anim_name).get_number_of_frames();
		}

		uint32_t xml_entity::get_anim_total_duration(const std::string& state_name, const std::string& anim_name) const noexcept
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			assert(animations_map.contains(anim_name));
			return animations_map.at(anim_name).get_total_frames_duration();
		}

		uint32_t xml_entity::get_anim_frame_duration(const std::string& state_name, const std::string& anim_name) const noexcept
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			assert(animations_map.contains(anim_name));
			return animations_map.at(anim_name).get_single_frame_duration();
		}

		bool xml_entity::check_if_animation_exists(const std::string& state_name, const std::string& anim_name) const noexcept
		{
			assert(states_map.contains(state_name));
			auto& animations_map = this->states_map.at(state_name).animations;
			return animations_map.contains(anim_name);
		}

		uint16_t xml_entity::get_number_of_directions(void) const noexcept
		{
			return static_cast<uint16_t>(this->directions);
		}

		const entityPointsVec_t& xml_entity::get_points_by_type_cref(const std::string& type) const
		{
			if (xml_entity::static_points_map.contains(type) == false)
				throw std::runtime_error("missing point");  // TODO exception - sostituire con eccezione apposita
			if (this->points_map.contains(xml_entity::static_points_map.at(type)) == false)
				throw std::runtime_error("missing point for this");  // TODO exception - sostituire con eccezione apposita (diversa da precendete ma ereditante da una stessa classe)

			return this->points_map.at(xml_entity::static_points_map.at(type));
		}
		const std::shared_ptr<xml_entity_image> xml_entity::get_image_by_id(const uint32_t id) const
		{
			if (this->images_map.contains(id) == false)
				throw std::runtime_error("missing image");  // TODO exception - sostituire con eccezione apposita

			return this->images_map.at(id);
		}


		xml_entity::xml_entity(std::string ent_path) :
			ent_file_path(std::move(ent_path)),
			zip_s(rattlesmake::services::zip_service::get_instance()),
			file_s(rattlesmake::services::file_service::get_instance()),
			ent_folder_path(file_s.get_file_folder_path(ent_file_path))
		{
			auto xml_text = zip_s.get_text_file("?entities.zip", ent_file_path);

			// read xml
			tinyxml2::XMLDocument xml;
			auto xml_error = xml.Parse(xml_text.c_str());
			auto xml_element_entity = xml.FirstChildElement("entity");
			assert(xml_element_entity != nullptr);

			// parse information
			read_basic_info(xml_element_entity);
			read_hitbox(xml_element_entity->FirstChildElement("hitbox"));
			read_properties(xml_element_entity->FirstChildElement("properties"));
			read_points(xml_element_entity->FirstChildElement("points"));
			read_images(xml_element_entity->FirstChildElement("images"));
			read_states(xml_element_entity->FirstChildElement("states"));
		}
		xml_entity::~xml_entity()
		{
		}

		/*
			parse from xml functions
		*/
		void xml_entity::read_basic_info(tinyxml2::XMLElement* xml_element)
		{
			if (xml_element == nullptr)
				return;

			class_name = TryParseStrAttribute(xml_element, "class_name");
			assert(!class_name.empty());

			type = TryParseStrAttribute(xml_element, "type");
			assert(!type.empty());

			zip_file = TryParseStrAttribute(xml_element, "zip_file");
			assert(!zip_file.empty());
			zip_file = "?" + zip_file; // the "?" is important to match the zip key (see engine::initialize -> zip_service initialization)

			int _directions = TryParseIntAttribute(xml_element, "directions", -1);
			assert(directions != -1);
			this->directions = (uint32_t)_directions;

			origin_offset_x = TryParseIntAttribute(xml_element, "origin_offset_x", 0);
			origin_offset_y = TryParseIntAttribute(xml_element, "origin_offset_y", 0);
		}
		void xml_entity::read_hitbox(tinyxml2::XMLElement* xml_hitbox)
		{
			if (xml_hitbox == nullptr)
				return;

			hitbox_width = TryParseIntAttribute(xml_hitbox, "width", 0);
			hitbox_height = TryParseIntAttribute(xml_hitbox, "height", 0);
		}
		void xml_entity::read_properties(tinyxml2::XMLElement* xml_properties)
		{
			if (xml_properties == nullptr)
				return;

			for (tinyxml2::XMLElement* xml_it = xml_properties->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
			{
				properties_vec.push_back({ TryParseStrAttribute(xml_it, "name"), TryParseStrAttribute(xml_it, "value") });
			}
		}
		void xml_entity::read_points(tinyxml2::XMLElement* xml_points)
		{
			if (xml_points == nullptr)
				return;

			for (tinyxml2::XMLElement* p = xml_points->FirstChildElement(); p != nullptr; p = p->NextSiblingElement())
			{
				auto type = TryParseStrAttribute(p, "type");
				assert(!type.empty() && xml_entity::static_points_map.contains(type));
				this->points_map[xml_entity::static_points_map.at(type)].push_back({ TryParseIntAttribute(p, "id"), TryParseIntAttribute(p, "x"), TryParseIntAttribute(p, "y"), xml_entity::static_points_map.at(type) });
			}
		}
		void xml_entity::read_images(tinyxml2::XMLElement* xml_images)
		{
			if (xml_images == nullptr)
				return;

			bool load_texture = false;

			for (tinyxml2::XMLElement* i = xml_images->FirstChildElement(); i != nullptr; i = i->NextSiblingElement())
			{
				int id = TryParseIntAttribute(i, "id", -1);
				assert(id != -1 && !images_map.contains(id));
				images_map.insert(
					{ (uint32_t)id,
					xml_entity_image::create(load_texture, id, TryParseBoolAttribute(i, "reverse", false), zip_file, TryParseStrAttribute(i, "file")) }
				);
			}
		}
		void xml_entity::read_states(tinyxml2::XMLElement* xml_states)
		{
			if (xml_states == nullptr)
				return;

			for (tinyxml2::XMLElement* s = xml_states->FirstChildElement(); s != nullptr; s = s->NextSiblingElement())
			{
				std::string state_name = TryParseStrAttribute(s, "name");
				Encode::ToLowercase(&state_name);

				xml_entity_state state{ state_name };
				assert(state.valid && !states_map.contains(state.name));

				auto xml_animations = s->FirstChildElement("animations");
				read_animations(xml_animations, state);

				this->states_map.insert({ state.name, state });
			}
		}
		void xml_entity::read_animations(tinyxml2::XMLElement* xml_animations, xml_entity_state& state)
		{
			if (xml_animations == nullptr)
				return;

			for (tinyxml2::XMLElement* a = xml_animations->FirstChildElement(); a != nullptr; a = a->NextSiblingElement())
			{
				std::string anim_name = TryParseStrAttribute(a, "name");
				Encode::ToLowercase(&anim_name);

				xml_entity_animation anim{ anim_name, TryParseIntAttribute(a, "duration", 1), TryParseIntAttribute(a, "frames", -1), TryParseFloatAttribute(a, "speed_multiplier", 1.f)};
				assert(anim.valid && !state.animations.contains(anim.name));

				// layers and shadow
				auto xml_layers = a->FirstChildElement("layers");
				assert(xml_layers != nullptr);
				auto xml_shadow = a->FirstChildElement("shadow");
				read_animation_layers_and_shadow(xml_layers, xml_shadow, anim);

				state.animations.insert({ anim.name, anim });
			}
		}
		void xml_entity::read_animation_layers_and_shadow(tinyxml2::XMLElement* xml_layers, tinyxml2::XMLElement* xml_shadow, xml_entity_animation& anim)
		{
			for (auto l = xml_layers->FirstChildElement(); l != nullptr; l = l->NextSiblingElement())
			{
				std::shared_ptr<xml_entity_layer> layer{ new xml_entity_layer(l) };
				anim.layers.push_back(layer);
			}
			if (xml_shadow != nullptr)
			{
				anim.shadow = std::shared_ptr<xml_entity_shadow>(new xml_entity_shadow(xml_shadow));
			}
		}
	};
};
