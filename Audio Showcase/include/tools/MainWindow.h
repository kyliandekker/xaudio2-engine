#pragma once

#include "AudioSystem.h"
#include "SoundSystem.h"
#include "tools/BaseTool.h"

class MainWindow : public BaseTool
{
public:
	MainWindow(std::vector<BaseTool*>& a_Tools);

	std::map<std::string, std::vector<BaseTool*>> SortByCategory();
	void Render() override;
private:
	std::vector<BaseTool*>& m_Tools;
};
