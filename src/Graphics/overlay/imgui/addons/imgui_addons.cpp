#include "imgui_addons.h"

#include "../imgui.h"
#include "../imgui_internal.h"

#include <map>
#include <string>
#include <iostream>

using namespace ImGui;
using std::string;

ImVec4 ImAdd::HexToColorVec4(unsigned int hex_color, float alpha)
{
    ImVec4 color;

    color.x = ((hex_color >> 16) & 0xFF) / 255.0f;
    color.y = ((hex_color >> 8) & 0xFF) / 255.0f;
    color.z = (hex_color & 0xFF) / 255.0f;
    color.w = alpha;

    return color;
}

void ImAdd::Text(ImVec4 color, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    PushStyleColor(ImGuiCol_Text, color);
    TextV(fmt, args);
    PopStyleColor();
    va_end(args);
}

void ImAdd::TextV(const char* fmt, va_list args)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const char* text, * text_end;
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);

    // Accept null ranges
    if (text == text_end)
        text = text_end = "";

    // Calculate length
    const char* text_begin = text;
    if (text_end == NULL)
        text_end = text + strlen(text); // FIXME-OPT

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 text_size = CalcTextSize(text, NULL, true);

    ImRect bb(pos, pos + text_size);
    ItemSize(text_size, 0.0f);
    if (!ItemAdd(bb, 0))
        return;

    // Render (we don't hide text after ## in this end-user function)
    PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
    {
        RenderTextWrapped(bb.Min + ImVec2(-1, -1), text_begin, text_end, 0.0f);
        RenderTextWrapped(bb.Min + ImVec2(1, -1), text_begin, text_end, 0.0f);
        RenderTextWrapped(bb.Min + ImVec2(1, 1), text_begin, text_end, 0.0f);
        RenderTextWrapped(bb.Min + ImVec2(-1, 1), text_begin, text_end, 0.0f);
    }
    PopStyleColor();
    RenderTextWrapped(bb.Min, text_begin, text_end, 0.0f);
}
bool ImAdd::Button(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
        pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;

    // Modify padding to be larger and the height to be shorter
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 4.0f, label_size.y + style.FramePadding.y * 1.5f); // Increased padding horizontally and reduced height

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Colors
    ImVec4 colFrame = GetStyleColorVec4((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), true, style.FrameRounding);
    window->DrawList->AddRectFilledMultiColor(bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));

    window->DrawList->AddText(pos + size / 2 - label_size / 2 + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + size / 2 - label_size / 2 + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + size / 2 - label_size / 2 + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + size / 2 - label_size / 2 + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + size / 2 - label_size / 2, GetColorU32(ImGuiCol_Text), label);

    return pressed;
}

