#include "tools/BaseTool.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

BaseTool::BaseTool(ImGuiWindowFlags a_Flags, std::string a_Name, std::string a_Category, bool a_FullScreen) : m_Flags(a_Flags), m_Name(a_Name), m_Category(a_Category), m_FullScreen(a_FullScreen)
{ }

BaseTool::~BaseTool() = default;

float BaseTool::GetRGBColor(int color)
{
    return 1.0f / 255.0f * static_cast<float>(color);
}

void BaseTool::ShowValue(const char* a_Text, const char* a_Value)
{
	const ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    ImGui::Text("%s\n", a_Text);
    ImGui::SameLine();
    ImGui::TextColored(color, "%s\n", a_Value);
}

void BaseTool::WindowBegin()
{
    ImGui::Begin(m_Name.c_str(), 0, m_Flags);
}

void BaseTool::WindowEnd()
{
    ImGui::End();
}

void BaseTool::Update()
{
    if (!m_Enabled)
        return;

    if (m_FullScreen)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(m_Size.x, m_Size.y));
    }

    WindowBegin();

    Render();

    WindowEnd();
}

void BaseTool::SetSize(ImVec2 a_Size)
{
    m_Size = a_Size;
}

bool BaseTool::IsFullScreen() const
{
    return m_FullScreen;
}

void BaseTool::SetEnabled(bool a_Enabled)
{
    m_Enabled = a_Enabled;
}

void BaseTool::ToggleEnabled()
{
    m_Enabled = !m_Enabled;
}

std::string BaseTool::GetName() const
{
    return m_Name;
}
bool BaseTool::IsEnabled() const
{
    return m_Enabled;
}
std::string BaseTool::GetCategory() const
{
    return m_Category;
}