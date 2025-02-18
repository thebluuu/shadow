#pragma once

#include <cstdint>

#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <iostream>
#include <string_view>
#include <map>

#include "imgui.h"
#include "imgui_internal.h"

#if defined(USE_ICONS)
#include "icons.h" 
#endif



namespace ImGui
{

    static void TextGradiented(const char* text, ImU32 leftcolor, ImU32 rightcolor, float smooth = 175, float alpha = 266) {

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        const ImVec2 pos = window->DC.CursorPos;
        ImDrawList* draw_list = window->DrawList;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(text);
        const ImVec2 size = CalcTextSize(text);

        const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return;

        const ImU32 col_white = IM_COL32(255, 255, 255, alpha == 266 ? static_cast<int>(ImGui::GetStyle().Alpha * 255.f) : alpha);
        float centeredvertex = ImMax((int)smooth, 35);
        float vertex_out = centeredvertex * 0.50f;
        float text_inner = vertex_out - centeredvertex;
        const int vert_start_idx = draw_list->VtxBuffer.Size;
        draw_list->AddText(pos, col_white, text);
        const int vert_end_idx = draw_list->VtxBuffer.Size;
        for (int n = 0; n < 1; n++)
        {
            const ImU32 col_hues[2] = { leftcolor, rightcolor };
            ImVec2 textcenter(pos.x + (size.x / 2), pos.y + (size.y / 2));

            const float a0 = (n) / 6.0f * 2.0f * IM_PI - (0.5f / vertex_out);
            const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + (0.5f / vertex_out);

            ImVec2 gradient_p0(textcenter.x + ImCos(a0) * text_inner, textcenter.y + ImSin(a0) * text_inner);
            ImVec2 gradient_p1(textcenter.x + ImCos(a1) * text_inner, textcenter.y + ImSin(a1) * text_inner);
            ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
        }
    }

    static void TextGradiented(const char* text, ImVec2 pos, ImU32 leftcolor, ImU32 rightcolor, float smooth = 175, float alpha = 266) {

        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImDrawList* draw_list = window->DrawList;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(text);
        const ImVec2 size = CalcTextSize(text);

        const ImU32 col_white = IM_COL32(255, 255, 255, alpha == 266 ? static_cast<int>(ImGui::GetStyle().Alpha * 255.f) : alpha);
        float centeredvertex = ImMax((int)smooth, 35);
        float vertex_out = centeredvertex * 0.50f;
        float text_inner = vertex_out - centeredvertex;
        const int vert_start_idx = draw_list->VtxBuffer.Size;
        draw_list->AddText(pos, col_white, text);
        const int vert_end_idx = draw_list->VtxBuffer.Size;
        for (int n = 0; n < 1; n++)
        {
            const ImU32 col_hues[2] = { leftcolor, rightcolor };
            ImVec2 textcenter(pos.x + (size.x / 2), pos.y + (size.y / 2));

            const float a0 = (n) / 6.0f * 2.0f * IM_PI - (0.5f / vertex_out);
            const float a1 = (n + 1.0f) / 6.0f * 2.0f * IM_PI + (0.5f / vertex_out);

            ImVec2 gradient_p0(textcenter.x + ImCos(a0) * text_inner, textcenter.y + ImSin(a0) * text_inner);
            ImVec2 gradient_p1(textcenter.x + ImCos(a1) * text_inner, textcenter.y + ImSin(a1) * text_inner);
            ShadeVertsLinearColorGradientKeepAlpha(draw_list, vert_start_idx, vert_end_idx, gradient_p0, gradient_p1, col_hues[n], col_hues[n + 1]);
        }
    }

    static void RenderDropShadow(ImTextureID tex_id, float size, ImU8 opacity)
    {

        ImVec2 p = ImGui::GetWindowPos();
        ImVec2 s = ImGui::GetWindowSize();
        ImVec2 m = { p.x + s.x, p.y + s.y };
        float uv0 = 0.0f;
        float uv1 = 0.333333f;
        float uv2 = 0.666666f;
        float uv3 = 1.0f;
        ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(ImGui::GetStyle().Alpha * 255.f));
        ImDrawList* dl = ImGui::GetBackgroundDrawList();
        dl->PushClipRectFullScreen();
        dl->AddImage(tex_id, { p.x - size, p.y - size }, { p.x,        p.y }, { uv0, uv0 }, { uv1, uv1 }, col);
        dl->AddImage(tex_id, { p.x,        p.y - size }, { m.x,        p.y }, { uv1, uv0 }, { uv2, uv1 }, col);
        dl->AddImage(tex_id, { m.x,        p.y - size }, { m.x + size, p.y }, { uv2, uv0 }, { uv3, uv1 }, col);
        dl->AddImage(tex_id, { p.x - size, p.y }, { p.x,        m.y }, { uv0, uv1 }, { uv1, uv2 }, col);
        dl->AddImage(tex_id, { m.x,        p.y }, { m.x + size, m.y }, { uv2, uv1 }, { uv3, uv2 }, col);
        dl->AddImage(tex_id, { p.x - size, m.y }, { p.x,        m.y + size }, { uv0, uv2 }, { uv1, uv3 }, col);
        dl->AddImage(tex_id, { p.x,        m.y }, { m.x,        m.y + size }, { uv1, uv2 }, { uv2, uv3 }, col);
        dl->AddImage(tex_id, { m.x,        m.y }, { m.x + size, m.y + size }, { uv2, uv2 }, { uv3, uv3 }, col);
        dl->PopClipRect();

    }

}

namespace framework
{

    static ImFont* bold_font = nullptr;
    static ImFont* small_font = nullptr;
    static float animated_border_duration = 0.22f;

    struct scaled_font_t
    {
        ImFont* m_font{};
        float scale{};
    };

    static std::vector<scaled_font_t> bold_fonts{};
    static std::vector<scaled_font_t> default_fonts{};
    static int current_scaling = 0;

    static float get_dpi_scale()
    {

        return framework::current_scaling == 0 ? 1.f : framework::current_scaling == 1 ? 1.2f : framework::current_scaling == 2 ? 1.6f : framework::current_scaling == 3 ? 1.8f : framework::current_scaling == 4 ? 2.f : 1.f;

    }

    static float get_dpi_scale(int current_scaling)
    {

        return current_scaling == 0 ? 1.f : current_scaling == 1 ? 1.2f : current_scaling == 2 ? 1.6f : current_scaling == 3 ? 1.8f : current_scaling == 4 ? 2.f : 1.f;

    }

}