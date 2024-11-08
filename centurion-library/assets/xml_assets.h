/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <memory>

#include <xml_typedef.h>

#include <xml_scripts.h>
#include <xml_values.h>
#include <xml_classes.h>

namespace centurion
{
	namespace assets
	{
		class xml_assets
		{
		private:
			std::shared_ptr<xml_classes> xml_classes_sp;
			std::shared_ptr<xml_scripts> xml_scripts_sp;
			std::shared_ptr<xml_values> xml_values_sp;
		public:
			xml_assets(void);
			xml_assets(const xml_assets& other) = delete;
			~xml_assets(void);

			xml_assets& operator=(const xml_assets& other) = delete;

			[[nodiscard]] std::shared_ptr<centurion::assets::xml_classes> get_xml_classes(void) const noexcept;
			[[nodiscard]] std::shared_ptr<centurion::assets::xml_scripts> get_xml_scripts(void) const noexcept;
			[[nodiscard]] std::shared_ptr<centurion::assets::xml_values> get_xml_values(void) const noexcept;
		};
	}
}