bool ImAdd::CheckBox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    // Make the checkbox smaller by reducing the size of the checkbox itself
    float height = g.FontSize * 0.8f;  // 80% of the default font size

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImVec2(height, height);

    const ImRect bb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) *v = !*v;

    // Define Colors
    ImVec4 colFrame = GetStyleColorVec4(*v ? ImGuiCol_SliderGrab : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);

    // Smooth Animation
    static std::map<ImGuiID, ImVec4> anim;
    if (anim.find(id) == anim.end()) anim[id] = colFrame;
    anim[id] = ImLerp(anim[id], colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Draw Checkbox
    RenderNavHighlight(bb, id);
    window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(anim[id]), style.FrameRounding);

    // Shadow Effect
    window->DrawList->AddRectFilledMultiColor(pos, pos + size,
        GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0),
        GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));

    // Border
    if (style.FrameBorderSize) {
        window->DrawList->AddRect(pos, pos + size, GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    // Render Label with Shadow Effect, adjusted spacing for smaller checkbox
    ImVec2 text_pos = pos + ImVec2(height + style.ItemInnerSpacing.x * 0.8f, 0);  // Reduced spacing
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            window->DrawList->AddText(text_pos + ImVec2(x, y), GetColorU32(ImGuiCol_TitleBg), label);
        }
    }
    window->DrawList->AddText(text_pos, GetColorU32(ImGuiCol_Text), label);

    return pressed;
}
bool CustomSliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, float width, const char* format)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    float w = ImGui::CalcItemSize(ImVec2(width, 0), ImGui::CalcItemWidth(), 0).x - (label_size.x > 0 ? 0 : ImGui::CalcTextSize(value_buf).x + style.ItemInnerSpacing.x);

    ImVec2 pos = window->DC.CursorPos;
    float label_height = label_size.x > 0 ? g.FontSize + style.ItemInnerSpacing.y : 0.0f;
    const ImRect frame_bb(pos + ImVec2(0, label_height), pos + ImVec2(w, g.FontSize + label_height - 3));
    const ImRect total_bb(pos, pos + ImVec2(w, g.FontSize + label_height));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb)) return false;

    if (format == NULL) format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    bool hovered = ImGui::ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    bool clicked = hovered && ImGui::IsMouseClicked(0, id, 0);
    bool make_active = (clicked || g.NavActivateId == id);

    static std::map<ImGuiID, bool> is_dragging;
    if (make_active && !is_dragging[id]) is_dragging[id] = true;
    if (!hovered && !clicked) is_dragging[id] = false;

    ImVec4 colFrame = ImGui::GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colGrab = ImGui::GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);

    static std::map<ImGuiID, float> current_value;
    static std::map<ImGuiID, float> target_value;

    if (current_value.find(id) == current_value.end()) {
        current_value[id] = (data_type == ImGuiDataType_S32) ? (float)(*(int*)p_data) : *(float*)p_data;
        target_value[id] = current_value[id];
    }

    if (is_dragging[id]) {
        float mouse_pos = ImGui::GetIO().MousePos.x - frame_bb.Min.x;
        float range = (data_type == ImGuiDataType_S32) ? (float)(*(int*)p_max - *(int*)p_min) : *(float*)p_max - *(float*)p_min;
        float new_value = (data_type == ImGuiDataType_S32) ? (float)(*(int*)p_min) + mouse_pos / w * range : *(float*)p_min + mouse_pos / w * range;
        target_value[id] = new_value;
    }

    current_value[id] = ImLerp(current_value[id], target_value[id], 1.0f / 0.1f * ImGui::GetIO().DeltaTime);

    if (data_type == ImGuiDataType_S32)
        *(int*)p_data = (int)roundf(current_value[id]);
    else
        *(float*)p_data = current_value[id];

    ImRect grab_bb;
    bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, 0, &grab_bb);
    if (value_changed) ImGui::MarkItemEdited(id);

    ImVec2 grab_padding = ImVec2(grab_bb.Min.y - frame_bb.Min.y, 0);
    grab_padding.y = grab_padding.x;

    ImGui::RenderNavHighlight(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(colFrame), true, g.Style.FrameRounding);

    if (grab_bb.Max.x > grab_bb.Min.x) {
        window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), grab_bb.Max + grab_padding - ImVec2(style.FrameBorderSize, style.FrameBorderSize), ImGui::GetColorU32(colGrab), style.FrameRounding);
    }

    window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), frame_bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize),
        ImGui::GetColorU32(ImGuiCol_WindowShadow, 0), ImGui::GetColorU32(ImGuiCol_WindowShadow, 0), ImGui::GetColorU32(ImGuiCol_WindowShadow), ImGui::GetColorU32(ImGuiCol_WindowShadow));

    ImVec2 text_pos = pos + ImVec2(w / 2 - ImGui::CalcTextSize(value_buf).x / 2, label_size.y > 0 ? label_size.y + style.ItemInnerSpacing.y : 0);
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            window->DrawList->AddText(text_pos + ImVec2(x, y), ImGui::GetColorU32(ImGuiCol_TitleBg), value_buf);
        }
    }
    window->DrawList->AddText(text_pos, ImGui::GetColorU32(ImGuiCol_Text), value_buf);

    if (label_size.x > 0.0f) {
        ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
        ImGui::RenderText(pos + ImVec2(1, 1), label);
        ImGui::RenderText(pos + ImVec2(-1, -1), label);
        ImGui::RenderText(pos + ImVec2(-1, 1), label);
        ImGui::RenderText(pos + ImVec2(1, -1), label);
        ImGui::PopStyleColor();
        ImGui::RenderText(pos, label);
    }

    return value_changed;
}

