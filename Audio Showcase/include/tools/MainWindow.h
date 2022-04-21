#pragma once

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"
#include "imgui/imgui_helpers.h"

class MainWindow : public BaseTool
{
public:
	MainWindow(std::vector<BaseTool*>& a_Tools);

	std::map<std::string, std::vector<BaseTool*>> SortByCategory() const;
	void Render() override;
private:
	std::vector<BaseTool*>& m_Tools;
};
