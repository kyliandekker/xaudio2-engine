#include <iostream>

#include "AudioSDLWindow.h"

int main(int, char*[])
{
	AudioSDLWindow audioSDLWindow = AudioSDLWindow();

	audioSDLWindow.CreateWindow();
	audioSDLWindow.RenderWindow();
	audioSDLWindow.DeleteWindow();

	return 0;
}