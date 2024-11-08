#include "mds.h"

//Engine, peripherals and input
#include <engine.h>

// services
#include <fileservice.h>
#include <zipservice.h>

// assets
#include <xml_scripts.h>
#include <xml_values.h>
#include <xml_classes.h>
#include <xml_class.h>

// stl
#include <iostream>
#include <sstream>
#include <iostream>
#include <iomanip>

#pragma region Constrcutor and destructor:
MDS::MDS(void) : IEnvironment(IEnvironment::Environments::e_mds)
{
	xml_scripts = centurion::assets::xml_scripts::create();
	xml_values = centurion::assets::xml_values::create();
	xml_classes = centurion::assets::xml_classes::create(xml_values, xml_scripts, false);

	classes_names = xml_classes->get_xml_classes_names();
	classes_bools = std::vector<bool>(classes_names.size(), false);
}

MDS::~MDS(void) {}
#pragma endregion

#pragma region Public static members:
std::shared_ptr<MDS> MDS::CreateMDS(void)
{
	std::shared_ptr<MDS> newMDS{ new MDS() };
	return newMDS;
}
MDS& MDS::GetInstance(void)
{
	//Instantiated on first use.
	static MDS instance;

	return instance;
}
#pragma endregion

#pragma region Public members:
void MDS::Run(void)
{
	auto vpHeight = Engine::GetInstance().GetViewportHeight();
	auto vpWidth = Engine::GetInstance().GetViewportWidth();

	//selected_class = "";

	if (!classes_names.empty())
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(200, vpHeight));
		ImGui::Begin("Classes", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);

		if (ImGui::ListBoxHeader("##ClassesList", ImVec2(190, vpHeight - 50)))
		{
			for (size_t i = 0; i < classes_names.size(); i++)
			{
				if (ImGui::Selectable(classes_names[i].c_str(), classes_bools.at(i)))
				{
					classes_bools[i] = true;
					selected_class = classes_names[i];

					for (size_t j = 0; j < classes_bools.size(); j++)
					{
						if (j != i)
						{
							classes_bools[j] = false;
						}
					}
				}
			}
		}
		ImGui::ListBoxFooter();

		ImGui::End();
	}

	auto xml_class = xml_classes->get_xml_class(selected_class);

	if (!selected_class.empty())
	{
		ImGui::SetNextWindowPos(ImVec2(201, 0));
		ImGui::SetNextWindowSize(ImVec2(500, vpHeight));
		ImGui::Begin(selected_class.c_str(), nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);

		ImGui::Text(std::string("Class name: " + xml_class->get_class_name_cref()).c_str());
		ImGui::Separator();
		ImGui::Text(std::string("Parent class name: " + xml_class->get_parent_class_name_cref()).c_str());
		ImGui::Separator();
		ImGui::Text("Attributes:");

		const auto& attrs = xml_class->get_attributes();
		for (const auto& [key, value] : attrs)
		{
			ImGui::Text(std::string(" " + key + ": " + value).c_str());
		}

		ImGui::End();
	}


	Engine::GetInstance().GetGameTime().Update();
}
#pragma endregion
