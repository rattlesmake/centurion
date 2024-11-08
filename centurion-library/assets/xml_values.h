/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

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

class PyService;

namespace centurion
{
	namespace assets
	{
		class xml_value;
		class xml_values;
		class xml_class;

		typedef std::shared_ptr<xml_values> xmlValuesPtr_t;

		/*
			this class reads the content of assets/data.zip/object_values
		*/
		class xml_values
		{
			friend class xml_class;
		private:
			xml_values(void);

			/*
				main container
			*/
			std::unordered_map<std::string, std::shared_ptr<const xml_value>> values_map;

			/*
				shortcuts
			*/
			PyService& python_s;
			rattlesmake::services::zip_service& zip_s;
		public:
			/*
				Constants
			*/
			const static uint8_t MAX_NUMBER_OF_VALUES_PER_CLASS = 12;

			xml_values(const xml_values& other) = delete;
			xml_values& operator=(const xml_values& other) = delete;
			~xml_values(void);
			/*
				use this method to instantiate a new shared_ptr of xml_values
				typically inside of game environment constructor
				note: it must be called after xml_scripts instantiation
				note: it must be called before xml_classes instantiation
			*/
			[[nodiscard]] static xmlValuesPtr_t create(void);

			[[nodiscard]] const std::shared_ptr<const xml_value> get_value_ptr(const std::string& name) const noexcept;
		};
	};
};
