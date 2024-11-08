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

#include <memory>
#include <string>
#include <tinyxml2_utils.h>

namespace centurion
{
	namespace assets
	{
		class xml_class;
		class xml_scripts;
		class xml_script;

		class xml_class_method
		{
			friend class xml_class;
		private:
			std::string name;
			std::weak_ptr<xml_script> associated_script;
		public:
			xml_class_method(tinyxml2::XMLElement* el, const std::shared_ptr<xml_scripts>& scripts_collection);
			xml_class_method(const xml_class_method& other) = delete;
			xml_class_method& operator=(const xml_class_method& other) = delete;

			[[nodiscard]] const std::string& get_name_cref(void) const noexcept;
			[[nodiscard]] const std::string& get_script_id_cref(void) const noexcept;
			[[nodiscard]] std::shared_ptr<centurion::assets::xml_script> get_associated_script(void) const noexcept;
			[[nodiscard]] bool is_valid(void) const noexcept;
		};
	};
};
