/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

/*
	Please include this header only in .cpp files!
*/

#ifndef CMD_TYPE_1 
#define CMD_TYPE_1   "training"
#endif

#ifndef CMD_TYPE_2 
#define CMD_TYPE_2   "technology"
#endif

#include "xml_class_enums.h"

#include <encode_utils.h>
#include <tinyxml2_utils.h>

#include <cassert>
#include <cstdint>
#include <list>
#include <string>
#include <unordered_set>
#include <vector>
#include <memory>

namespace centurion
{
	namespace assets
	{
		struct xml_class_command_params
		{
			xml_class_command_params(std::string&& _name, std::string&& _type, std::string&& _value) :
				name(std::move(_name)), type(std::move(_type)), value(std::move(_value))
			{
			}

			xml_class_command_params(xml_class_command_params&& other) noexcept :
				name(std::move(other.name)), type(std::move(other.type)), value(std::move(other.value))
			{
			}

			xml_class_command_params& operator=(xml_class_command_params&& other) noexcept
			{
				if (this != &other)
				{
					this->name = std::move(other.name);
					this->type = std::move(other.type);
					this->value = std::move(other.value);
				}
				return (*this);
			}
			
			[[nodiscard]] inline bool operator==(const xml_class_command_params& other) const noexcept
			{
				return (this->name == other.name);
			}

			struct cmd_param_hash_function
			{
				[[nodiscard]] inline size_t operator()(const xml_class_command_params& cmdParam) const noexcept
				{
					return ((std::hash<std::string>()(cmdParam.name) ^ (std::hash<std::string>()(cmdParam.type) << 1)) >> 1);
				}
			};

			std::string name;
			std::string value;
			std::string type;

			/// <summary>
			/// This operator== checks that a list of provided commands satisfies a set of required commands.
			/// In other words, in order to get true, a list should provide all the pair name, type required by the set.
			/// </summary>
			friend inline bool operator==(const std::list<xml_class_command_params>& list, const std::unordered_set<xml_class_command_params, cmd_param_hash_function>& set)
			{
				if (list.size() == set.size())
				{
					for (auto const& it_provided : list)
					{
						if (set.contains(it_provided) == false)
							return false;
					}
					return true;
				}
				return false;
			}
		};

		typedef xml_class_command_params::cmd_param_hash_function commandParamsHasher_t;
		typedef std::unordered_set<xml_class_command_params, commandParamsHasher_t> requiredCmdParams_t;
		
		class xml_class_default_command
		{
		public:
			explicit xml_class_default_command(tinyxml2::XMLElement* el);

			xml_class_default_command& operator=(const xml_class_default_command& other) = delete;

			[[nodiscard]] const std::string& GetTargetCRef(void) const noexcept;
			[[nodiscard]] const std::list<std::string> GetCommandListCRef(void) const noexcept;
			[[nodiscard]] std::list<std::string> const* GetCommandListCPtr(void) const noexcept;
		private:
			std::list<std::string> commands;
			std::string target;
		};

		class xml_class_command
		{
		private:
			struct cmd_s
			{
				std::string id;
				std::string displayedName;
				std::string description;
				std::string method;
				std::string hotKey;
				std::string bottomBarIcon;
				std::string topBarIcon;
				std::string iframeToOpen;
				xml_command_type type{ xml_command_type::Other };
				uint8_t staminaCost = 0;
				uint16_t bottombarPriority = 0;
				uint32_t goldCost = 0;
				uint32_t foodCost = 0;
				uint32_t populationCost = 0;
				uint32_t delay = 0;
				std::list<xml_class_command_params> params;
				uint16_t readOrder = 0;
			};
			std::shared_ptr<cmd_s> cmd;
			uint16_t inheritedCmdLevel = 0;
		public:
			xml_class_command(tinyxml2::XMLElement* el, const uint16_t _readOrder, const bool opensIframe = false);
			[[nodiscard]] bool operator<(const xml_class_command& other) const noexcept;
			[[nodiscard]] const std::string& GetIdCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetDisplayedNameCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetDescriptionCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetMethodCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetHotKeyCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetBottombarIconCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetTopbarIconCRef(void) const noexcept;
			[[nodiscard]] const std::string& GetIframeToOpen(void) const noexcept;
			[[nodiscard]] xml_command_type GetType(void) const noexcept;
			[[nodiscard]] uint8_t GetStaminaCost(void) const noexcept;
			[[nodiscard]] const uint16_t GetBottombarPriority(void) const noexcept;
			[[nodiscard]] const uint16_t GetInheritedLevel(void) const noexcept;
			[[nodiscard]] const uint16_t GetXMLReadOrder(void) const noexcept;
			[[nodiscard]] uint32_t GetGoldCost(void) const noexcept;
			[[nodiscard]] uint32_t GetFoodCost(void) const noexcept;
			[[nodiscard]] uint32_t GetPopulationCost(void) const noexcept;
			[[nodiscard]] uint32_t GetDelay(void) const noexcept;
			[[nodiscard]] const providedCmdParams_t& GetParamsListCRef(void) const noexcept;
			void IncrementBarPriority(const uint32_t i);
			void SetInheritedCmdLevel(const uint16_t _inheritedCmdLevel);
		};
	};
};