bool ImAdd::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, float width, const char* format)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    // Make the slider skinner by adjusting the width of the item
    float w = CalcItemSize(ImVec2(width, 0), CalcItemWidth(), 0).x - (label_size.x > 0 ? 0 : CalcTextSize(value_buf).x + style.ItemInnerSpacing.x);

    ImVec2 pos = window->DC.CursorPos;
    // Reduced height for a skinner appearance
    float label_height = label_size.x > 0 ? g.FontSize + style.ItemInnerSpacing.y : 0.0f;
    const ImRect frame_bb(pos + ImVec2(0, label_height), pos + ImVec2(w, g.FontSize + label_height - 3));  // Reduced height of the frame
    const ImRect total_bb(pos, pos + ImVec2(w, g.FontSize + label_height));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb)) return false;

    if (format == NULL) format = DataTypeGetInfo(data_type)->PrintFmt;

    bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    bool clicked = hovered && IsMouseClicked(0, id, 0);
    bool make_active = (clicked || g.NavActivateId == id);

    // Only start tracking when the slider is active
    static std::map<ImGuiID, bool> is_dragging;
    if (make_active && !is_dragging[id]) {
        is_dragging[id] = true; // Slider has been activated, start tracking
    }

    if (!hovered && !clicked) {
        is_dragging[id] = false; // Reset the dragging state when mouse is not hovered or clicked
    }

    // Define Colors
    ImVec4 colFrame = GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colGrab = GetStyleColorVec4(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab);

    // Smooth Animation: only when dragging
    static std::map<ImGuiID, float> current_value;
    static std::map<ImGuiID, float> target_value;

    if (current_value.find(id) == current_value.end()) {
        current_value[id] = *((float*)p_data); // Initialize current value from the data pointer
        target_value[id] = *((float*)p_data); // Initialize target value from the data pointer
    }

    if (is_dragging[id]) {
        // Calculate target value based on mouse position
        float mouse_pos = ImGui::GetIO().MousePos.x - frame_bb.Min.x;
        float range = *(float*)p_max - *(float*)p_min;
        float new_value = *(float*)p_min + mouse_pos / w * range;
        target_value[id] = new_value; // Update target value based on mouse position
    }

    // Interpolation: smoothly update the slider value towards the target value
    current_value[id] = ImLerp(current_value[id], target_value[id], 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Update the slider's p_data with the smoothed value
    *((float*)p_data) = current_value[id];

    // Slider Behavior
    ImRect grab_bb;
    bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, 0, &grab_bb);
    if (value_changed) MarkItemEdited(id);

    // Smaller grab handle size for a skinner look
    ImVec2 grab_padding = ImVec2(grab_bb.Min.y - frame_bb.Min.y, 0);
    grab_padding.y = grab_padding.x;

    // Render Slider Bar
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(colFrame), true, g.Style.FrameRounding);

    // Render Grab Handle with reduced size
    if (grab_bb.Max.x > grab_bb.Min.x) {
        window->DrawList->AddRectFilled(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), grab_bb.Max + grab_padding - ImVec2(style.FrameBorderSize, style.FrameBorderSize), GetColorU32(colGrab), style.FrameRounding);
    }

    // Add Shadow Effect
    window->DrawList->AddRectFilledMultiColor(frame_bb.Min + ImVec2(style.FrameBorderSize, style.FrameBorderSize), frame_bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize),
        GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));

    // Centered Value Text
    ImVec2 text_pos = pos + ImVec2(w / 2 - CalcTextSize(value_buf).x / 2, label_size.y > 0 ? label_size.y + style.ItemInnerSpacing.y : 0);
    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            window->DrawList->AddText(text_pos + ImVec2(x, y), GetColorU32(ImGuiCol_TitleBg), value_buf);
        }
    }
    window->DrawList->AddText(text_pos, GetColorU32(ImGuiCol_Text), value_buf);

    // Label with Shadow Effect
    if (label_size.x > 0.0f) {
        PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
        RenderText(pos + ImVec2(1, 1), label);
        RenderText(pos + ImVec2(-1, -1), label);
        RenderText(pos + ImVec2(-1, 1), label);
        RenderText(pos + ImVec2(1, -1), label);
        PopStyleColor();
        RenderText(pos, label);
    }

    return value_changed;
}

bool ImAdd::SliderFloat(const char* label, float* v, float v_min, float v_max, float width, const char* format)
{
    return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, width, format);
}



bool ImAdd::SliderInt(const char* label, int* v, int v_min, int v_max, float width, const char* format)
{
     return CustomSliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, width, format);
}

bool ImAdd::Selectable(const char* label, bool selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    float borderSize = style.FrameBorderSize;

    // Colors
    ImVec4 colFrameMain = GetStyleColorVec4((hovered && !selected) ? held ? ImGuiCol_HeaderActive : ImGuiCol_HeaderHovered : ImGuiCol_Header);
    ImVec4 colFrameNull = colFrameMain; colFrameNull.w = 0.0f;
    ImVec4 colFrame = ((!hovered && !selected) ? colFrameNull : colFrameMain);

    ImVec4 colBorderMain = GetStyleColorVec4(ImGuiCol_Border);
    ImVec4 colBorderNull = colBorderMain; colBorderNull.w = 0.0f;
    ImVec4 colBorder = (selected ? colBorderMain : colBorderNull);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Border = colBorder;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (selected) {
        window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));
    }

    if (borderSize > 0)
        window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, borderSize);

    PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
    {
        RenderText(pos + (bb.Max - bb.Min) / 2 - label_size / 2 + ImVec2(1, 1), label);
        RenderText(pos + (bb.Max - bb.Min) / 2 - label_size / 2 + ImVec2(-1, -1), label);
        RenderText(pos + (bb.Max - bb.Min) / 2 - label_size / 2 + ImVec2(-1, 1), label);
        RenderText(pos + (bb.Max - bb.Min) / 2 - label_size / 2 + ImVec2(1, -1), label);
    }
    PopStyleColor();
    RenderText(pos + (bb.Max - bb.Min) / 2 - label_size / 2, label);

    return pressed;
}

