#include "xml_class.h"
#include "xml_class_command.h"
#include "xml_class_method.h"
#include "xml_value.h"
#include "xml_values.h"
#include "xml_script.h"
#include "xml_entity.h"
#include "xml_scripts.h"

#include <algorithm>
#include <iostream>

namespace centurion
{
	namespace assets
	{
		using tinyxml2::TryParseStrAttribute;

		std::unordered_map<std::string, xml_class_type> xml_class::class_type_map {
			{"cpp_abstractclass", xml_class_type::e_abstractClass},
			{"cpp_noclass", xml_class_type::e_noClass},
			{"cpp_buildingclass", xml_class_type::e_buildingClass},
			{"cpp_unitclass", xml_class_type::e_unitClass},
			{"cpp_heroclass", xml_class_type::e_heroClass},
			{"cpp_wagonclass", xml_class_type::e_wagonClass},
			{"cpp_decorationclass", xml_class_type::e_decorationClass},
			{"cpp_vfxclass", xml_class_type::e_vfxClass},
			{"cpp_settlementclass", xml_class_type::e_settlementClass}
		};
		
		/*
			public functions
		*/
		std::shared_ptr<xml_class> xml_class::create(tinyxml2::XMLElement* xml_element, std::weak_ptr<centurion::assets::xml_values> xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts> xml_scripts_ptr)
		{
			assert(xml_element != nullptr);
			return std::shared_ptr<xml_class>{new xml_class{ xml_element, std::move(xml_values_ptr), std::move(xml_scripts_ptr) } };
		}

		const std::string& xml_class::get_class_name_cref(void) const noexcept
		{
			return this->class_name;
		}

		const std::string& xml_class::get_parent_class_name_cref(void) const noexcept
		{
			return this->parent_class_name;
		}

		bool xml_class::has_method(const std::string& method_id) const noexcept
		{
			return this->repeated_methods_map.contains(method_id) || this->unrepeated_methods_map.contains(method_id);
		}

		const std::unordered_map<std::string, std::shared_ptr<xml_class_method>>& xml_class::get_repeated_methods_map_cref(void) const noexcept
		{
			return this->repeated_methods_map;
		}

		const std::unordered_map<std::string, std::shared_ptr<xml_class_method>>& xml_class::get_unrepeated_methods_map_cref(void) const noexcept
		{
			return this->unrepeated_methods_map;
		}

		const std::unordered_set<std::string>& xml_class::get_parent_classes_set_cref(void) const noexcept
		{
			return this->parent_classes_set;
		}

		const std::vector<xmlCmdSP_t>& xml_class::get_commands_list_cref(void) const noexcept
		{
			return this->commands_ordered_by_priority;
		}

		const defaultClassCommandsList_t& xml_class::get_default_commands_list_cref(void) const noexcept
		{
			return this->default_commands_list;
		}

		const classValuesList_t& xml_class::get_values_vec_cref(void) const noexcept
		{
			return this->values_list;
		}

		xml_class_type xml_class::get_class_type(void) const noexcept
		{
			return this->class_type;
		}

		const attributesMap_t& xml_class::get_attributes(void) const noexcept
		{
			return this->attributes_map;
		}

		std::optional<std::string> xml_class::get_attribute(const std::string& attribute) const noexcept
		{
			return (this->attributes_map.contains(attribute) == true) ? this->attributes_map.at(attribute) : std::optional<std::string>();
		}

		std::shared_ptr<xml_entity> xml_class::get_entity(void) const noexcept
		{
			return this->entity;
		}

		/*
			private constructor
		*/
		xml_class::xml_class(tinyxml2::XMLElement* xml_element, std::weak_ptr<centurion::assets::xml_values>&& xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts>&& xml_scripts_ptr) :
			xml_values_ptr(std::move(xml_values_ptr)),
			xml_scripts_ptr(std::move(xml_scripts_ptr))
		{
			this->read_basic_info(xml_element);
			this->read_attributes(xml_element->FirstChildElement("attributes"));
			this->read_sounds(xml_element->FirstChildElement("sounds"));

			if (this->class_type != xml_class_type::e_decorationClass && this->class_type != xml_class_type::e_settlementClass)
			{
				this->read_methods(xml_element->FirstChildElement("methods"));
				this->read_commands(xml_element->FirstChildElement("commands"));
				this->read_commands_to_exclude(xml_element->FirstChildElement("excludeInheritedCommands"));
				this->read_default_commands(xml_element->FirstChildElement("defaultCommands"));

				this->read_values(xml_element->FirstChildElement("values"));
			}
			
			// read entity
			if (this->attributes_map.contains("ent_path"))
				this->entity = xml_entity::create(this->attributes_map.at("ent_path"));
			
			//todo editor tree 
		}

