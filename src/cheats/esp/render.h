#pragma once

#include <string>

#include "../../Graphics/overlay/imgui/imgui.h"
#include "../../Graphics/overlay/imgui/imgui_internal.h"
#include "../../Graphics/overlay/imgui/includes.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
struct render_t
{

#define vec_ref const ImVec2&
#define decl static auto

	inline static ImFont* verdana_12{ NULL };
	inline static ImFont* verdana_bold{ NULL };

	decl rect_filled_blur(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f, int layers = 5, float offset = 0.5f) {
		auto draw = ImGui::GetBackgroundDrawList();
		for (int i = 0; i < layers; ++i) {
			ImVec2 offset_pos{ offset * i, offset * i };
			draw->AddRectFilled(from - offset_pos, to + offset_pos, col, rounding);
		}
	}

	decl rect(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f) {
		ImGui::GetBackgroundDrawList()->AddRect(from, to, col, rounding);
	}

	decl rect_filled(vec_ref from, vec_ref to, ImU32 col, float rounding = 0.f) {
		ImGui::GetBackgroundDrawList()->AddRectFilled(from, to, col, rounding);
	}

	decl outlined_rect_blur(vec_ref pos, vec_ref size, ImU32 col, float blur_intensity = 0.5f, int layers = 30) {
		auto draw = ImGui::GetBackgroundDrawList();

		for (int i = 1; i <= layers; ++i) {
			float alpha_factor = 1.0f - (float(i) / layers);
			ImU32 faded_col = ImGui::ColorConvertFloat4ToU32(ImVec4(
				(col >> IM_COL32_R_SHIFT & 0xFF) / 255.0f,
				(col >> IM_COL32_G_SHIFT & 0xFF) / 255.0f,
				(col >> IM_COL32_B_SHIFT & 0xFF) / 255.0f,
				alpha_factor * blur_intensity
			));

			ImVec2 offset(i * 0.5f, i * 0.5f);
			draw->AddRect(pos - offset, pos + size + offset, faded_col, 0.0f);
			draw->AddRect(pos + offset, pos + size - offset, faded_col, 0.0f);
		}

		draw->AddRect(pos, pos + size, col, 0.0f);
	}

	decl outlined_rect(ImVec2& pos, ImVec2& size, ImU32 col, float rounding = 0.f) {
		pos.x = std::round(pos.x); pos.y = std::round(pos.y);
		size.x = std::round(size.x); size.y = std::round(size.y);

		auto draw = ImGui::GetBackgroundDrawList();
		ImRect rect_bb(std::round(pos.x), std::round(pos.y), std::round(pos.x + size.x), std::round(pos.y + size.y));

		ImGuiStyle& style = ImGui::GetStyle();
		float shadow_size = style.WindowShadowSize;
		ImU32 shadow_col = ImGui::GetColorU32(ImGuiCol_WindowShadow);
		ImVec2 shadow_offset = ImVec2(ImCos(style.WindowShadowOffsetAngle), ImSin(style.WindowShadowOffsetAngle)) * style.WindowShadowOffsetDist;

		draw->AddShadowRect(rect_bb.Min, rect_bb.Max, shadow_col, shadow_size, shadow_offset, ImDrawFlags_ShadowCutOutShapeBackground, rounding);

		if (globals::fill_box) {
			ImU32 box_color = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_5[0], globals::color_5[1], globals::color_5[2], 0.3f));
			ImU32 fill_col = box_color;
		