bool ImAdd::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();

    ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
    g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
    if (window->SkipItems)
        return false;

    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together
    if (flags & ImGuiComboFlags_WidthFitPreview)
        IM_ASSERT((flags & (ImGuiComboFlags_NoPreview | ImGuiComboFlags_CustomPreview)) == 0);

    const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float preview_width = ((flags & ImGuiComboFlags_WidthFitPreview) && (preview_value != NULL)) ? CalcTextSize(preview_value, NULL, true).x : 0.0f;
    const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : ((flags & ImGuiComboFlags_WidthFitPreview) ? (arrow_size + preview_width + style.FramePadding.x * 2.0f) : CalcItemWidth());

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect bb(window->DC.CursorPos + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f), window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f) + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f));
    const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f) + ImVec2(0.0f, label_size.x > 0 ? label_size.y + style.ItemInnerSpacing.y : 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &bb))
        return false;

    // Open on click
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
    bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
    if (pressed && !popup_open)
    {
        OpenPopupEx(popup_id, ImGuiPopupFlags_None);
        popup_open = true;
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colText = GetStyleColorVec4((popup_open || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render shape
    const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
    RenderNavHighlight(bb, id);
    if (!(flags & ImGuiComboFlags_NoPreview))
        window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (!(flags & ImGuiComboFlags_NoArrowButton))
    {
        if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x) {
            //RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), GetColorU32(it_anim->second.Text), ImGuiDir_Down, 1.0f);
            window->DrawList->AddText(pos + ImVec2(w - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), "+");
            window->DrawList->AddText(pos + ImVec2(w - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), "+");
            window->DrawList->AddText(pos + ImVec2(w - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), "+");
            window->DrawList->AddText(pos + ImVec2(w - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), "+");
            window->DrawList->AddText(pos + ImVec2(w - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2), GetColorU32(ImGuiCol_Text), "+");
        }
    }

    window->DrawList->AddRectFilledMultiColor(bb.Min, bb.Max, GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));
    RenderFrameBorder(bb.Min, bb.Max, style.FrameRounding);

    // Custom preview
    if (flags & ImGuiComboFlags_CustomPreview)
    {
        g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
        IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
        preview_value = NULL;
    }

    // Render preview and label
    if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
    {
        if (g.LogEnabled) {
            LogSetNextTextDecoration("{", "}");
        }

        PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
        {
            RenderText(pos + ImVec2(style.FramePadding.y, ImGui::GetFontSize() + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(1, 1), preview_value);
            RenderText(pos + ImVec2(style.FramePadding.y, ImGui::GetFontSize() + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(-1, -1), preview_value);
            RenderText(pos + ImVec2(style.FramePadding.y, ImGui::GetFontSize() + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(-1, 1), preview_value);
            RenderText(pos + ImVec2(style.FramePadding.y, ImGui::GetFontSize() + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(1, -1), preview_value);
        }
        PopStyleColor();
        PushStyleColor(ImGuiCol_Text, GetStyleColorVec4(ImGuiCol_Text));
        RenderText(pos + ImVec2(style.FramePadding.y, ImGui::GetFontSize() + style.ItemInnerSpacing.y + style.FramePadding.y), preview_value);
        PopStyleColor();
    }
    if (label_size.x > 0) {
        PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TitleBg]);
        {
            RenderText(total_bb.Min + ImVec2(1, 1), label);
            RenderText(total_bb.Min + ImVec2(-1, -1), label);
            RenderText(total_bb.Min + ImVec2(-1, 1), label);
            RenderText(total_bb.Min + ImVec2(1, -1), label);
        }
        PopStyleColor();
        RenderText(total_bb.Min, label);
    }

    if (!popup_open)
        return false;

    g.NextWindowData.Flags = backup_next_window_data_flags;
    return BeginComboPopup(popup_id, bb, flags);
}
struct AnimatedNotification {
    bool active = false;
    float opacity = 1.0f;
    ImVec2 position = ImVec2(-300, -100); // Starting position off-screen (top-left corner)
    float duration = 0.0f;
};

void ImAdd::ShowNotification(const char* message, float duration) {
    static std::vector<AnimatedNotification> notifications;
    static float lastTime = 0.0f;
    float currentTime = ImGui::GetTime();

    // Add a new notification if it's not active and there's no notification currently showing
    if (currentTime - lastTime > duration && notifications.empty()) {
        AnimatedNotification notification;
        notification.active = true;
        notification.position = ImVec2(-300, -100); // Top-left off-screen
        notification.opacity = 0.0f;
        notification.duration = duration;
        notifications.push_back(notification);
        lastTime = currentTime;
        std::cout << "Notification triggered" << std::endl; // Debugging line
    }

    // Loop through all notifications and update their state
    for (auto& notification : notifications) {
        if (notification.active) {
            // Fade-in effect
            if (notification.opacity < 1.0f) {
                notification.opacity += ImGui::GetIO().DeltaTime * 2.0f; // Fade in over time
            }

            // Slide-in effect: Move the notification towards the top-left corner (0, 0)
            if (notification.position.x < 0.0f) {
                notification.position.x += ImGui::GetIO().DeltaTime * 300.0f; // Slide from left to right
            }
            if (notification.position.y < 0.0f) {
                notification.position.y += ImGui::GetIO().DeltaTime * 100.0f; // Slide from top to down
            }

            // After the notification duration ends, stop showing it
            if (currentTime - lastTime > notification.duration) {
                notification.active = false;
            }
        }
    }

    // Render notifications as debug boxes to check if positioning is working
    for (auto& notification : notifications) {
        if (notification.active) {
            // Draw a debug box to check if position and opacity are correct
            ImVec4 boxColor(1.0f, 0.0f, 0.0f, notification.opacity); // Red box with opacity
            ImGui::GetWindowDrawList()->AddRectFilled(
                notification.position,
                ImVec2(notification.position.x + 200, notification.position.y + 50),
                IM_COL32((int)(boxColor.x * 255), (int)(boxColor.y * 255), (int)(boxColor.z * 255), (int)(boxColor.w * 255))
            );
        }
    }
}


static float CalcMaxPopupHeightFromItemCount(int items_count)
{
    ImGuiContext& g = *GImGui;
    if (items_count <= 0)
        return FLT_MAX;
    return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImAdd::BeginComboPopup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags)
{
    ImGuiContext& g = *GImGui;
    if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
    {
        g.NextWindowData.ClearFlags();
        return false;
    }

    // Set popup size
    float w = bb.GetWidth();
    if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
    {
        g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
    }
    else
    {
        if ((flags & ImGuiComboFlags_HeightMask_) == 0)
            flags |= ImGuiComboFlags_HeightRegular;
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
        int popup_max_height_in_items = -1;
        if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
        else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
        else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
        ImVec2 constraint_min(0.0f, 0.0f), constraint_max(FLT_MAX, FLT_MAX);
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.x <= 0.0f) // Don't apply constraints if user specified a size
            constraint_min.x = w;
        if ((g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSize) == 0 || g.NextWindowData.SizeVal.y <= 0.0f)
            constraint_max.y = CalcMaxPopupHeightFromItemCount(popup_max_height_in_items);
        SetNextWindowSizeConstraints(constraint_min, constraint_max);
    }

    // This is essentially a specialized version of BeginPopupEx()
    char name[16];
    ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

    // Set position given a custom constraint (peak into expected window size so we can position it)
    // FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
    // FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
    if (ImGuiWindow* popup_window = FindWindowByName(name))
        if (popup_window->WasActive)
        {
            // Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
            ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
            popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
            ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
            ImVec2 pos = FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
            SetNextWindowPos(pos + ImVec2(0, g.Style.ItemSpacing.y));
        }

    // We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
    PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
    PushStyleVar(ImGuiStyleVar_PopupRounding, g.Style.FrameRounding);
    bool ret = Begin(name, NULL, window_flags);
    PopStyleVar(2);
    if (!ret)
    {
        EndPopup();
        IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
        return false;
    }
    return true;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int popup_max_height_in_items)
{
    ImGuiContext& g = *GImGui;

    // Call the getter to obtain the preview string which is a parameter to BeginCombo()
    const char* preview_value = NULL;
    if (*current_item >= 0 && *current_item < items_count)
        preview_value = getter(user_data, *current_item);

    // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
    if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
        SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

    if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
        return false;

    // Display items
    // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
    bool value_changed = false;
    for (int i = 0; i < items_count; i++)
    {
        const char* item_text = getter(user_data, i);
        if (item_text == NULL)
            item_text = "*Unknown item*";

        PushID(i);
        const bool item_selected = (i == *current_item);
        if (Selectable(item_text, item_selected, ImVec2(-0.1f, 0)) && *current_item != i)
        {
            value_changed = true;
            *current_item = i;
            //CloseCurrentPopup();
        }
        if (item_selected)
            SetItemDefaultFocus();
        PopID();
    }

    EndCombo();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}

// Getter for the old Combo() API: const char*[]
static const char* Items_ArrayGetter(void* data, int idx)
{
    const char* const* items = (const char* const*)data;
    return items[idx];
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static const char* Items_SingleStringGetter(void* data, int idx)
{
    const char* items_separated_by_zeros = (const char*)data;
    int items_count = 0;
    const char* p = items_separated_by_zeros;
    while (*p)
    {
        if (idx == items_count)
            break;
        p += strlen(p) + 1;
        items_count++;
    }
    return *p ? p : NULL;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
    const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
    return value_changed;
}

bool ImAdd::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, float width, int height_in_items)
{
    int items_count = 0;
    const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
    while (*p)
    {
        p += strlen(p) + 1;
        items_count++;
    }

    const float w = CalcItemSize(ImVec2(width, 0), CalcItemWidth(), 0).x;
    PushItemWidth(w);
    bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
    PopItemWidth();

    return value_changed;
}

bool ImAdd::MultiCombo(const char* label, std::vector<int>* selected, std::vector<const char*> items, float width)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(width, 0), label_size.x + style.FramePadding.x + GetFrameHeight(), GetFrameHeight());

    const ImRect bb(pos, pos + size + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize));
    ItemSize(size + ImVec2(0, g.FontSize + style.ItemInnerSpacing.y));
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held, popup_open = false;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) popup_open = !popup_open;

    std::string preview = "";

    int stop_at = -1;
    for (int i = 0; i < items.size(); i++)
    {
        if ((*selected)[i] == 1)
        {
            stop_at = i;
        }
    }

    for (int i = 0; i < items.size(); i++)
    {
        if ((*selected)[i] == 1)
        {
            preview += items[i] + std::string(i == stop_at ? "" : ", ");
        }
    }

    // Colors
    ImVec4 colFrame = GetStyleColorVec4(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImVec4 colText = GetStyleColorVec4((popup_open || hovered) ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animations
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize), bb.Max, GetColorU32(it_anim->second.Frame), true, style.FrameRounding);


    window->DrawList->AddRectFilledMultiColor(bb.Min + ImVec2(0, style.ItemInnerSpacing.y + g.FontSize) + ImVec2(style.FrameBorderSize, style.FrameBorderSize), bb.Max - ImVec2(style.FrameBorderSize, style.FrameBorderSize), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));

    window->DrawList->AddText(pos + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos, GetColorU32(ImGuiCol_Text), label);

    window->DrawList->AddText(pos + ImVec2(style.FramePadding.x, g.FontSize + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), preview.c_str());
    window->DrawList->AddText(pos + ImVec2(style.FramePadding.x, g.FontSize + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), preview.c_str());
    window->DrawList->AddText(pos + ImVec2(style.FramePadding.x, g.FontSize + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), preview.c_str());
    window->DrawList->AddText(pos + ImVec2(style.FramePadding.x, g.FontSize + style.ItemInnerSpacing.y + style.FramePadding.y) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), preview.c_str());
    window->DrawList->AddText(pos + ImVec2(style.FramePadding.x, g.FontSize + style.ItemInnerSpacing.y + style.FramePadding.y), GetColorU32(ImGuiCol_Text), preview.c_str());
    
    //RenderArrow(window->DrawList, bb.Min + ImVec2(size.x - GetFrameHeight(), g.FontSize + style.ItemInnerSpacing.y) + style.FramePadding, GetColorU32(it_anim->second.Text), ImGuiDir_Down, 1.0f);
    window->DrawList->AddText(pos + ImVec2(size.x - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), "+");
    window->DrawList->AddText(pos + ImVec2(size.x - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), "+");
    window->DrawList->AddText(pos + ImVec2(size.x - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), "+");
    window->DrawList->AddText(pos + ImVec2(size.x - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), "+");
    window->DrawList->AddText(pos + ImVec2(size.x - GetFrameHeight() / 2 - CalcTextSize("+").x / 2, GetFontSize() + style.ItemInnerSpacing.y + GetFrameHeight() / 2 - GetFontSize() / 2), GetColorU32(ImGuiCol_Text), "+");

    if (popup_open)
    {
        OpenPopup(std::string("MultiCombo_" + std::string(label)).c_str());
    }
    if (BeginPopup(std::string("MultiCombo_" + std::string(label)).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        SetWindowPos(pos + ImVec2(0, GetFrameHeight() + GetFontSize() + style.ItemInnerSpacing.y + style.WindowPadding.y));
        SetWindowSize(ImVec2(size.x, 0));

        for (int i = 0; i < items.size(); i++)
        {
            if (ImAdd::Selectable(items[i], (*selected)[i] == 1, ImVec2(size.x - style.WindowPadding.x * 2, 0)))
            {
                (*selected)[i] = (*selected)[i] == 1 ? 0 : 1;
            }
        }

        ImGui::EndPopup();
    }

    return pressed;
}

