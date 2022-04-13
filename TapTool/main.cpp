#include "include/AudioSDLWindow.h"

int main(int, char* [])
{
	AudioSystem test;
	AudioSDLWindow* audioSDLWindow = new AudioSDLWindow();
	audioSDLWindow->RenderWindow();
	delete audioSDLWindow;
	return 0;
}