			ImU32 glowboy = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::color_1[0], globals::color_1[1], globals::color_1[2], globals::glow_opacity));
			ImU32 glow_color = glowboy;

			ImVec2 glow_offset = { 0, 0 };
			draw->AddShadowRect(rect_bb.Min, rect_bb.Max, glow_color, globals::glow_size, glow_offset, ImDrawFlags_ShadowCutOutShapeBackground, 0.0f);
			draw->AddRectFilled(rect_bb.Min, rect_bb.Max, fill_col);
		}

		draw->AddRect(rect_bb.Min, rect_bb.Max, IM_COL32(0, 0, 0, col >> 24), rounding);

		draw->AddRect(
			ImVec2(std::round(rect_bb.Min.x - 2.f), std::round(rect_bb.Min.y - 2.f)),
			ImVec2(std::round(rect_bb.Max.x + 2.f), std::round(rect_bb.Max.y + 2.f)),
			IM_COL32(0, 0, 0, col >> 24),
			rounding
		);

		draw->AddRect(
			ImVec2(std::round(rect_bb.Min.x - 1.f), std::round(rect_bb.Min.y - 1.f)),
			ImVec2(std::round(rect_bb.Max.x + 1.f), std::round(rect_bb.Max.y + 1.f)),
			col,
			rounding
		);
	}


	decl cornered_rect(vec_ref pos, vec_ref size, ImU32 col)
	{

		auto draw = ImGui::GetBackgroundDrawList();

		float X = pos.x; float Y = pos.y;
		float W = size.x; float H = size.y;

		float lineW = (size.x / 4);
		float lineH = (size.y / 4);
		float lineT = 1;

		auto outline = IM_COL32(0, 0, 0, col >> 24);
		ImU32 fillColor = IM_COL32((col & 0xFF0000) >> 16, (col & 0x00FF00) >> 8, col & 0x0000FF, (col >> 24) / 2);;
		if (globals::fill_box) {
			draw->AddRectFilled({ X, Y }, { X + W, Y + H }, fillColor, 0.0f);
		}

		draw->AddLine({ X - lineT + 1.f, Y - lineT }, { X + lineW, Y - lineT }, outline);
		draw->AddLine({ X - lineT, Y - lineT }, { X - lineT, Y + lineH }, outline);

		draw->AddLine({ X + W - lineW, Y - lineT }, { X + W + lineT, Y - lineT }, outline);
		draw->AddLine({ X + W + lineT, Y - lineT }, { X + W + lineT, Y + lineH }, outline);

		draw->AddLine({ X + W + lineT, Y + H - lineH }, { X + W + lineT, Y + H + lineT }, outline);
		draw->AddLine({ X + W - lineW, Y + H + lineT }, { X + W + lineT, Y + H + lineT }, outline);

		draw->AddLine({ X - lineT, Y + H - lineH }, { X - lineT, Y + H + lineT }, outline);
		draw->AddLine({ X - lineT, Y + H + lineT }, { X + lineW, Y + H + lineT }, outline);

		{

			draw->AddLine({ X - (lineT - 3), Y - (lineT - 2) }, { X + lineW, Y - (lineT - 2) }, outline);
			draw->AddLine({ X - (lineT - 2), Y - (lineT - 2) }, { X - (lineT - 2), Y + lineH }, outline);

			draw->AddLine({ X - (lineT - 2), Y + H - lineH }, { X - (lineT - 2), Y + H + (lineT - 2) }, outline);
			draw->AddLine({ X - (lineT - 2), Y + H + (lineT - 2) }, { X + lineW, Y + H + (lineT - 2) }, outline);

			draw->AddLine({ X + W - lineW, Y - (lineT - 2) }, { X + W + (lineT - 2), Y - (lineT - 2) }, outline);
			draw->AddLine({ X + W + (lineT - 2), Y - (lineT - 2) }, { X + W + (lineT - 2), Y + lineH }, outline);

			draw->AddLine({ X + W + (lineT - 2), Y + H - lineH }, { X + W + (lineT - 2), Y + H + (lineT - 2) }, outline);
			draw->AddLine({ X + W - lineW, Y + H + (lineT - 2) }, { X + W + (lineT - 2), Y + H + (lineT - 2) }, outline);

		}

		draw->AddLine({ X, Y }, { X, Y + lineH }, col);
		draw->AddLine({ X + 1.f, Y }, { X + lineW, Y }, col);

		draw->AddLine({ X + W - lineW, Y }, { X + W, Y }, col);
		draw->AddLine({ X + W , Y }, { X + W, Y + lineH }, col);

		draw->AddLine({ X, Y + H - lineH }, { X, Y + H }, col);
		draw->AddLine({ X, Y + H }, { X + lineW, Y + H }, col);

		draw->AddLine({ X + W - lineW, Y + H }, { X + W, Y + H }, col);
		draw->AddLine({ X + W, Y + H - lineH }, { X + W, Y + H }, col);

	}

	decl health_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor, float x_pad = 4.f, float bar_width = 4.f, bool blue = false)
	{

		int clamped_health = static_cast<int>(std::min(max, current));
		float bar_height = ((size.y - 1.f) * clamped_health) / max;

		auto adjusted_left = ImVec2{ pos.x + size.x, pos.y + size.y - 1.f };
		auto adjusted_right = ImVec2{ pos.x + size.x, pos.y + 1.f };

		rect_filled(
			{ (adjusted_left.x - bar_width) - x_pad, adjusted_left.y },
			{ adjusted_left.x - x_pad, adjusted_right.y },
			IM_COL32(60, 60, 60, static_cast<int>(200 * alpha_factor))
		);

		ImU32 healthBarColor = ImGui::ColorConvertFloat4ToU32(ImVec4(globals::box_color[0], globals::box_color[1], globals::box_color[2], globals::alpha));

		
		rect_filled(
			{ (adjusted_left.x - bar_width - x_pad + 1.f), adjusted_left.y - bar_height },
			{ (adjusted_left.x - x_pad - 1.f), adjusted_left.y },
			healthBarColor
		);

		rect(
			{ (adjusted_left.x - bar_width) - x_pad, adjusted_left.y },
			{ adjusted_left.x - x_pad, adjusted_right.y },
			IM_COL32(0, 0, 0, static_cast<int>(255 * alpha_factor))
		);
	}

	decl shield_bar_blur(float max, float current, vec_ref pos, vec_ref size, float alpha_factor, int layers = 16, float blur_intensity = 0.3f) {
		auto draw = ImGui::GetBackgroundDrawList();

		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;
		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		float bar_width = current * adj_size_x / max;

		for (int i = 1; i <= layers; ++i) {
			float alpha_layer = (1.0f - (float(i) / layers)) * blur_intensity;
			ImU32 blur_color = IM_COL32(52, 103, 235, static_cast<int>(150 * alpha_layer * alpha_factor));

			ImVec2 offset(i * 0.3f, i * 0.3f);
			draw->AddRectFilled(adj_from - offset, adj_to + offset, blur_color, 0.0f);
		}

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		ImVec2 shield_bar_from = ImVec2((adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f);
		ImVec2 shield_bar_to = ImVec2((adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f);
		rect_filled(shield_bar_from, shield_bar_to, IM_COL32(52, 103, 235, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));
	}
	decl flame_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor)
	{
		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;

		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		auto bar_width = current * adj_size_x / max;

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		rect_filled({ (adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f }, { (adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f }, IM_COL32(255, 0, 0, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));
	}

	decl shield_bar(float max, float current, vec_ref pos, vec_ref size, float alpha_factor)
	{

		auto adj_pos_x = pos.x - -1;
		auto adj_size_x = size.x + -2;

		auto adj_from = ImVec2(adj_pos_x, pos.y + 5.f);
		auto adj_to = ImVec2(adj_pos_x + adj_size_x, pos.y + 8.f);

		auto bar_width = current * adj_size_x / max;

		rect_filled(adj_from, adj_to, IM_COL32(60, 60, 60, 200 * alpha_factor));

		rect_filled({ (adj_pos_x + adj_size_x) + 1.f, pos.y + 5.f }, { (adj_pos_x + adj_size_x) - bar_width, pos.y + 8.f }, IM_COL32(52, 103, 235, 255 * alpha_factor));

		rect(adj_from - ImVec2{ 1.f, 1.f }, adj_to + ImVec2{ 1.f, 1.f }, IM_COL32(0, 0, 0, 255 * alpha_factor));

	}

	decl text(vec_ref pos, const std::string& text, ImU32 col, ImFont* font) {
		ImGui::GetBackgroundDrawList()->AddText(font, 13.5f, pos, col, text.c_str());
	}

	decl text_shadowed(vec_ref pos, const std::string& text, ImU32 col, ImFont* font, float font_size = 13.5f)
	{

		auto alpha = col >> 24;

		if (alpha > 0)
		{

			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(-1.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(1.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(1.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(-1.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(0.f, 1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(0.f, -1.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());

			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(1.f, 0.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());
			ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos + ImVec2(-1.f, 0.f), IM_COL32(0, 0, 0, col >> 24), text.c_str());

		}

		ImGui::GetBackgroundDrawList()->AddText(font, font_size, pos, col, text.c_str());

	}

	decl line(vec_ref from, vec_ref to, ImU32 col, float thickness) {
		ImGui::GetBackgroundDrawList()->AddLine(from, to, col, thickness);
	}

	decl line_segment(vec_ref from, vec_ref to, ImU32 col, float thickness, float segments = 1.f) {

		if (segments > 1)
		{

			auto draw_list = ImGui::GetBackgroundDrawList();

			float segment_length = 1.0f / segments;
			ImVec2 delta = to - from;

			for (int i = 0; i < segments; ++i)
			{

				float alpha = segment_length * i;

				ImVec2 segment_pos = ImVec2(from.x + delta.x * alpha, from.y + delta.y * alpha);
				draw_list->AddCircleFilled(segment_pos, thickness, col);

			}

		}

	}

	decl circle(vec_ref pos, ImU32 col, float radius, int segments = 6) {
		ImGui::GetBackgroundDrawList()->AddCircle(pos, radius, col, segments);
	}

	decl circle_filled(vec_ref pos, ImU32 col, float radius) {
		ImGui::GetBackgroundDrawList()->AddCircleFilled(pos, radius, col, 128);
	}

	decl initialize()
	{

	}

}; inline render_t Visualize{};