bool ImAdd::FrameRadioIcon(const char* label, bool* selected, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) { *selected = !*selected; }

    // Colors
    const ImVec4 colBorder = GetStyleColorVec4((hovered && !*selected) ? ImGuiCol_SliderGrab : ImGuiCol_Border);
    const ImVec4 colFrame = GetStyleColorVec4(*selected ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const ImVec4 colText = GetStyleColorVec4(*selected ? ImGuiCol_SliderGrab : hovered ? ImGuiCol_Text : ImGuiCol_SliderGrabActive);

    // Animation
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Border = colBorder;
        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (style.FrameBorderSize > 0) {
        window->DrawList->AddRect(pos, pos + size, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2), GetColorU32(it_anim->second.Text), label);

    return pressed;
}

bool ImAdd::FrameRadio(const char* label, int* v, int current_id, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held, active = *v == current_id;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    if (pressed) { *v = current_id; }

    // Colors
    const ImVec4 colBorder = GetStyleColorVec4((hovered && !active) ? ImGuiCol_SliderGrab : ImGuiCol_Border);
    const ImVec4 colFrame = GetStyleColorVec4(active ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    const ImVec4 colText = GetStyleColorVec4(active ? ImGuiCol_SliderGrab : hovered ? ImGuiCol_Text : ImGuiCol_TextDisabled);

    // Animation State (For smooth color transitions)
    struct stColors_State {
        ImVec4 Frame;
        ImVec4 Border;
        ImVec4 Text;
    };

    static std::map<ImGuiID, stColors_State> anim;
    auto it_anim = anim.find(id);

    if (it_anim == anim.end())
    {
        anim.insert({ id, stColors_State() });
        it_anim = anim.find(id);

        it_anim->second.Border = colBorder;
        it_anim->second.Frame = colFrame;
        it_anim->second.Text = colText;
    }

    // Smooth color transition for animations
    it_anim->second.Frame = ImLerp(it_anim->second.Frame, colFrame, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Border = ImLerp(it_anim->second.Border, colBorder, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);
    it_anim->second.Text = ImLerp(it_anim->second.Text, colText, 1.0f / IMADD_ANIMATIONS_SPEED * ImGui::GetIO().DeltaTime);

    // Render
    RenderNavHighlight(bb, id);

    // Render the frame with a glowing effect when selected
    if (active) {
        // Apply glow effect when the radio button is selected
        window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(it_anim->second.Frame), style.FrameRounding);
        window->DrawList->AddRect(pos, pos + size, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    // Frame rendering with smooth animations
    window->DrawList->AddRectFilled(pos, pos + size, GetColorU32(it_anim->second.Frame), style.FrameRounding);
    if (style.FrameBorderSize > 0) {
        window->DrawList->AddRect(pos, pos + size, GetColorU32(it_anim->second.Border), style.FrameRounding, 0, style.FrameBorderSize);
    }

    // Render text with shadow effect
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), label);
    window->DrawList->AddText(pos + ImVec2(size.x / 2 - label_size.x / 2, (size.y - label_size.y) / 2), GetColorU32(it_anim->second.Text), label);

    return pressed;
}

void ImAdd::BeginChild(const char* label, const ImVec2& size_arg)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = GetIO(); (void)io;
    const ImGuiStyle& style = g.Style;

    ImGui::BeginChild(label, size_arg, ImGuiChildFlags_None, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
    {

        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        float HeaderHeight = ImGui::GetFontSize() + style.WindowPadding.y * 2 + style.ChildBorderSize * 2;
            
        pDrawList->AddRectFilled(pos, pos + size, GetColorU32(ImGuiCol_ChildBg));

        if (style.ChildBorderSize > 0)
        {
            pDrawList->AddRect(pos, pos + size, GetColorU32(ImGuiCol_Border));
        }

        pDrawList->AddText(pos + style.WindowPadding + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), label);
        pDrawList->AddText(pos + style.WindowPadding + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), label);
        pDrawList->AddText(pos + style.WindowPadding + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), label);
        pDrawList->AddText(pos + style.WindowPadding + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), label);
        pDrawList->AddText(pos + style.WindowPadding + ImVec2(0, style.ChildBorderSize * 2), GetColorU32(ImGuiCol_Text), label);

        pDrawList->AddLine(pos + ImVec2(2, 2), pos + ImVec2(size.x - 2, 2), ImGui::GetColorU32(ImGuiCol_SliderGrab));
        pDrawList->AddLine(pos + ImVec2(2, 3), pos + ImVec2(size.x - 2, 3), ImGui::GetColorU32(ImGuiCol_SliderGrabActive));

        ImGui::SetCursorScreenPos(pos + ImVec2(0, HeaderHeight - style.WindowPadding.y));
        ImGui::BeginChild(string(label + string("#Main")).c_str(), ImVec2(), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
    }
}