		/*
			destructor
		*/
		xml_class::~xml_class(void)
		{
			//todo ?
		}


		#pragma region Stati public members:
		std::optional<int> xml_class::try_parse_integer(const attributesMap_t& map, const std::string& name)
		{
			std::optional<int> result;
			if (map.contains(name) == true)
			{
				std::string value = map.at(name);
				try
				{
					const int value_int = std::stoi(value);
					result = value_int;
				}
				catch (...)
				{
					// TODO - logger
					// Logger::LogMessage msg = Logger::LogMessage("The value of variable \"" + name + "\" is not an integer", "Warn", "", "ClassesData", __FUNCTION__);
					// Logger::Warn(msg);
				}
			}
			return result;
		}

		std::optional<float> xml_class::try_parse_float(const attributesMap_t& map, const std::string& name)
		{
			std::optional<float> result;
			if (map.contains(name) == true)
			{
				std::string value = map.at(name);
				try
				{
					const float value_float = std::stof(value);
					result = value_float;
				}
				catch (...)
				{
					// TODO - logger
					// Logger::LogMessage msg = Logger::LogMessage("The value of variable \"" + name + "\" is not a float", "Warn", "", "ClassesData", __FUNCTION__);
					// Logger::Warn(msg);
				}
			}
			return result;
		}

		std::optional<std::string> xml_class::try_parse_string(const attributesMap_t& map, const std::string& name)
		{
			return (map.contains(name) == true) ? map.at(name) : std::optional<std::string>();
		}

		std::optional<bool> xml_class::try_parse_bool(const attributesMap_t& map, const std::string& name)
		{
			if (map.contains(name) == false)
				return std::optional<bool>();
			else if (map.at(name) == "true")
				return true;
			else if (map.at(name) == "false")
				return false;
			else
				return std::optional<bool>();
		}
		#pragma endregion


		/*
			private functions
		*/
		void xml_class::read_basic_info(tinyxml2::XMLElement* xml_element)
		{
			this->class_name = TryParseStrAttribute(xml_element, "class_name");
			this->class_type_str = TryParseStrAttribute(xml_element, "type");
			assert(!this->class_name.empty() && !this->class_type_str.empty() && xml_class::class_type_map.contains(this->class_type_str));
			this->parent_class_name = TryParseStrAttribute(xml_element, "parent");

			Encode::ToLowercase(&this->class_name);
			Encode::ToLowercase(&this->class_type_str);
			Encode::ToLowercase(&this->parent_class_name);

			// set class_type from static map
			this->class_type = xml_class::class_type_map.at(this->class_type_str);
		}

