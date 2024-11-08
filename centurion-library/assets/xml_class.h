/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "xml_class_enums.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


namespace tinyxml2 { class XMLElement; };


namespace centurion
{
	namespace assets
	{
		class xml_classes;
		class xml_class_command;
		class xml_class_method;
		class xml_class_default_command;
		class xml_values;
		class xml_value;
		class xml_script;
		class xml_entity;
		class xml_scripts;

		/*
			this class contains all the information
			included in class_name.oc.xml files
			in assets/data.zip/classes/
		*/
		class xml_class
		{
			friend class xml_classes;

		public:
			xml_class(const xml_class& other) = delete;

			xml_class& operator=(const xml_class& other) = delete;


			/*
				destructor
			*/
			~xml_class(void);


			[[nodiscard]] static std::optional<int> try_parse_integer(const attributesMap_t& map, const std::string& name);
			[[nodiscard]] static std::optional<float> try_parse_float(const attributesMap_t& map, const std::string& name);
			[[nodiscard]] static std::optional<std::string> try_parse_string(const attributesMap_t& map, const std::string& name);
			[[nodiscard]] static std::optional<bool> try_parse_bool(const attributesMap_t& map, const std::string& name);


			/*
				returns the class name ~ id
			*/
			[[nodiscard]] const std::string& get_class_name_cref(void) const noexcept;


			/*
				returns the parent class name
			*/
			[[nodiscard]] const std::string& get_parent_class_name_cref(void) const noexcept;


			/*
				check if a method is currently contained in class methods (unrepeated or repeated)
			*/
			[[nodiscard]] bool has_method(const std::string& method_id) const noexcept;

			[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<xml_class_method>>& get_repeated_methods_map_cref(void) const noexcept;

			[[nodiscard]] const std::unordered_map<std::string, std::shared_ptr<xml_class_method>>& get_unrepeated_methods_map_cref(void) const noexcept;

			[[nodiscard]] const std::unordered_set<std::string>& get_parent_classes_set_cref(void) const noexcept;

			[[nodiscard]] const std::vector<xmlCmdSP_t>& get_commands_list_cref(void) const noexcept;

			[[nodiscard]] const defaultClassCommandsList_t& get_default_commands_list_cref(void) const noexcept;

			[[nodiscard]] const classValuesList_t& get_values_vec_cref(void) const noexcept;


			/*
				returns the class type (enum format)
			*/
			[[nodiscard]] xml_class_type get_class_type(void) const noexcept;


			/*
				returns the xml class attributes
			*/
			[[nodiscard]] const attributesMap_t& get_attributes(void) const noexcept;


			/*
				returns the required attribute
			*/
			[[nodiscard]] std::optional<std::string> get_attribute(const std::string& attribute) const noexcept;


			/*
				returns the entity pointer
			*/
			[[nodiscard]] std::shared_ptr<xml_entity> get_entity(void) const noexcept;
		private:

			/*
				the right way to create a new xml_class object.
				pass as xml_element the <object> tag of the xml class file
			*/
			[[nodiscard]] static std::shared_ptr<xml_class> create(tinyxml2::XMLElement* xml_element, std::weak_ptr<centurion::assets::xml_values> xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts> xml_scripts_ptr);


			/*
				private constructor
			*/
			xml_class(tinyxml2::XMLElement* xml_element, std::weak_ptr<centurion::assets::xml_values>&& xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts>&& xml_scripts_ptr);


			/*
				private functions
			*/
			void read_basic_info(tinyxml2::XMLElement* xml_element);
			void read_attributes(tinyxml2::XMLElement* xml_attributes);
			void read_sounds(tinyxml2::XMLElement* xml_sounds);
			void read_methods(tinyxml2::XMLElement* xml_methods);
			void read_commands(tinyxml2::XMLElement* xml_commands);
			void read_commands_to_exclude(tinyxml2::XMLElement* xml_commands_to_exclude);
			void read_default_commands(tinyxml2::XMLElement* xml_default_commands);
			void read_values(tinyxml2::XMLElement* xml_values);
			

			/*
				parent classes members
			*/
			void add_parent_class(const std::string& parent_class_name);
			void inject_parent_class_data(const std::string& parent_class_name, const std::shared_ptr<xml_class>& parent_class_ptr, const uint16_t recursive_call);
			void inject_attributes(const std::shared_ptr<xml_class>& parent_class_ptr);
			void inject_sounds(const std::shared_ptr<xml_class>& parent_class_ptr);
			void inject_methods(const std::shared_ptr<xml_class>& parent_class_ptr);
			void inject_commands(const std::shared_ptr<xml_class>& parent_class_ptr, const uint16_t recursive_call);
			void inject_commands_to_exclude(const std::shared_ptr<xml_class>& parent_class_ptr);
			void inject_default_commands(const std::shared_ptr<xml_class>& parent_class_ptr);
			void inject_values(const std::shared_ptr<xml_class>& parent_class_ptr);
			std::unordered_set<std::string> parent_classes_set;
			std::string parent_class_name;  // one level above


			/*
				class methods
			*/
			void add_method_if_missing(tinyxml2::XMLElement* xml_method);
			void add_method_if_missing(std::shared_ptr<xml_class_method> method_ptr);
			std::unordered_map<std::string, std::shared_ptr<xml_class_method>> repeated_methods_map;  // Methods whose script is automatically executed.
			std::unordered_map<std::string, std::shared_ptr<xml_class_method>> unrepeated_methods_map;


			/*
				class commands
			*/
			void add_command_if_missing(tinyxml2::XMLElement* xml_command, const uint16_t cmd_count);
			void add_parent_command(std::string command_id, const xmlCmdSP_t& command, const uint16_t recursive_call);
			void add_command_to_exclude_if_missing(const std::string& command_id, const bool is_parent_command);
			void add_default_command_if_missing(tinyxml2::XMLElement* xml_default_command);
			void add_default_command_if_missing(std::shared_ptr<xml_class_default_command>& new_default_command);
			void exclude_commands(void);
			void check_commands_validity(void);
			void set_final_commands_priority(void);
			std::unordered_map<std::string, xmlCmdSP_t> commands_map;
			std::vector<xmlCmdSP_t> commands_ordered_by_priority;
			std::unordered_set<std::string> commands_to_exclude_set;
			defaultClassCommandsList_t default_commands_list;
				
			/*
				class attributes
			*/
			void add_attribute_if_missing(std::string name, std::string value);
			attributesMap_t attributes_map;

			/*
				class sounds
			*/
			void add_sound_if_missing(std::string name, std::string value);
			std::unordered_map<std::string, std::string> sound_map;

			/*
				class values
			*/
			void add_value_if_missing(const std::string& valueID, std::shared_ptr<const xml_value> value);
			classValuesList_t values_list;

			/*
				attributes
			*/
			std::string class_name;
			std::string class_type_str;
			xml_class_type class_type;
			std::shared_ptr<centurion::assets::xml_values> xml_values_ptr;
			std::shared_ptr<centurion::assets::xml_scripts> xml_scripts_ptr;
			std::shared_ptr<xml_entity> entity;

			/*
				static attributes
			*/
			static std::unordered_map<std::string, xml_class_type> class_type_map;
		};
	};
};