void ImAdd::EndChild()
{
    ImGui::EndChild();
    ImGui::EndChild();
}

bool ImAdd::KeyBind(const char* str_id, int* k, float custom_width)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(str_id);

    char buf_display[32] = "None";
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 buf_display_size = ImGui::CalcTextSize(buf_display, NULL, true);
    float width = custom_width == 0 ? ImGui::CalcItemSize(ImVec2(-0.1f, 0), 0, 0).x : custom_width;
    float height = ImGui::GetFontSize();

    ImVec2 size = ImVec2(width, height);
    ImRect frame_bb(pos, pos + size);
    ImRect total_bb(pos, frame_bb.Max + ImVec2(size.x, 0));

    ImGui::ItemSize(total_bb);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id, 0);

    if (hovered)
    {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_Hand;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];

    if (user_clicked)
    {
        if (g.ActiveId != id)
        {
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *k = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0])
    {
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    bool value_changed = false;
    int key = *k;

    if (g.ActiveId == id)
    {
        if (!value_changed)
        {
            for (auto i = 0x08; i <= 0xA5; i++)
            {
                if (io.KeysDown[i])
                {
                    key = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            *k = 0;
            ImGui::ClearActiveID();
        }
        else *k = key;
    }

    window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), style.FrameRounding);
    window->DrawList->AddRectFilledMultiColor(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));

    const float border_size = g.Style.FrameBorderSize;
    if (border_size > 0.0f)
    {
        window->DrawList->AddRect(frame_bb.Min + ImVec2(1, 1), frame_bb.Max + ImVec2(1, 1), ImGui::GetColorU32(ImGuiCol_BorderShadow), style.FrameRounding, 0, border_size);
        window->DrawList->AddRect(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImGuiCol_Border), style.FrameRounding, 0, border_size);
    }

    ImGui::RenderNavHighlight(total_bb, id);

    if (*k != 0 && g.ActiveId != id)
        strcpy_s(buf_display, sizeof buf_display, szKeyNames[*k]);
    else if (g.ActiveId == id)
        strcpy_s(buf_display, sizeof buf_display, "...");

    window->DrawList->AddText(pos + (frame_bb.Max - frame_bb.Min) / 2 - CalcTextSize(buf_display) / 2 - ImVec2(0, 1) + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(1, 1), GetColorU32(ImGuiCol_TitleBg), buf_display);
    window->DrawList->AddText(pos + (frame_bb.Max - frame_bb.Min) / 2 - CalcTextSize(buf_display) / 2 - ImVec2(0, 1) + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(-1, -1), GetColorU32(ImGuiCol_TitleBg), buf_display);
    window->DrawList->AddText(pos + (frame_bb.Max - frame_bb.Min) / 2 - CalcTextSize(buf_display) / 2 - ImVec2(0, 1) + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(-1, 1), GetColorU32(ImGuiCol_TitleBg), buf_display);
    window->DrawList->AddText(pos + (frame_bb.Max - frame_bb.Min) / 2 - CalcTextSize(buf_display) / 2 - ImVec2(0, 1) + ImVec2(0, style.ChildBorderSize * 2) + ImVec2(1, -1), GetColorU32(ImGuiCol_TitleBg), buf_display);
    window->DrawList->AddText(pos + (frame_bb.Max - frame_bb.Min) / 2 - CalcTextSize(buf_display) / 2 - ImVec2(0, 1) + ImVec2(0, style.ChildBorderSize * 2), GetColorU32(ImGuiCol_Text), buf_display);

    return value_changed;
}

