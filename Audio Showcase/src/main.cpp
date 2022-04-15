#include <iostream>

#include "AudioSDLWindow.h"

int main(int, char* [])
{
	uaudio::AudioSystem aSys;
	uaudio::SoundSystem sSys;
	AudioSDLWindow* audioSDLWindow = new AudioSDLWindow(&aSys, &sSys);
	audioSDLWindow->RenderWindow();
	delete audioSDLWindow;

	return 0;
}