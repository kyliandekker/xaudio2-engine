#include "ImGuiElement.h"

#include <imgui/imgui.h>

float ImGuiElement::GetRGBColor(int color)
{
    return 1.0f / 255.0f * static_cast<float>(color);
}

void ImGuiElement::ShowValue(const char* a_Text, const char* a_Value)
{
    const ImVec4 color = ImVec4(GetRGBColor(61), GetRGBColor(133), GetRGBColor(224), 1.0f);
    ImGui::Text("%s\n", a_Text);
    ImGui::SameLine();
    ImGui::TextColored(color, "%s\n", a_Value);
}
