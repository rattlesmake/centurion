#include "xml_assets.h"

namespace centurion
{
	namespace assets
	{
		#pragma region Constructor and destructor:
		xml_assets::xml_assets(void)
		{
			// initialize assets
			this->xml_scripts_sp = centurion::assets::xml_scripts::create();
			this->xml_values_sp = centurion::assets::xml_values::create();
			this->xml_classes_sp = centurion::assets::xml_classes::create(this->xml_values_sp, this->xml_scripts_sp, true);
		}

		xml_assets::~xml_assets(void)
		{
		}
		#pragma endregion


		#pragma region Public members:
		std::shared_ptr<centurion::assets::xml_classes> xml_assets::get_xml_classes(void) const noexcept
		{
			return this->xml_classes_sp;
		}

		std::shared_ptr<centurion::assets::xml_scripts> xml_assets::get_xml_scripts(void) const noexcept
		{
			return this->xml_scripts_sp;
		}

		std::shared_ptr<centurion::assets::xml_values> xml_assets::get_xml_values(void) const noexcept
		{
			return this->xml_values_sp;
		}
		#pragma endregion
	}
}
