#include <iostream>

#include "AudioSDLWindow.h"

int main(int, char*[])
{
	AudioSystem s;
	Handle h = s.CreateSound("C:/Users/Kylian/SynologyDrive/HQ Tracks/2 Brothers on the 4th Floor - Fairytales (Re-Style Remix) (Extended Mix).wav");
	s.Play(h);
	while (true)
	{
		s.Update();
	}

	return 0;
}