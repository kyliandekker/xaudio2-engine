#include "imgui/imgui_helpers.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <imgui/imgui_internal.h>

namespace ImGui
{
    ImVec2 operator+(ImVec2 const &a, ImVec2 const &b)
    {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    ImVec2 operator-(ImVec2 const &a, ImVec2 const &b)
    {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

    bool Knob(
        char const *label,
        float *p_value,
        float v_min,
        float v_max,
        ImVec2 const &size,
        char const *tooltip, float default_value)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO &io = GetIO();
        ImGuiStyle &style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x + 2, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetTextLineHeight();
        ImDrawList *draw_list = GetWindowDrawList();

        float ANGLE_MIN = 3.141592f * 0.70f;
        float ANGLE_MAX = 3.141592f * 2.30f;

        if (s.x != 0.0f && s.y != 0.0f)
        {
            center.x = pos.x + (s.x / 2.0f);
            center.y = pos.y + (s.y / 2.0f);
            InvisibleButton(label, ImVec2(s.x, s.y + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }
        else
        {
            InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }
        bool value_changed = false;
        bool is_active = IsItemActive();
        bool is_hovered = IsItemActive();
        bool double_clicked = IsMouseDoubleClicked(ImGuiMouseButton_Left);

        if (is_active && double_clicked)
        {
            *p_value = default_value;
            value_changed = true;
            return value_changed;
        }

        if (is_active && io.MouseDelta.y != 0.0f)
        {
            float step = (v_max - v_min) / 200.0f;
            *p_value -= io.MouseDelta.y * step;
            if (*p_value < v_min)
                *p_value = v_min;
            if (*p_value > v_max)
                *p_value = v_max;
            value_changed = true;
        }

        float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * (*p_value - v_min) / (v_max - v_min);
        float angle_cos = cosf(angle);
        float angle_sin = sinf(angle);

        draw_list->AddCircleFilled(center, radius_outer * 0.7f, GetColorU32(ImGuiCol_Button), 16);
        draw_list->PathArcTo(center, radius_outer, ANGLE_MIN, ANGLE_MAX, 16);
        draw_list->PathStroke(GetColorU32(ImGuiCol_FrameBg), false, 3.0f);
        draw_list->AddLine(
            ImVec2(center.x + angle_cos * (radius_outer * 0.35f), center.y + angle_sin * (radius_outer * 0.35f)),
            ImVec2(center.x + angle_cos * (radius_outer * 0.7f), center.y + angle_sin * (radius_outer * 0.7f)),
            GetColorU32(ImGuiCol_SliderGrabActive), 2.0f);
        draw_list->PathArcTo(center, radius_outer, ANGLE_MIN, angle + 0.02f, 16);
        draw_list->PathStroke(GetColorU32(ImGuiCol_SliderGrabActive), false, 3.0f);

        if (showLabel)
        {
            std::string lbl = std::string(label);
            std::size_t lbl_pos = lbl.find("#");
            std::string label_text = lbl.substr(0, lbl_pos);
            auto textSize = CalcTextSize(label_text.c_str());
            draw_list->AddText(ImVec2(pos.x + ((size.x / 2) - (textSize.x / 2)), pos.y + radius_outer * 2 + style.ItemInnerSpacing.y), GetColorU32(ImGuiCol_Text), label_text.c_str());
        }

        if (is_active || is_hovered)
        {
            SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - (line_height * 2) - style.ItemInnerSpacing.y - style.WindowPadding.y));
            BeginTooltip();
            if (tooltip != nullptr)
            {
                Text("%s\nValue : %.3f", tooltip, static_cast<double>(*p_value));
            }
            else if (showLabel)
            {
                Text("%s %.3f", label, static_cast<double>(*p_value));
            }
            else
            {
                Text("%.3f", static_cast<double>(*p_value));
            }
            EndTooltip();
        }

        return value_changed;
    }

    bool OnOffButton(
        char const *label,
        bool *p_value,
        ImVec2 const &size)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO &io = GetIO();
        ImGuiStyle &style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x + 2, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetTextLineHeight();
        ImDrawList *draw_list = GetWindowDrawList();

        if (s.x != 0.0f && s.y != 0.0f)
        {
            center.x = pos.x + (s.x / 2.0f);
            center.y = pos.y + (s.y / 2.0f);
            InvisibleButton(label, ImVec2(s.x, s.y + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }
        else
        {
            InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }

        bool value_changed = false;
        bool is_clicked = IsItemClicked();

        draw_list->AddCircleFilled(center, radius_outer * 0.35f, GetColorU32(ImGuiCol_FrameBg), 16);
        draw_list->AddCircleFilled(center, radius_outer * 0.3f, *p_value ? GetColorU32(ImGuiCol_Button) : GetColorU32(ImGuiCol_FrameBg), 16);

        if (is_clicked)
        {
            *p_value = !(*p_value);
            value_changed = true;
        }

        return value_changed;
    }

    bool KnobUchar(
        char const *label,
        unsigned char *p_value,
        unsigned char v_min,
        unsigned char v_max,
        ImVec2 const &size,
        char const *tooltip)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO &io = GetIO();
        ImGuiStyle &style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetFrameHeight();
        ImDrawList *draw_list = GetWindowDrawList();

        float ANGLE_MIN = 3.141592f * 0.70f;
        float ANGLE_MAX = 3.141592f * 2.30f;

        if (s.x != 0.0f && s.y != 0.0f)
        {
            center.x = pos.x + (s.x / 2.0f);
            center.y = pos.y + (s.y / 2.0f);
            InvisibleButton(label, ImVec2(s.x, s.y + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }
        else
        {
            InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
        }
        bool value_changed = false;
        bool is_active = IsItemActive();
        bool is_hovered = IsItemActive();
        if (is_active && io.MouseDelta.y != 0.0f)
        {
            int step = (v_max - v_min) / 127;
            int newVal = static_cast<int>(*p_value - io.MouseDelta.y * step);
            if (newVal < v_min)
                newVal = v_min;
            if (newVal > v_max)
                newVal = v_max;
            *p_value = static_cast<unsigned char>(newVal);
            value_changed = true;
        }

        float angle = ANGLE_MIN + (ANGLE_MAX - ANGLE_MIN) * (*p_value - v_min) / (v_max - v_min);
        float angle_cos = cosf(angle);
        float angle_sin = sinf(angle);

        draw_list->AddCircleFilled(center, radius_outer * 0.7f, GetColorU32(ImGuiCol_Button), 16);
        draw_list->PathArcTo(center, radius_outer, ANGLE_MIN, ANGLE_MAX, 16);
        draw_list->PathStroke(GetColorU32(ImGuiCol_FrameBg), false, 3.0f);
        draw_list->AddLine(
            ImVec2(center.x + angle_cos * (radius_outer * 0.35f), center.y + angle_sin * (radius_outer * 0.35f)),
            ImVec2(center.x + angle_cos * (radius_outer * 0.7f), center.y + angle_sin * (radius_outer * 0.7f)),
            GetColorU32(ImGuiCol_Text), 2.0f);
        draw_list->PathArcTo(center, radius_outer, ANGLE_MIN, angle + 0.02f, 16);
        draw_list->PathStroke(GetColorU32(ImGuiCol_SliderGrabActive), false, 3.0f);

        if (showLabel)
        {
            auto textSize = CalcTextSize(label);
            draw_list->AddText(ImVec2(pos.x + ((size.x / 2) - (textSize.x / 2)), pos.y + radius_outer * 2 + style.ItemInnerSpacing.y), GetColorU32(ImGuiCol_Text), label);
        }

        if (is_active || is_hovered)
        {
            SetNextWindowPos(ImVec2(pos.x - style.WindowPadding.x, pos.y - (line_height * 2) - style.ItemInnerSpacing.y - style.WindowPadding.y));
            BeginTooltip();
            if (tooltip != nullptr)
            {
                Text("%s\nValue : %d", tooltip, static_cast<unsigned int>(*p_value));
            }
            else if (showLabel)
            {
                Text("%s %d", label, static_cast<unsigned int>(*p_value));
            }
            else
            {
                Text("%d", static_cast<unsigned int>(*p_value));
            }
            EndTooltip();
        }
        else if (IsItemHovered() && tooltip != nullptr)
        {
            ShowTooltipOnHover(tooltip);
        }

        return value_changed;
    }

    void ShowTooltipOnHover(
        char const *tooltip)
    {
        if (IsItemHovered())
        {
            BeginTooltip();
            Text("%s", tooltip);
            EndTooltip();
        }
    }

    bool DropDown(
        char const *label,
        unsigned char &value,
        char const *const names[],
        unsigned int nameCount,
        char const *tooltip)
    {
        bool value_changed = false;

        auto current_effect_item = names[value];
        if (BeginCombo(label, current_effect_item, ImGuiComboFlags_HeightLarge))
        {
            for (unsigned char n = 0; n < nameCount; n++)
            {
                bool is_selected = (current_effect_item == names[n]);
                if (Selectable(names[n], is_selected))
                {
                    current_effect_item = names[n];
                    value = n;
                    value_changed = true;
                }
            }

            EndCombo();
        }

        ShowTooltipOnHover(tooltip == nullptr ? label : tooltip);

        return value_changed;
    }

    void UvMeter(char const *label, ImVec2 const &size, int *value, int v_min, int v_max)
    {
        ImDrawList *draw_list = GetWindowDrawList();

        ImVec2 pos = GetCursorScreenPos();

        InvisibleButton(label, size);

        float stepHeight = (v_max - v_min + 1) / size.y;
        auto y = pos.y + size.y;
        auto hue = 0.4f;
        auto sat = 0.6f;
        for (int i = v_min; i <= v_max; i += 5)
        {
            hue = 0.4f - (static_cast<float>(i) / static_cast<float>(v_max - v_min)) / 2.0f;
            sat = (*value < i ? 0.0f : 0.6f);
            draw_list->AddRectFilled(ImVec2(pos.x, y), ImVec2(pos.x + size.x, y - (stepHeight * 4)), static_cast<ImU32>(ImColor::HSV(hue, sat, 0.6f)));
            y = pos.y + size.y - (i * stepHeight);
        }
    }

    void TextBox(char const *label, ImVec2 const &size)
    {
        ImDrawList *draw_list = GetWindowDrawList();

        ImVec2 pos = GetCursorScreenPos();

        draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, ImColor(255, 255, 0, 255), "Hello World", 0, 0.0f, 0);
    }

    bool CheckboxButton(const char *label, bool *p_value, const ImVec2 &size_arg)
    {
        ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        ImVec4 color_inactive = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
        if (*p_value)
            ImGui::PushStyleColor(ImGuiCol_Button, color);
        else
            ImGui::PushStyleColor(ImGuiCol_Button, color_inactive);
        bool b = ImGui::Button(label, size_arg);
        if (b)
            *p_value = !(*p_value);
        ImGui::PopStyleColor();

        return b;
    }

    float RoundScalarWithFormatFloat(const char* format, ImGuiDataType data_type, float v)
    {
        return RoundScalarWithFormatT<float, float>(format, data_type, v);
    }

    float SliderCalcRatioFromValueFloat(ImGuiDataType data_type, float v, float v_min, float v_max, float power, float linear_zero_pos)
    {
        return ScaleRatioFromValueT<float, float, float>(data_type, v, v_min, v_max, false, power, linear_zero_pos);
    }

    // ~80% common code with ImGui::SliderBehavior
    bool RangeSliderBehavior(const ImRect &frame_bb, ImGuiID id, float *v1, float *v2, float v_min, float v_max, float power, int decimal_precision, ImGuiSliderFlags flags)
    {
        ImGuiContext &g = *GImGui;
        ImGuiWindow *window = GetCurrentWindow();
        const ImGuiStyle &style = g.Style;

        // Draw frame
        RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

        const bool is_non_linear = (power < 1.0f - 0.00001f) || (power > 1.0f + 0.00001f);
        const bool is_horizontal = (flags & ImGuiSliderFlags_Vertical) == 0;

        const float grab_padding = 2.0f;
        const float slider_sz = is_horizontal ? (frame_bb.GetWidth() - grab_padding * 2.0f) : (frame_bb.GetHeight() - grab_padding * 2.0f);
        float grab_sz;
        if (decimal_precision > 0)
            grab_sz = ImMin(style.GrabMinSize, slider_sz);
        else
            grab_sz = ImMin(ImMax(1.0f * (slider_sz / ((v_min < v_max ? v_max - v_min : v_min - v_max) + 1.0f)), style.GrabMinSize), slider_sz); // Integer sliders, if possible have the grab size represent 1 unit
        const float slider_usable_sz = slider_sz - grab_sz;
        const float slider_usable_pos_min = (is_horizontal ? frame_bb.Min.x : frame_bb.Min.y) + grab_padding + grab_sz * 0.5f;
        const float slider_usable_pos_max = (is_horizontal ? frame_bb.Max.x : frame_bb.Max.y) - grab_padding - grab_sz * 0.5f;

        // For logarithmic sliders that cross over sign boundary we want the exponential increase to be symmetric around 0.0f
        float linear_zero_pos = 0.0f; // 0.0->1.0f
        if (v_min * v_max < 0.0f)
        {
            // Different sign
            const float linear_dist_min_to_0 = powf(fabsf(0.0f - v_min), 1.0f / power);
            const float linear_dist_max_to_0 = powf(fabsf(v_max - 0.0f), 1.0f / power);
            linear_zero_pos = linear_dist_min_to_0 / (linear_dist_min_to_0 + linear_dist_max_to_0);
        }
        else
        {
            // Same sign
            linear_zero_pos = v_min < 0.0f ? 1.0f : 0.0f;
        }

        // Process clicking on the slider
        bool value_changed = false;
        if (g.ActiveId == id)
        {
            if (g.IO.MouseDown[0])
            {
                const float mouse_abs_pos = is_horizontal ? g.IO.MousePos.x : g.IO.MousePos.y;
                float clicked_t = (slider_usable_sz > 0.0f) ? ImClamp((mouse_abs_pos - slider_usable_pos_min) / slider_usable_sz, 0.0f, 1.0f) : 0.0f;
                if (!is_horizontal)
                    clicked_t = 1.0f - clicked_t;

                float new_value;
                if (is_non_linear)
                {
                    // Account for logarithmic scale on both sides of the zero
                    if (clicked_t < linear_zero_pos)
                    {
                        // Negative: rescale to the negative range before powering
                        float a = 1.0f - (clicked_t / linear_zero_pos);
                        a = powf(a, power);
                        new_value = ImLerp(ImMin(v_max, 0.0f), v_min, a);
                    }
                    else
                    {
                        // Positive: rescale to the positive range before powering
                        float a;
                        if (fabsf(linear_zero_pos - 1.0f) > 1.e-6f)
                            a = (clicked_t - linear_zero_pos) / (1.0f - linear_zero_pos);
                        else
                            a = clicked_t;
                        a = powf(a, power);
                        new_value = ImLerp(ImMax(v_min, 0.0f), v_max, a);
                    }
                }
                else
                {
                    // Linear slider
                    new_value = ImLerp(v_min, v_max, clicked_t);
                }

                char fmt[64];
                snprintf(fmt, 64, "%%.%df", decimal_precision);

                // Round past decimal precision
                new_value = RoundScalarWithFormatFloat(fmt, ImGuiDataType_Float, new_value);
                if (*v1 != new_value || *v2 != new_value)
                {
                    if (fabsf(*v1 - new_value) < fabsf(*v2 - new_value))
                    {
                        *v1 = new_value;
                    }
                    else
                    {
                        *v2 = new_value;
                    }
                    value_changed = true;
                }
            }
            else
            {
                ClearActiveID();
            }
        }

        // Calculate slider grab positioning
        float grab_t = SliderCalcRatioFromValueFloat(ImGuiDataType_Float, *v1, v_min, v_max, power, linear_zero_pos);

        // Draw
        if (!is_horizontal)
            grab_t = 1.0f - grab_t;
        float grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        ImRect grab_bb1;
        if (is_horizontal)
            grab_bb1 = ImRect(ImVec2(grab_pos - grab_sz * 0.5f, frame_bb.Min.y + grab_padding), ImVec2(grab_pos + grab_sz * 0.5f, frame_bb.Max.y - grab_padding));
        else
            grab_bb1 = ImRect(ImVec2(frame_bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f), ImVec2(frame_bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f));
        window->DrawList->AddRectFilled(grab_bb1.Min, grab_bb1.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        // Calculate slider grab positioning
        grab_t = SliderCalcRatioFromValueFloat(ImGuiDataType_Float, *v2, v_min, v_max, power, linear_zero_pos);

        // Draw
        if (!is_horizontal)
            grab_t = 1.0f - grab_t;
        grab_pos = ImLerp(slider_usable_pos_min, slider_usable_pos_max, grab_t);
        ImRect grab_bb2;
        if (is_horizontal)
            grab_bb2 = ImRect(ImVec2(grab_pos - grab_sz * 0.5f, frame_bb.Min.y + grab_padding), ImVec2(grab_pos + grab_sz * 0.5f, frame_bb.Max.y - grab_padding));
        else
            grab_bb2 = ImRect(ImVec2(frame_bb.Min.x + grab_padding, grab_pos - grab_sz * 0.5f), ImVec2(frame_bb.Max.x - grab_padding, grab_pos + grab_sz * 0.5f));
        window->DrawList->AddRectFilled(grab_bb2.Min, grab_bb2.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        ImRect connector(grab_bb1.Min, grab_bb2.Max);
        connector.Min.x += grab_sz;
        connector.Min.y += grab_sz * 0.3f;
        connector.Max.x -= grab_sz;
        connector.Max.y -= grab_sz * 0.3f;

        window->DrawList->AddRectFilled(connector.Min, connector.Max, GetColorU32(ImGuiCol_SliderGrab), style.GrabRounding);

        return value_changed;
    }

    // ~95% common code with ImGui::SliderFloat
    bool RangeSliderFloat(const char *label, float *v1, float *v2, float v_min, float v_max, const char *display_format, float power)
    {
        ImGuiWindow *window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext &g = *GImGui;
        const ImGuiStyle &style = g.Style;
        const ImGuiID id = window->GetID(label);
        const float w = CalcItemWidth();

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        // NB- we don't call ItemSize() yet because we may turn into a text edit box below
        if (!ItemAdd(total_bb, id))
        {
            ItemSize(total_bb, style.FramePadding.y);
            return false;
        }

        const bool hovered = ItemHoverable(frame_bb, id);
        if (hovered)
            SetHoveredID(id);

        if (!display_format)
            display_format = "(%.3f, %.3f)";
        int decimal_precision = ImParseFormatPrecision(display_format, 3);

        // Tabbing or CTRL-clicking on Slider turns it into an input box
        bool start_text_input = false;
        const bool tab_focus_requested = (GetItemStatusFlags() & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]))
        {
            SetActiveID(id, window);
            FocusWindow(window);

            if (tab_focus_requested || g.IO.KeyCtrl)
            {
                start_text_input = true;
                g.TempInputId = 0;
            }
        }

        if (start_text_input || (g.ActiveId == id && g.TempInputId == id))
        {
            char fmt[64];
            snprintf(fmt, 64, "%%.%df", decimal_precision);
            return TempInputScalar(frame_bb, id, label, ImGuiDataType_Float, v1, fmt);
        }

        ItemSize(total_bb, style.FramePadding.y);

        // Actual slider behavior + render grab
        const bool value_changed = RangeSliderBehavior(frame_bb, id, v1, v2, v_min, v_max, power, decimal_precision, 0);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        char value_buf[64];
        const char *value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v1, *v2);
        RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        return value_changed;
    }
}