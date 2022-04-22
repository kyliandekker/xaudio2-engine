#include <iostream>

#include "AudioSDLWindow.h"

#include "tools/ChannelsTool.h"
#include "tools/MasterTool.h"
#include "tools/SoundsTool.h"
#include <uaudio/utils/Logger.h>

int main(int, char *[])
{
	uaudio::AudioSystem aSys(AUDIO_MODE::AUDIO_MODE_THREADED);
	aSys.Start();
	uaudio::SoundSystem sSys;
	AudioSDLWindow audioSDLWindow = AudioSDLWindow(aSys, sSys);

	ChannelsTool channelsTool = ChannelsTool(aSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&channelsTool);
	SoundsTool soundsTool = SoundsTool(aSys, sSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&soundsTool);
	MasterTool masterTool = MasterTool(aSys, sSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&masterTool);

	// Render loop.
	audioSDLWindow.RenderWindow();

	// Stop
	aSys.Stop();

	return 0;
}
