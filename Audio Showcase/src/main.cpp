#include <iostream>

#include "AudioSDLWindow.h"

#include "tools/ChannelsTool.h"
#include "tools/MainWindow.h"
#include "tools/MasterTool.h"
#include "tools/SoundsTool.h"
#include "utils/Logger.h"

int main(int, char* [])
{
	FILE* file;

	// Open the file.
	const char* a_FilePath = "C:/test/test.wav";
	fopen_s(&file, a_FilePath, "wb");
	if (file == nullptr)
	{
		uaudio::logger::log_warning("<WaveReader> Failed saving file: (\"%s\").", a_FilePath);
		return 1;
	}

	uaudio::FMT_Chunk chunk;
	memcpy(chunk.chunkId, "fmt ", sizeof(chunk.chunkId));
	chunk.chunkSize = 16;
	chunk.audioFormat = 1;
	chunk.numChannels = 2;
	chunk.sampleRate = 44100;
	chunk.byteRate = 176400;
	chunk.blockAlign = 4;
	chunk.bitsPerSample = 16;

	chunk.Write(file);

	fclose(file);
	file = nullptr;

	const char* a_FilePath2 = "C:/test/test2.wav";
	fopen_s(&file, a_FilePath2, "wb");
	if (file == nullptr)
	{
		uaudio::logger::log_warning("<WaveReader> Failed saving file: (\"%s\").", a_FilePath2);
		return 1;
	}

	fwrite(reinterpret_cast<char*>(&chunk), sizeof(chunk), 1, file);

	fclose(file);
	file = nullptr;

	uaudio::AudioSystem aSys;
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
