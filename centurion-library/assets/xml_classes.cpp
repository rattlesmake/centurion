#include "xml_classes.h"
#include "xml_class.h"
#include "xml_class_command.h"
#include "xml_class_method.h"
#include "xml_script.h"
#include "xml_scripts.h"

// stl
#include <iostream>
#include <algorithm>

// services
#include <zipservice.h>

// utils
#include <encode_utils.h>

namespace centurion
{
	namespace assets
	{
		xmlClassesSP_t xml_classes::create(std::weak_ptr<centurion::assets::xml_values> xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts> xml_scripts_ptr, const bool exclude_noclass)
		{
			return std::shared_ptr<xml_classes>{ new xml_classes{ std::move(xml_values_ptr), std::move(xml_scripts_ptr), exclude_noclass } };
		}

		const std::vector<std::string>& xml_classes::get_xml_classes_names(void) const noexcept
		{
			return this->classes_names_vec;
		}

		const std::shared_ptr<xml_class> xml_classes::get_xml_class(const std::string& class_name) const noexcept
		{
			return this->classes_map.contains(class_name) ? this->classes_map.at(class_name) : std::shared_ptr<xml_class>();
		}

		xml_classes::xml_classes(std::weak_ptr<centurion::assets::xml_values>&& xml_values_ptr, std::weak_ptr<centurion::assets::xml_scripts>&& xml_scripts_ptr, const bool exclude_noclass) :
			xml_values_ptr(std::move(xml_values_ptr)),
			xml_scripts_ptr(std::move(xml_scripts_ptr)),
			zip_s(rattlesmake::services::zip_service::get_instance())
		{
			read_all_xml_classes();
			set_xml_classes_hierarchy(exclude_noclass);
		}

		void xml_classes::read_all_xml_classes(void)
		{
			auto file_list = zip_s.get_all_files_within_folder("?data.zip", "classes/", "xml");
			for (uint32_t i = 0; i < file_list.size(); ++i)
			{
				std::string& path = file_list[i];
				Encode::NormalizePath(path);

				// read xml file
				const std::string fileTxt = zip_s.get_text_file("?data.zip", path);
				if (fileTxt.empty() == false)
				{
					tinyxml2::XMLDocument xmlFile;
					const auto resultParsing = xmlFile.Parse(fileTxt.c_str());
					assert(resultParsing == tinyxml2::XML_SUCCESS);

					// create xml_class object and add to map
					auto new_xml_class = xml_class::create(xmlFile.FirstChildElement("object"), this->xml_values_ptr, this->xml_scripts_ptr);
					if (new_xml_class)
					{
						std::string class_name = new_xml_class->get_class_name_cref();
						assert(!this->classes_map.contains(class_name));
						this->classes_map.insert({ std::move(class_name), std::move(new_xml_class) });
					}
				}
			}
		}

		void xml_classes::set_xml_classes_hierarchy(const bool exclude_noclass)
		{
			std::list<std::string> classes_to_drop;
			for (auto& [class_name, data] : this->classes_map)
			{
				if (data->get_class_type() != xml_class_type::e_noClass || !exclude_noclass)
				{
					this->recursive_inject_parent_xml_class(data, data->get_parent_class_name_cref(), 1);
					
					data->exclude_commands();
					data->set_final_commands_priority();

					this->classes_names_vec.push_back(class_name);
				}
				else
				{
					classes_to_drop.push_back(class_name);
				}
			}
			if (exclude_noclass)
			{
				while (classes_to_drop.empty() == false)
				{
					//Remove useless classes (parent ones)
					this->classes_map.erase(classes_to_drop.front());
					classes_to_drop.pop_front();
				}
			}

			// maybe not needed
			std::sort(this->classes_names_vec.begin(), this->classes_names_vec.end());
		}

		void xml_classes::recursive_inject_parent_xml_class(const std::shared_ptr<xml_class> fixed_class_ptr, const std::string& parent, const uint16_t recursive_call)
		{
			const bool stop_recursion = parent.empty();
			if (stop_recursion)
				return;

			// Inject parent data
			assert(this->classes_map.contains(parent));
			auto& parent_class = this->classes_map.at(parent);
			fixed_class_ptr->inject_parent_class_data(parent, parent_class, recursive_call);

			// Get new parent class name
			const std::string& new_parent_class_name = parent_class->get_parent_class_name_cref();

			// Recursive call
			this->recursive_inject_parent_xml_class(fixed_class_ptr, new_parent_class_name, recursive_call + 1);

			if (fixed_class_ptr->class_type != xml_class_type::e_decorationClass && fixed_class_ptr->class_type != xml_class_type::e_settlementClass)
			{
				// Values - They are read after recursive call in order to have before the parent's values into the values list
				fixed_class_ptr->inject_values(parent_class);
			}
		}

		void xml_classes::fill_editor_tree_src(std::vector<std::pair<std::string, std::string>>& src) const
		{
			for (auto const& [xml_class_name, xml_class] : this->classes_map)
			{
				std::optional<std::string> ent_path = xml_class->get_attribute("editor_tree");
				if (ent_path.has_value() == true)
				{
					src.push_back({ std::move(ent_path.value()), xml_class_name });
				}
			}
		}

		xml_classes::~xml_classes(void)
		{
		}
	};
};
