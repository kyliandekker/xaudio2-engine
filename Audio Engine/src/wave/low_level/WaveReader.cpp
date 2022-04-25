#include <uaudio/wave/low_level/WaveReader.h>

#include <uaudio/utils/Logger.h>
#include <uaudio/utils/Utils.h>
#include <uaudio/wave/low_level/WaveFormat.h>

namespace uaudio
{
	/// <summary>
	/// Loads the sound.
	/// </summary>
	/// <param name="a_FilePath">The path to the file.</param>
	/// <param name="a_WaveFormat">The wave format.</param>
	/// <param name="a_File">The pointer to the file.</param>
	/// <param name="a_WaveConfig">The config containing specific loading instructions.</param>
	/// <returns>WAVE loading status.</returns>
	UAUDIO_RESULT WaveReader::LoadSound(const char *a_FilePath, WaveFormat &a_WaveFormat, FILE *&a_File, WaveConfig a_WaveConfig)
	{
		// Ensure these chunks are always there.
		a_WaveConfig.chunksToLoad.push_back(DATA_CHUNK_ID);
		a_WaveConfig.chunksToLoad.push_back(FMT_CHUNK_ID);

		a_WaveFormat.SetFileName(a_FilePath);

		// Check if the file that has been passed is opened in some way.
		if (a_File != nullptr)
		{
			fclose(a_File);
			a_File = nullptr;
		}

		// Open the file.
		fopen_s(&a_File, a_FilePath, "rb");
		if (a_File == nullptr)
		{
			logger::log_warning("<WaveReader> Failed opening file: (%s\"%s%s\").", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
			return UAUDIO_RESULT::UAUDIO_ERR_FILE_NOTFOUND;
		}

		fseek(a_File, 0, SEEK_END);
		long total_size = ftell(a_File);
		rewind(a_File);

		logger::log_info("<WaveReader> Reading wave file: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);

		char previous_chunk_id[4] = {};

		// Check if file has reached eof.
		while (total_size != ftell(a_File))
		{
			char chunk_id[CHUNK_ID_SIZE] = {};

			// Read what chunk id the a_File has.
			fread(&chunk_id, sizeof(chunk_id), 1, a_File);

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (strncmp(&chunk_id[0], &previous_chunk_id[0], CHUNK_ID_SIZE) == 0)
			{
				logger::log_warning("<WaveReader> Failed to load wave a_File (\"%s\").", a_FilePath);
				return UAUDIO_RESULT::UAUDIO_ERR_FILE_BAD;
			}

			UAUDIO_DEFAULT_MEMCPY(previous_chunk_id, chunk_id, sizeof(chunk_id));

			uint32_t chunk_size = 0;

			// Check chunk size (chunk size).
			fread(&chunk_size, sizeof(chunk_size), 1, a_File);

			if (strncmp(&chunk_id[0], "RIFF", CHUNK_ID_SIZE) == 0)
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk.)", logger::COLOR_YELLOW, RIFF_CHUNK_ID, logger::COLOR_WHITE);

				fseek(a_File, 4, SEEK_CUR);
				continue;
			}

			bool get_chunk = false;
			// Check if the chunk is in the config.
			for (const auto &chunk_name : a_WaveConfig.chunksToLoad)
				if (strncmp(&chunk_id[0], chunk_name, CHUNK_ID_SIZE) == 0)
					get_chunk = true;

			if (get_chunk)
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);

				WaveChunkData *chunk_data = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(chunk_size + sizeof(WaveChunkData)));

				if (chunk_data != nullptr)
				{
					UAUDIO_DEFAULT_MEMCPY(chunk_data->chunk_id, chunk_id, sizeof(chunk_id));
					chunk_data->chunkSize = chunk_size;
					fread(utils::add(chunk_data, sizeof(WaveChunkData)), 1, chunk_size, a_File);
				}
				else
					fseek(a_File, static_cast<long>(chunk_size), SEEK_CUR);

				a_WaveFormat.AddChunk(chunk_data);
			}
			else
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s that is not in config.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);
				fseek(a_File, static_cast<long>(chunk_size), SEEK_CUR);
			}
		}

		a_WaveFormat.ConfigConversion(a_WaveConfig);

		fclose(a_File);
		a_File = nullptr;

		logger::log_success("<WaveReader> Opened file successfully: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return UAUDIO_RESULT::UAUDIO_OK;
	}

	/// <summary>
	/// Saves a file with the chunks inside the wave format.
	/// </summary>
	/// <param name="a_FilePath">The path to save to.</param>
	/// <param name="a_WaveFormat">The format with all the chunks.</param>
	/// <returns>WAVE saving status.</returns>
	UAUDIO_RESULT WaveReader::SaveSound(const char *a_FilePath, const WaveFormat &a_WaveFormat)
	{
		FILE *file;

		// Open the file.
		fopen_s(&file, a_FilePath, "wb");
		if (file == nullptr)
		{
			logger::log_warning("<WaveReader> Failed saving file: (%s\"%s%s\").", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
			return UAUDIO_RESULT::UAUDIO_ERR_FILE_BAD;
		}

		fwrite("RIFF", CHUNK_ID_SIZE, 1, file);
		uint32_t chunk_size = CHUNK_ID_SIZE;
		for (auto &m_Chunk : a_WaveFormat.m_Chunks)
			chunk_size += m_Chunk->chunkSize + sizeof(WaveChunkData);
		fwrite(reinterpret_cast<char *>(&chunk_size), sizeof(chunk_size), 1, file);
		fwrite(FMT_CHUNK_FORMAT, CHUNK_ID_SIZE, 1, file);

		for (auto *m_Chunk : a_WaveFormat.m_Chunks)
		{
			fwrite(reinterpret_cast<char *>(&m_Chunk->chunk_id), CHUNK_ID_SIZE, 1, file);
			fwrite(reinterpret_cast<char *>(&m_Chunk->chunkSize), sizeof(m_Chunk->chunkSize), 1, file);
			fwrite(reinterpret_cast<char *>(utils::add(m_Chunk, sizeof(WaveChunkData))), m_Chunk->chunkSize, 1, file);
			logger::log_info(R"(<WaveReader> Saved chunk %s"%.4s"%s with size %s"%i"%s to file: (%s"%s%s").)", logger::COLOR_YELLOW, m_Chunk->chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, m_Chunk->chunkSize, logger::COLOR_WHITE, logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		}

		fclose(file);
		file = nullptr;
		logger::log_success(R"(<WaveReader> Saved file: (%s"%s%s").)", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return UAUDIO_RESULT::UAUDIO_OK;
	}
}
