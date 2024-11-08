/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <memory>

namespace centurion
{
	namespace assets
	{
		/*
			struct for <point> xml tags
		*/
		struct xml_entity_point
		{
			inline xml_entity_point(const int id, const int x, const int y, const int type) :
				x(x),
				y(y),
				id((uint32_t)id),
				type((uint32_t)type)
			{}
			int32_t x, y;
			uint32_t id, type;
		};

		/*
			struct for <property> xml tags
		*/
		struct xml_entity_property
		{
			xml_entity_property(std::string name, std::string value) :
				name(std::move(name)),
				value(std::move(value))
			{
			}
			std::string name, value;
		};

		/*
			struct for <layer> <texture /> </layer>
			it could be shadow or normal/color/level/skin
		*/

		struct xml_entity_layer_texture
		{
			//xml_entity_layer_texture() {}
			xml_entity_layer_texture(const int image) :
				image((uint32_t)image),
				valid(image != -1)
			{
			}
			uint32_t image = 0;
			const bool valid;
		};

		/*
			struct for <animation> xml tags
			it contains layers and frames
		*/
		class xml_entity_layer;
		class xml_entity_shadow;
		struct xml_entity_animation
		{
			xml_entity_animation(std::string _name, const int duration, const int frames, const float _speed_multiplier) :
				name(std::move(_name)),
				speed_multiplier(_speed_multiplier),
				total_frames((uint32_t)frames),
				frame_duration((uint32_t)duration),
				valid(!this->name.empty() && frames > 0)
			{
			}

			/*
				functions
			*/
			[[nodiscard]] inline uint32_t get_number_of_frames(void) const noexcept
			{
				return this->total_frames;
			}

			[[nodiscard]] inline uint32_t get_single_frame_duration(void) const noexcept
			{
				return this->frame_duration;
			}

			[[nodiscard]] inline uint32_t get_total_frames_duration(void) const noexcept
			{
				return this->frame_duration * this->total_frames;
			}

			[[nodiscard]] inline const bool has_shadow(void) const noexcept
			{
				return this->shadow != nullptr;
			}

			/*
				members
			*/
			std::string name;

			// total frames of the animation = the columns of the sprite
			uint32_t total_frames = 0;

			// frame duration = teorically the game runs 60fps 
			// => if frame_duration = 2 
			// => in one second we can render 30 potential columns of the sprite
			uint32_t frame_duration;

			float speed_multiplier = 1.f;
			std::vector<std::shared_ptr<xml_entity_layer>> layers;
			std::shared_ptr<xml_entity_shadow> shadow = nullptr;
			const bool valid;
		};

		/*
			struct for <state> xml tags
			it contains animations
		*/
		struct xml_entity_state
		{
			xml_entity_state(const std::string& name) : 
				name(name),
				valid(!name.empty())
			{}
			std::map<std::string, xml_entity_animation> animations;
			std::string name;
			const bool valid;
		};
	};
};
