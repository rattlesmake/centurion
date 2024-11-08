#include "imgui_utils.h"

#include <engine.h>
#include <png.h>

#include <viewport.h>
#include <mouse.h>

namespace ImGui
{
	namespace
	{
		std::unordered_map<std::string, ImFont*> FONT_MAP = std::unordered_map<std::string, ImFont*>();
	};

	void cil_RenderImage(std::shared_ptr<rattlesmake::image::png> img, ImDrawList* drawList, const uint32_t x, const uint32_t y, const int flags, const uint32_t customWidth, const uint32_t customHeight)
	{
		if (img->is_png_read_correctly() == false)
			return;

		img->set_flags(flags);
		img->set_position(x, y);
		img->set_custom_size(customWidth, customHeight);

		float imageWidth = float((customWidth != RIL_IMAGE_DEFAULT_VAL) ? customWidth : img->get_width());
		float imageHeight = float((customHeight != RIL_IMAGE_DEFAULT_VAL) ? customHeight : img->get_height());
		auto _positionMin = ImVec2(x, y);
		auto _positionMax = ImVec2(x + imageWidth, y + imageHeight);
		drawList->AddImage((void*)(intptr_t)img->get_opengl_texture_id(), _positionMin, _positionMax);
	}

	void SaveFont(std::string font_name, ImFont* imfont)
	{
		FONT_MAP[font_name] = imfont;
	}

	ImFont* GetFontByName(std::string font_name)
	{
		return FONT_MAP[font_name];
	}

	static auto vector_getter = [](void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
	{
		return Combo(label, currIndex, vector_getter,
			static_cast<void*>(&values), (int)values.size());
	}

	bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values, const int max_options)
	{
		if (values.empty()) { return false; }
		return ListBox(label, currIndex, vector_getter,
			static_cast<void*>(&values), max_options);
	}

	bool ImGui::ImageButton(std::shared_ptr<rattlesmake::image::png> image, unsigned int pickingID, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, ImGuiButtonFlags flags)
	{
		if (image == nullptr)
			return false;

		ImTextureID user_texture_id = (void*)(intptr_t)image->get_opengl_texture_id();
		int frame_padding = 0;
		ImVec4 tint_col = ImVec4(1, 1, 1, 1);
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		// Default to using texture ID as ID. User can still push string/integer prefixes.
		// We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
		PushID((void*)(intptr_t)user_texture_id);
		const ImGuiID id = window->GetID("#image");
		PopID();

		const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
		const ImVec2 bb2 = ImVec2(window->DC.CursorPos.x + size.x + padding.x * 2, window->DC.CursorPos.y + size.y + padding.y * 2);
		const ImRect bb(window->DC.CursorPos, bb2);

		const ImVec2 image_bb1 = ImVec2(window->DC.CursorPos.x + padding.x, window->DC.CursorPos.y + padding.y);
		const ImVec2 image_bb2 = ImVec2(window->DC.CursorPos.x + padding.x + size.x, window->DC.CursorPos.y + padding.y + size.y);
		const ImRect image_bb(image_bb1, image_bb2);
		ItemSize(bb);
		if (!ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(image, bb, id, pickingID, &hovered, &held, image_bb1, size, flags);

		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavHighlight(bb, id);
		RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));

		window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

		return pressed;
	}

	bool ImGui::ButtonBehavior(std::shared_ptr<rattlesmake::image::png> image, const ImRect& bb, ImGuiID id, unsigned int pickingID, bool* out_hovered, bool* out_held, ImVec2 pos, ImVec2 size, ImGuiButtonFlags flags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = GetCurrentWindow();

		if (flags & ImGuiItemFlags_Disabled)
		{
			if (out_hovered) *out_hovered = false;
			if (out_held) *out_held = false;
			if (g.ActiveId == id) ClearActiveID();
			return false;
		}

		ImGuiWindow* backup_hovered_window = g.HoveredWindow;

		bool pressed = false;
		bool hovered = ItemHoverable(bb, id);

		// Mouse handling
		if (hovered)
		{
			if (IsMouseClicked(0, true))
			{
				uint32_t xLeftclick = (uint32_t)rattlesmake::peripherals::mouse::get_instance().GetXLeftClick();
				uint32_t yLeftclick = (uint32_t)rattlesmake::peripherals::mouse::get_instance().GetYLeftClick();
				pressed = image->check_if_clicked(int(pos.x), int(rattlesmake::peripherals::viewport::get_instance().GetHeight() - pos.y - size.y), xLeftclick, yLeftclick, pickingID, int(size.x), int(size.y));
			}

			if (g.HoveredIdTimer >= 1.f)
			{
				//Create tooltip here
			}
		}
		return pressed;
	}

	uint16_t ImGui::GetStateCursor(const char* label)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window == nullptr)
			return 0;

		ImGuiInputTextState* state = GetInputTextState(window->GetID(label));
		if (state == nullptr)
			return 0;

		return state->Stb.cursor;
	}
	
};