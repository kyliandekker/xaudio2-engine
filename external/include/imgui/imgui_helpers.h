#ifndef IMGUI_HELPERS_H
#define IMGUI_HELPERS_H

#include <imgui/imgui.h>

namespace ImGui
{
    ImVec2 operator+(ImVec2 const &a, ImVec2 const &b);
    ImVec2 operator-(ImVec2 const &a, ImVec2 const &b);

    bool Knob(
        char const *label,
        float *p_value,
        float v_min,
        float v_max,
        ImVec2 const &size,
        char const *tooltip, float default_value);

    bool OnOffButton(
        char const *label,
        bool *p_value,
        ImVec2 const &size);

    bool KnobUchar(
        char const *label,
        unsigned char *p_value,
        unsigned char v_min,
        unsigned char v_max,
        ImVec2 const &size,
        char const *tooltip);

    void ShowTooltipOnHover(
        char const *tooltip);

    bool DropDown(
        char const *label,
        unsigned char &value,
        char const *const names[],
        unsigned int nameCount,
        char const *tooltip);

    void UvMeter(char const *label, ImVec2 const &size, int *value, int v_min, int v_max);

    void TextBox(
        char const *label,
        ImVec2 const &size);

    bool CheckboxButton(const char *label, bool *p_value, const ImVec2 &size_arg = ImVec2(0, 0));

	bool RangeSliderFloat(const char *label, float *v1, float *v2, float v_min, float v_max, const char *display_format = "(%.3f, %.3f)", float power = 1.0f);

#endif // IMGUI_HELPERS_H
}