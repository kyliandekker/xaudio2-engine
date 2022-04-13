#pragma once
#include <cstdint>

class ImGuiElement
{
public:
	virtual void RenderElement(uint32_t a_Index) = 0;
	float GetRGBColor(int color);
	void ShowValue(const char* a_Text, const char* a_Value);
};
