#include <iostream>

#include "AudioSDLWindow.h"

int main(int, char* [])
{
	uaudio::AudioSystem aSys;
	aSys.Start();
	uaudio::SoundSystem sSys;
	AudioSDLWindow audioSDLWindow = AudioSDLWindow(aSys, sSys);
	audioSDLWindow.RenderWindow();
	aSys.Stop();

	return 0;
}