		void xml_class::read_attributes(tinyxml2::XMLElement* xml_attributes)
		{
			if (xml_attributes == nullptr)
				return;
			
			for (auto xml_it = xml_attributes->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
			{
				this->add_attribute_if_missing(TryParseStrAttribute(xml_it, "name"), TryParseStrAttribute(xml_it, "value"));
			}
		}

		void xml_class::read_sounds(tinyxml2::XMLElement* xml_sounds)
		{
			if (xml_sounds != nullptr)
			{
				for (auto xml_it = xml_sounds->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
				{
					this->add_sound_if_missing(TryParseStrAttribute(xml_it, "name"), TryParseStrAttribute(xml_it, "path"));
				}
			}
		}

		void xml_class::read_methods(tinyxml2::XMLElement* xml_methods)
		{
			if (xml_methods != nullptr)
			{
				for (auto xml_it = xml_methods->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
				{
					this->add_method_if_missing(xml_it);
				}
			}
		}

		void xml_class::read_commands(tinyxml2::XMLElement* xml_commands)
		{
			if (xml_commands != nullptr)
			{
				uint16_t cmd_count = 1;
				for (auto xml_it = xml_commands->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
				{
					this->add_command_if_missing(xml_it, cmd_count);
					cmd_count++;
				}
			}
		}

		void xml_class::read_commands_to_exclude(tinyxml2::XMLElement* xml_commands_to_exclude)
		{
			if (xml_commands_to_exclude != nullptr)
			{
				for (auto xml_it = xml_commands_to_exclude->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
				{
					this->add_command_to_exclude_if_missing(TryParseStrAttribute(xml_it, "id"), false);
				}
			}
		}

		void xml_class::read_default_commands(tinyxml2::XMLElement* xml_default_commands)
		{
			if (xml_default_commands != nullptr)
			{
				for (auto xml_it = xml_default_commands->FirstChildElement(); xml_it != nullptr; xml_it = xml_it->NextSiblingElement())
				{
					this->add_default_command_if_missing(xml_it);
				}
			}
		}

		void xml_class::read_values(tinyxml2::XMLElement* xml_values)
		{
			if (xml_values == nullptr)
				return;

			for (tinyxml2::XMLElement* _it_vl = xml_values->FirstChildElement(); _it_vl != nullptr; _it_vl = _it_vl->NextSiblingElement())
			{
				std::string value_id = tinyxml2::TryParseStrAttribute(_it_vl, "name");
				assert(value_id.empty() == false);
				auto valueSP = this->xml_values_ptr->get_value_ptr(value_id);
				// ASSERTION: value inside an xml class must be previously read in values.xml
				assert(valueSP);
				this->add_value_if_missing(value_id, std::move(valueSP));
			}
		}

		void xml_class::exclude_commands(void)
		{
			this->check_commands_validity();

			std::unordered_set<std::string>::iterator it = this->commands_to_exclude_set.begin();
			std::unordered_set<std::string>::const_iterator end = this->commands_to_exclude_set.cend();
			while (it != end)
			{
				const std::string s = (*it);
				this->commands_map.erase(s);
				this->commands_to_exclude_set.erase(s);
				it++;  // go to next command to exclude
			}
		}

		void xml_class::check_commands_validity(void)
		{
			std::list<std::string> unvalid_cmds;
			for (auto const& [cmdID, cmd] : this->commands_map)
			{
				assert(cmd);
				const std::string& methodAssociatedToCmd = cmd->GetMethodCRef();
				bool bValidCmd = false;
				const bool bIsRepeatedMethod = this->repeated_methods_map.contains(methodAssociatedToCmd);
				const bool bIsUnrepeatedMethod = this->unrepeated_methods_map.contains(methodAssociatedToCmd);
				// ASSERTION: method must not be present in both maps
				assert((bIsRepeatedMethod && bIsUnrepeatedMethod) == false);
				if (cmdID.empty() == false && methodAssociatedToCmd.empty() == false && (bIsRepeatedMethod == true || bIsUnrepeatedMethod == true))
				{
					auto& method = (bIsUnrepeatedMethod == true) ? this->unrepeated_methods_map.at(methodAssociatedToCmd) : this->repeated_methods_map.at(methodAssociatedToCmd);
					auto scriptSP = method->get_associated_script();
					assert(scriptSP);
					if (scriptSP->is_autoexecutable() == false && scriptSP->is_associable_to_cmd() == true)
					{
						// Check the compatibility between params
						auto& requiredParamsSet = scriptSP->get_required_cmd_params_cref();
						auto& providedParamsList = cmd->GetParamsListCRef();
						bValidCmd = (providedParamsList == requiredParamsSet);
					}
				}

				if (bValidCmd == false)
				{
					// #if CENTURION_DEBUG_MODE
					std::cout << "[DEBUG] class " << this->class_name << "; command " << cmdID << " is invalid (see " << __FUNCTION__ << ")" << std::endl;
					// #endif // CENTURION_DEBUG_MODE
					unvalid_cmds.push_back(cmdID);
				}
			}

			// Discard invalid commands
			while (unvalid_cmds.empty() == false)
			{
				const std::string& cmd_id = unvalid_cmds.front();
				this->commands_map.erase(cmd_id);
				this->commands_to_exclude_set.erase(cmd_id);
				unvalid_cmds.pop_front();
			}
		}

		void xml_class::set_final_commands_priority(void)
		{
			if (this->commands_map.empty())
				return;

			// filling the list:
			for (auto& cmd : this->commands_map)
				this->commands_ordered_by_priority.push_back(cmd.second);
			
			// ordering commands by priority:
			std::sort(
				this->commands_ordered_by_priority.begin(), 
				this->commands_ordered_by_priority.end(), 
				[](const xmlCmdSP_t& l, const xmlCmdSP_t& r) {
					return (*l) < (*r);  // In order to compare the pointed objects and not the pointers
				}
			);
			
			// if adiacent elements have the same priority, the priority 
			// of the second will be incremented in order to don't have commands 
			// with the same priority.
			const size_t n = this->commands_ordered_by_priority.size();
			for (size_t i = 0; i < (n - 1); i++)
			{
				while (this->commands_ordered_by_priority[i + 1]->GetBottombarPriority() <= this->commands_ordered_by_priority[i]->GetBottombarPriority())
				{
					this->commands_ordered_by_priority[i + 1]->IncrementBarPriority(1);
				}
			}
		}

		/*
			inject from parent methods
		*/
		void xml_class::inject_parent_class_data(const std::string& parent_class_name, const std::shared_ptr<xml_class>& parent_class_ptr, const uint16_t recursive_call)
		{
			assert(!parent_class_name.empty() && parent_class_ptr != nullptr);
			
			this->add_parent_class(parent_class_name);

			this->inject_attributes(parent_class_ptr);
			this->inject_sounds(parent_class_ptr);
			if (this->class_type != xml_class_type::e_decorationClass && this->class_type != xml_class_type::e_settlementClass)
			{
				this->inject_methods(parent_class_ptr);
				this->inject_commands(parent_class_ptr, recursive_call);
				this->inject_commands_to_exclude(parent_class_ptr);
				this->inject_default_commands(parent_class_ptr);
			}
		}

		void xml_class::inject_attributes(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			const auto& parent_attrs = parent_class_ptr->attributes_map;
			for (auto const& [name, value] : parent_attrs)
				this->add_attribute_if_missing(name, value);
		}

		void xml_class::inject_sounds(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			const auto& parent_sounds = parent_class_ptr->sound_map;
			for (auto const& [name, path] : parent_sounds)
				this->add_sound_if_missing(name, path);
		}

		void xml_class::inject_methods(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			const auto& u_methods_map = parent_class_ptr->unrepeated_methods_map;
			for (auto& [name, method] : u_methods_map)
				this->add_method_if_missing(method);
			
			const auto& methods_map = parent_class_ptr->repeated_methods_map;
			for (auto& [name, method] : methods_map)
				this->add_method_if_missing(method);
		}

		void xml_class::inject_commands(const std::shared_ptr<xml_class>& parent_class_ptr, const uint16_t recursive_call)
		{
			const auto& parent_commands = parent_class_ptr->commands_map;
			for (auto& [command_id, command] : parent_commands)
			{
				this->add_parent_command(command_id, command, recursive_call);
			}
		}

		void xml_class::inject_commands_to_exclude(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			const auto& parent_cmd_to_exclude = parent_class_ptr->commands_to_exclude_set;
			for (auto& command_id : parent_cmd_to_exclude)
				this->add_command_to_exclude_if_missing(command_id, true);
		}

		void xml_class::inject_default_commands(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			const auto& parent_default_commands = parent_class_ptr->default_commands_list;
			for (auto default_command : parent_default_commands)
				this->add_default_command_if_missing(default_command);
		}

		void xml_class::inject_values(const std::shared_ptr<xml_class>& parent_class_ptr)
		{
			for (auto const& _value : parent_class_ptr->values_list)
				this->add_value_if_missing(_value->get_name_cref(), _value);
		}

		/*
			add functions
		*/
		void xml_class::add_attribute_if_missing(std::string name, std::string value)
		{
			if (name.empty() == false && value.empty() == false && !this->attributes_map.contains(name))
			{
				this->attributes_map.insert({ std::move(name), std::move(value) });
			}
		}

		void xml_class::add_sound_if_missing(std::string name, std::string value)
		{
			if (name.empty() == false && value.empty() == false && !this->sound_map.contains(name))
			{
				this->sound_map.insert({ std::move(name), std::move(value) });
			}
		}

		void xml_class::add_value_if_missing(const std::string& valueID, std::shared_ptr<const xml_value> value)
		{
			const size_t nValues = this->values_list.size();
			if (nValues >= xml_values::MAX_NUMBER_OF_VALUES_PER_CLASS)
			{
				// This class cannot have additional values
				// TODO aggiungere log @lezzo
				return; 
			}

			for (size_t i = 0; i < nValues; i++)
			{
				if (this->values_list.at(i)->get_name_cref() == valueID)
					return;  // This class has already the value
			}

			// This class doesn't have the value, so add it
			this->values_list.push_back(std::move(value));
		}

		void xml_class::add_method_if_missing(tinyxml2::XMLElement* xml_method)
		{
			if (xml_method == nullptr) 
				return;

			std::shared_ptr<xml_class_method> new_method{ new xml_class_method{ xml_method, this->xml_scripts_ptr } };
			if (new_method)
			{
				this->add_method_if_missing(new_method);
			}
		}

		void xml_class::add_method_if_missing(std::shared_ptr<xml_class_method> method_ptr)
		{
			assert(method_ptr);
			std::string method_id = method_ptr->get_name_cref();
			assert(method_id.empty() == false);
			if (method_ptr->is_valid() && this->has_method(method_id) == false)
			{
				const bool b_repeated_method = method_ptr->get_associated_script()->is_autoexecutable();
				if (b_repeated_method)
					this->repeated_methods_map.insert({ std::move(method_id), std::move(method_ptr) });
				else
					this->unrepeated_methods_map.insert({ std::move(method_id), std::move(method_ptr) });
			}
		}

		void xml_class::add_command_if_missing(tinyxml2::XMLElement* xml_command, const uint16_t cmd_count)
		{
			if (xml_command == nullptr) 
				return;

			xmlCmdSP_t new_command{ new xml_class_command{ xml_command, cmd_count } };
			if (new_command && !this->commands_map.contains(new_command->GetIdCRef()))
			{
				std::string command_id = new_command->GetIdCRef();
				this->commands_map.insert({ std::move(command_id), std::move(new_command) });
			}
		}

		void xml_class::add_parent_command(std::string command_id, const xmlCmdSP_t& command, const uint16_t recursive_call)
		{
			if (this->commands_map.contains(command_id))
				return;

			xmlCmdSP_t new_command{ new xml_class_command{*command} };
			new_command->SetInheritedCmdLevel(recursive_call);
			this->commands_map.insert({ std::move(command_id), std::move(new_command) });
		}

		void xml_class::add_command_to_exclude_if_missing(const std::string& command_id, const bool is_parent_command)
		{
			bool already_present = this->commands_to_exclude_set.contains(command_id);

			//A parent class can't exclude any commands to its child class.
			assert((is_parent_command && already_present) == false);

			//A class can't exclude its own commands: it's silly
			assert((!is_parent_command && already_present) == false);

			if (!already_present)
			{
				this->commands_to_exclude_set.insert(command_id);
			}
		}

		void xml_class::add_default_command_if_missing(tinyxml2::XMLElement* xml_default_command)
		{
			if (xml_default_command == nullptr) 
				return;

			std::shared_ptr<xml_class_default_command> new_default_command{ new xml_class_default_command{ xml_default_command } };
			this->add_default_command_if_missing(new_default_command);
		}

		void xml_class::add_default_command_if_missing(std::shared_ptr<xml_class_default_command>& new_default_command)
		{
			if (new_default_command && !new_default_command->GetTargetCRef().empty())
			{
				auto target = new_default_command->GetTargetCRef();
				for (auto& d : this->default_commands_list)
				{
					if (d->GetTargetCRef() == target)
						return;
				}
				this->default_commands_list.push_back(new_default_command);
			}
		}

		void xml_class::add_parent_class(const std::string& parent_class_name)
		{
			assert(!this->parent_classes_set.contains(parent_class_name));
			this->parent_classes_set.insert(parent_class_name);
		}
	};
};
