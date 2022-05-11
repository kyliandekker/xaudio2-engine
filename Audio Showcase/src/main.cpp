#include <iostream>

#include "AudioSDLWindow.h"

#include "tools/ChannelsTool.h"
#include "tools/MasterTool.h"
#include "tools/SoundsTool.h"
#include <uaudio/utils/Logger.h>

#include <uaudio/system/high_level/AudioSystem.h>
#include <uaudio/system/high_level/SoundSystem.h>

uaudio::SoundSystem uaudio::SoundSys;

int main(int, char *[])
{
	uaudio::AudioSystem aSys;
	AudioSDLWindow audioSDLWindow = AudioSDLWindow(aSys);

	ChannelsTool channelsTool = ChannelsTool(aSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&channelsTool);
	SoundsTool soundsTool = SoundsTool(aSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&soundsTool);
	MasterTool masterTool = MasterTool(aSys);
	audioSDLWindow.GetImGuiWindow().AddTool(&masterTool);

	// Render loop.
	audioSDLWindow.RenderWindow();

	return 0;
}
