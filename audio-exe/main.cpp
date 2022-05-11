#include <string>
#include <iostream>
#include <filesystem>

#include <uaudio/utils/Logger.h>
#include <uaudio/wave/low_level/WaveFormat.h>

constexpr bool DEBUG_MODE = true;

/*
 * 1st argument: working directory.
 */
int main(int argc, char** argv)
{
	if (argc == 1 && !DEBUG_MODE)
	{
		uaudio::logger::log_error("<ResourceMover> Cannot move resources. Did you forget to pass the arguments?\n");
		return 0;
	}

	std::string resource_path = DEBUG_MODE ? "C:/test/" : argv[1];
	std::string dest_path = resource_path + "converted/";

	std::filesystem::create_directory(dest_path);
	for (const auto& entry : std::filesystem::directory_iterator(resource_path))
	{
		uaudio::WaveFormat format;
		FILE* file = nullptr;
		uaudio::WaveReader::LoadSound(entry.path().string().c_str(), format, file);

		std::filesystem::path p(entry.path());
		uaudio::WaveReader::SaveSound(std::string(dest_path + p.filename().string().c_str()).c_str(), format);
	}
	return 0;
}