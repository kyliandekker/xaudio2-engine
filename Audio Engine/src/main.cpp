#include <iostream>

#include "AudioSDLWindow.h"

int main(int, char*[])
{
	AudioSDLWindow* audioSDLWindow = new AudioSDLWindow();
	audioSDLWindow->RenderWindow();
	delete audioSDLWindow;

	while(true)
	{
		
	}

	return 0;
}