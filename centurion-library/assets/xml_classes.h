/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include "xml_class_enums.h"

#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace rattlesmake
{
	namespace services
	{
		class zip_service;
	};
};

namespace centurion
{
	namespace assets
	{
		class xml_class;
		class xml_classes;
		class xml_values;
		class xml_value;
		class xml_script;
		class xml_scripts;

		/*
			This class reads the content of assets/data.zip/classes.
			The process starts during instantiation (using create:: static method).
		*/
		class xml_classes
		{
		public:
			/*
				use this method to instantiate a new shared_ptr of xml_classes
				typically inside of game environment constructor
				note: it must be called after xml_scripts instantiation
				note: it must be called after xml_values instantiation
			*/
			static xmlClassesSP_t create(std::weak_ptr<centurion::assets::xml_values> xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts> xml_scripts_ptr, const bool exclude_noclass);
			
			/*
				get a vector with all classes names (ordered alphabetically)
			*/
			[[nodiscard]] const std::vector<std::string>& get_xml_classes_names(void) const noexcept;

			/*
				get a pointer to the required xml_class (by class name)
			*/
			[[nodiscard]] const std::shared_ptr<xml_class> get_xml_class(const std::string& class_name) const noexcept;

			/*
				fill the editor tree source vector with classes data information
			*/
			void fill_editor_tree_src(std::vector<std::pair<std::string, std::string>>& src) const;
			
			~xml_classes(void);
		private:
			xml_classes(std::weak_ptr<centurion::assets::xml_values>&& xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts>&& xml_scripts_ptr, const bool exclude_noclass);
			
			/*
				initialization pipeline functions
			*/
			void read_all_xml_classes(void);
			void set_xml_classes_hierarchy(const bool exclude_noclass);

			/*
				classes manipulation
			*/
			void recursive_inject_parent_xml_class(const std::shared_ptr<xml_class> fixed_ptr, const std::string& parent, const uint16_t recursive_call);

			/*
				main container of data
			*/
			std::map<std::string, std::shared_ptr<xml_class>> classes_map;

			/*
				secondary containers
			*/
			std::vector<std::string> classes_names_vec; // an ordered vector with all classes

			/*
				shortcuts
			*/
			rattlesmake::services::zip_service& zip_s;
			std::weak_ptr<centurion::assets::xml_values> xml_values_ptr;
			std::weak_ptr<centurion::assets::xml_scripts> xml_scripts_ptr;
		};
	};
};
