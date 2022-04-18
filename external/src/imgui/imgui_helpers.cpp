#include "imgui/imgui_helpers.h"

#include <cmath>
#include <cstdio>
#include <string>

namespace ImGui
{
    ImVec2 operator+(ImVec2 const& a, ImVec2 const& b)
    {
        return ImVec2(a.x + b.x, a.y + b.y);
    }

    ImVec2 operator-(ImVec2 const& a, ImVec2 const& b)
    {
        return ImVec2(a.x - b.x, a.y - b.y);
    }

    bool Knob(
        char const* label,
        float* p_value,
        float v_min,
        float v_max,
        ImVec2 const& size,
        char const* tooltip, float default_value)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO& io = GetIO();
        ImGuiStyle& style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x + 2, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetTextLineHeight();
        ImDrawList* draw_list = GetWindowDrawList();

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
        char const* label,
        bool* p_value,
        ImVec2 const& size)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO& io = GetIO();
        ImGuiStyle& style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x + 2, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetTextLineHeight();
        ImDrawList* draw_list = GetWindowDrawList();

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
        char const* label,
        unsigned char* p_value,
        unsigned char v_min,
        unsigned char v_max,
        ImVec2 const& size,
        char const* tooltip)
    {
        bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

        ImGuiIO& io = GetIO();
        ImGuiStyle& style = GetStyle();
        ImVec2 s(size.x - 4, size.y - 4);

        float radius_outer = std::fmin(s.x, s.y) / 2.0f;
        ImVec2 pos = GetCursorScreenPos();
        pos = ImVec2(pos.x, pos.y + 2);
        ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

        float line_height = GetFrameHeight();
        ImDrawList* draw_list = GetWindowDrawList();

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
        char const* tooltip)
    {
        if (IsItemHovered())
        {
            BeginTooltip();
            Text("%s", tooltip);
            EndTooltip();
        }
    }

    bool DropDown(
        char const* label,
        unsigned char& value,
        char const* const names[],
        unsigned int nameCount,
        char const* tooltip)
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

    void UvMeter(char const* label, ImVec2 const& size, int* value, int v_min, int v_max)
    {
        ImDrawList* draw_list = GetWindowDrawList();

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


    void TextBox(char const* label, ImVec2 const& size)
    {
        ImDrawList* draw_list = GetWindowDrawList();

        ImVec2 pos = GetCursorScreenPos();

        draw_list->AddText(ImGui::GetFont(), ImGui::GetFontSize(), pos, ImColor(255, 255, 0, 255), "Hello World", 0, 0.0f, 0);
    }

    bool CheckboxButton(const char* label, const ImVec2& size_arg, bool* p_value)
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
}