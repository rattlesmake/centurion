/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>
#include <unordered_map>

namespace centurion
{
	namespace assets
	{
		class xml_script;
		class xml_scripts;

		typedef std::shared_ptr<xml_scripts> xmlScriptsPtr_t;

		/*
			this class reads the content of assets/data.zip/scripts
		*/
		class xml_scripts
		{
		private:
			xml_scripts(void);

			typedef std::unordered_map<std::string, std::shared_ptr<xml_script>> scriptsCollection_t;
			scriptsCollection_t scripts;
		public:
			xml_scripts(const xml_scripts& other) = delete;
			xml_scripts& operator=(const xml_scripts& other) = delete;
			~xml_scripts(void);

			/*
				use this method to instantiate a new shared_ptr of xml_scripts
				typically inside of game environment constructor
				note: it must be called before xml_scripts instantiation
				note: it must be called before xml_values instantiation
			*/
			[[nodiscard]] static xmlScriptsPtr_t create(void);

			[[nodiscard]] std::shared_ptr<xml_script> get_script_by_id(const std::string& id) const;
		};
	};
};
