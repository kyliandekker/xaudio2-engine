#include <uaudio/wave/low_level/WaveReader.h>

#include <uaudio/utils/Logger.h>
#include <uaudio/utils/Utils.h>
#include <uaudio/wave/low_level/WaveFormat.h>

#include "uaudio/wave/high_level/WaveChunks.h"

namespace uaudio
{
	/// <summary>
	/// Loads the sound.
	/// </summary>
	/// <param name="a_FilePath">The path to the file.</param>
	/// <param name="a_WaveFormat">The wave format.</param>
	/// <param name="a_WaveConfig">The config containing specific loading instructions.</param>
	/// <returns>WAVE loading status.</returns>
	UAUDIO_RESULT WaveReader::LoadSound(const char *a_FilePath, WaveFormat &a_WaveFormat, WaveConfig a_WaveConfig)
	{
		FILE* file = nullptr;

		// Empty the wave format.
		a_WaveFormat = WaveFormat();

		a_WaveFormat.m_FilePath = a_FilePath;

		// Check if the file that has been passed is opened in some way.
		if (file != nullptr)
		{
			fclose(file);
			file = nullptr;
		}

		// Open the file.
		fopen_s(&file, a_FilePath, "rb");
		if (file == nullptr)
		{
			logger::log_warning("<WaveReader> Failed opening file: (%s\"%s%s\").", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
			return UAUDIO_RESULT::UAUDIO_ERR_FILE_NOTFOUND;
		}

		fseek(file, 0, SEEK_END);
		long total_size = ftell(file);
		rewind(file);

		logger::log_info("<WaveReader> Reading wave file: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);

		char previous_chunk_id[4] = {};
		long previous_tell = 0;

		// Check if file has reached eof.
		while (total_size != ftell(file))
		{
			char chunk_id[CHUNK_ID_SIZE] = {};

			// Read what chunk id the file has.
			fread(&chunk_id, sizeof(chunk_id), 1, file);

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (previous_tell == ftell(file) && strncmp(&chunk_id[0], &previous_chunk_id[0], CHUNK_ID_SIZE) == 0)
			{
				logger::log_info(R"(<WaveReader> Got stuck on %s"%.4s"%s and %s"%.4s"%s chunks.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, previous_chunk_id, logger::COLOR_WHITE);
				logger::log_warning("<WaveReader> Failed to load wave file (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
				return UAUDIO_RESULT::UAUDIO_ERR_FILE_BAD;
			}
			previous_tell = ftell(file);

			UAUDIO_DEFAULT_MEMCPY(previous_chunk_id, chunk_id, sizeof(chunk_id));

			uint32_t chunk_size = 0;

			// Check chunk size (chunk size).
			fread(&chunk_size, sizeof(chunk_size), 1, file);

			if (strncmp(&chunk_id[0], RIFF_CHUNK_ID, CHUNK_ID_SIZE) == 0)
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk.)", logger::COLOR_YELLOW, RIFF_CHUNK_ID, logger::COLOR_WHITE);

				fseek(file, 4, SEEK_CUR);
				continue;
			}

			bool get_chunk = false;
			if (a_WaveConfig.chunksToLoad.empty())
				get_chunk = true;
			else
			{
				// Check if the chunk is in the config.
				for (const auto& chunk_name : a_WaveConfig.chunksToLoad)
					if (strncmp(&chunk_id[0], chunk_name, CHUNK_ID_SIZE) == 0)
						get_chunk = true;
			}

			if (get_chunk)
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);

				WaveChunkData *chunk_data = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(chunk_size + sizeof(WaveChunkData)));

				if (chunk_data != nullptr)
				{
					UAUDIO_DEFAULT_MEMCPY(chunk_data->chunk_id, chunk_id, sizeof(chunk_id));
					chunk_data->chunkSize = chunk_size;
					fread(utils::add(chunk_data, sizeof(WaveChunkData)), 1, chunk_size, file);
				}
				else
					fseek(file, static_cast<long>(chunk_size), SEEK_CUR);

				a_WaveFormat.AddChunk(chunk_data);
			}
			else
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s that is not in config.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);
				fseek(file, static_cast<long>(chunk_size), SEEK_CUR);
			}
		}

		a_WaveFormat.ConfigConversion(a_WaveConfig);
		a_WaveFormat.m_HasLoaded = true;

		fclose(file);
		file = nullptr;

		logger::log_success("<WaveReader> Opened file successfully: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Saves a file with the chunks inside the wave format.
	/// </summary>
	/// <param name="a_FilePath">The path to save to.</param>
	/// <param name="a_WaveFormat">The format with all the chunks.</param>
	/// <returns>WAVE saving status.</returns>
	UAUDIO_RESULT WaveReader::SaveSound(const char *a_FilePath, WaveFormat &a_WaveFormat)
	{
		FILE *file;

		// Open the file.
		fopen_s(&file, a_FilePath, "wb");
		if (file == nullptr)
		{
			logger::log_warning("<WaveReader> Failed saving file: (%s\"%s%s\").", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
			return UAUDIO_RESULT::UAUDIO_ERR_FILE_BAD;
		}

		WaveChunkData* chunk_data = nullptr;
		RIFF_Chunk riff_chunk;
		UAUDIO_DEFAULT_MEMCPY(riff_chunk.format, RIFF_CHUNK_ID, CHUNK_ID_SIZE);
		WaveFormat::ConstructChunk(chunk_data, RIFF_CHUNK_ID, riff_chunk);

		uint32_t chunk_size = 0;
		for (auto *m_Chunk : a_WaveFormat.m_Chunks)
			chunk_size += m_Chunk->chunkSize + sizeof(WaveChunkData);

		fwrite(reinterpret_cast<char*>(chunk_data), sizeof(WaveChunkData) + sizeof(RIFF_Chunk), 1, file);
		logger::log_info(R"(<WaveReader> Saved chunk %s"%.4s"%s with size %s"%i"%s to file: (%s"%s%s").)", logger::COLOR_YELLOW, RIFF_CHUNK_ID, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_data->chunkSize, logger::COLOR_WHITE, logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);

		for (auto *m_Chunk : a_WaveFormat.m_Chunks)
		{
			fwrite(reinterpret_cast<char*>(m_Chunk), sizeof(WaveChunkData) + m_Chunk->chunkSize, 1, file);
			logger::log_info(R"(<WaveReader> Saved chunk %s"%.4s"%s with size %s"%i"%s to file: (%s"%s%s").)", logger::COLOR_YELLOW, m_Chunk->chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, m_Chunk->chunkSize, logger::COLOR_WHITE, logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		}

		fclose(file);
		file = nullptr;
		logger::log_success(R"(<WaveReader> Saved file: (%s"%s%s").)", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return UAUDIO_RESULT::UAUDIO_OK;
	}
}
