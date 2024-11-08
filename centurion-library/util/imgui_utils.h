/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <iostream>
#include <string>
#include <unordered_map>

namespace gui { class Image; }
namespace rattlesmake { namespace image { class png; }; };

namespace ImGui
{
	void cil_RenderImage(std::shared_ptr<rattlesmake::image::png> img, ImDrawList* drawList, const uint32_t x, const uint32_t y, const int flags, const uint32_t customWidth, const uint32_t customHeight);

	void SaveFont(std::string font_name, ImFont* imfont);

	ImFont* GetFontByName(std::string font_name);

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, const int max_options);

	bool ImageButton(std::shared_ptr<rattlesmake::image::png> image, unsigned int pickingID, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, ImGuiButtonFlags flags = 0);

	bool ButtonBehavior(std::shared_ptr<rattlesmake::image::png> image, const ImRect& bb, ImGuiID id, unsigned int pickingID, bool* out_hovered, bool* out_held, ImVec2 pos, ImVec2 size, ImGuiButtonFlags flags = 0);

	uint16_t GetStateCursor(const char* label);
};
