#include "tools/MainWindow.h"
#include <array>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

MainWindow::MainWindow(std::vector<BaseTool*>&a_Tools) : BaseTool(ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking, "DockSpace", "", true), m_Tools(a_Tools)
{ }

std::map<std::string, std::vector<BaseTool*>> MainWindow::SortByCategory()
{
    std::map<std::string, std::vector<BaseTool*>> sorted_tools;
    for (auto* tool : m_Tools)
    {
        if (sorted_tools.find(tool->GetName()) == sorted_tools.end())
            sorted_tools.insert(std::make_pair(tool->GetCategory(), std::vector<BaseTool*>()));
        sorted_tools[tool->GetCategory()].emplace_back(tool);
    }
    return sorted_tools;
}

void MainWindow::Render()
{
    ImGui::DockSpace(ImGui::GetID("DockSpace"));
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::BeginMenu("Hide/Show"))
        {
            auto map = SortByCategory();
            for (auto& category : map)
            {
                for (const auto& tool : category.second)
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);
                    std::string name = category.first.empty() ? "Uncategorized" : category.first.c_str();
                    if (ImGui::BeginMenu(name.c_str()))
                    {
                        if (ImGui::MenuItem(tool->GetName().c_str(), "", tool->IsEnabled()))
                            tool->ToggleEnabled();
                        ImGui::EndMenu();
                    }
                    ImGui::PopItemFlag();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}