bool ImAdd::ColorEdit4(const char* label, float col[4])
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec4 col_v4(col[0], col[1], col[2], col[3]);

    ImVec2 size = CalcItemSize(ImVec2(-0.1f, g.FontSize), label_size.x + style.ItemInnerSpacing.x + g.FontSize * 2, g.FontSize);

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoSidePreview;

    BeginGroup();

    if (label_size.x > 0) {
        Text(style.Colors[ImGuiCol_Text], label);
        SameLine(size.x - g.FontSize * 2);
    }

    const ImVec2 pos = window->DC.CursorPos;
    bool result = ColorButton(label, col_v4, flags, ImVec2(g.FontSize * 2, g.FontSize));
    window->DrawList->AddRectFilledMultiColor(pos + ImVec2(1, 1), pos + ImVec2(GetFontSize() * 2, GetFontSize()) - ImVec2(1, 1), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow, 0), GetColorU32(ImGuiCol_WindowShadow), GetColorU32(ImGuiCol_WindowShadow));
    if (result)
    {
        OpenPopup(std::string(std::string(label) + "##Picker").c_str());
    }
    if (BeginPopup(std::string(std::string(label) + "##Picker").c_str()))
    {
        ColorPicker4(label, col, flags);
        EndPopup();
    }

    EndGroup();

    return result;
}