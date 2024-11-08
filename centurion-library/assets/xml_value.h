/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace tinyxml2 { class XMLElement; };

namespace centurion
{
	namespace assets
	{
		class xml_values;

		class xml_value
		{
			friend class xml_values;
		public:
			xml_value(const xml_value& other) = delete;
			~xml_value(void);

			xml_value& operator=(const xml_value& other) = delete;
			[[nodiscard]] bool operator==(const xml_value& other) const noexcept;

			[[nodiscard]] const std::string& get_icon_path_cref(void) const noexcept;
			[[nodiscard]] const std::string& get_function_cref(void) const noexcept;
			[[nodiscard]] const std::string& get_name_cref(void) const noexcept;
		private:
			explicit xml_value(tinyxml2::XMLElement* xml_element);

			/*
				the right way to create a new xml_value object.
				pass as xml_element the <value> tag of the xml class file
			*/
			[[nodiscard]] static const std::shared_ptr<xml_value> create(tinyxml2::XMLElement* xml_element);

			/*
				Attributes
			*/
			std::string name;
			std::string icon;
			std::string function;
		};
	};
};
