#include "wave/WaveReader.h"

#include "utils/Logger.h"

namespace uaudio
{
	/// <summary>
	/// Loads the sound.
	/// </summary>
	/// <param name="a_FilePath">The path to the file.</param>
	/// <param name="a_WavFormat">The wave format.</param>
	/// <param name="a_File">The pointer to the file.</param>
	/// <param name="a_Chunks">The chunks the user wants to load.</param>
	/// <returns>WAVE loading status.</returns>
	WAVE_LOADING_STATUS WaveReader::LoadSound(const char* a_FilePath, WaveFormat& a_WavFormat, FILE*& a_File, std::vector<const char*> a_Chunks)
	{
		// Ensure these chunks are always there.
		a_Chunks.push_back(DATA_CHUNK_ID);
		a_Chunks.push_back(FMT_CHUNK_ID);

		a_WavFormat.m_FilePath = a_FilePath;

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
			return WAVE_LOADING_STATUS::STATUS_FAILED_OPENING_FILE;
		}

		fseek(a_File, 0, SEEK_END);
		long total_size = ftell(a_File);
		rewind(a_File);

		logger::log_info("<WaveReader> Reading wave file: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);

		char previous_chunk_id[4] = {};

		// Check if file has reached eof.
		while (total_size != ftell(a_File))
		{
			char chunk_id[CHUNK_ID_SIZE] = { };

			// Read what chunk id the a_File has.
			fread(&chunk_id, sizeof(chunk_id), 1, a_File);

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (strncmp(&chunk_id[0], &previous_chunk_id[0], CHUNK_ID_SIZE) == 0)
			{
				logger::log_warning("<WaveReader> Failed to load wave a_File (\"%s\").", a_FilePath);
				return WAVE_LOADING_STATUS::STATUS_FAILED_LOADING_CHUNK;
			}

			memcpy(previous_chunk_id, chunk_id, sizeof(chunk_id));

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
			for (auto& chunk_name : a_Chunks)
				if (strncmp(&chunk_id[0], chunk_name, CHUNK_ID_SIZE) == 0)
					get_chunk = true;

			if (get_chunk)
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);

				Chunk_Data* chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(chunk_size + sizeof(Chunk_Data)));

				if (chunk_data != nullptr)
				{
					memcpy(chunk_data->chunk_id, chunk_id, sizeof(chunk_id));
					chunk_data->chunkSize = chunk_size;
					fread(reinterpret_cast<unsigned char*>(chunk_data) + sizeof(Chunk_Data), 1, chunk_size, a_File);
				}
				else
					fseek(a_File, static_cast<long>(chunk_size), SEEK_CUR);

				a_WavFormat.m_Chunks.push_back(chunk_data);
			}
			else
			{
				logger::log_info(R"(<WaveReader> Found %s"%.4s"%s chunk with size %s"%i"%s that is not in config.)", logger::COLOR_YELLOW, chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, chunk_size, logger::COLOR_WHITE);
				fseek(a_File, static_cast<long>(chunk_size), SEEK_CUR);
			}
		}

		fclose(a_File);
		a_File = nullptr;

		logger::log_success("<WaveReader> Opened file successfully: (%s\"%s\"%s).", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return WAVE_LOADING_STATUS::STATUS_SUCCESSFUL;
	}

	/// <summary>
	/// Saves a file with the chunks inside the wave format.
	/// </summary>
	/// <param name="a_FilePath">The path to save to.</param>
	/// <param name="a_WaveFormat">The format with all the chunks.</param>
	/// <returns>WAVE saving status.</returns>
	WAVE_SAVING_STATUS WaveReader::SaveSound(const char* a_FilePath, const WaveFormat& a_WaveFormat)
	{
		FILE* file;

		// Open the file.
		fopen_s(&file, a_FilePath, "wb");
		if (file == nullptr)
		{
			logger::log_warning("<WaveReader> Failed saving file: (%s\"%s%s\").", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
			return WAVE_SAVING_STATUS::STATUS_FAILED_OPENING_FILE;
		}

		fwrite("RIFF", CHUNK_ID_SIZE, 1, file);
		uint32_t chunk_size = CHUNK_ID_SIZE;
		for (auto& m_Chunk : a_WaveFormat.m_Chunks)
			chunk_size += m_Chunk->chunkSize + sizeof(Chunk_Data);
		fwrite(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size), 1, file);
		fwrite(FMT_CHUNK_FORMAT, CHUNK_ID_SIZE, 1, file);

		for (auto* m_Chunk : a_WaveFormat.m_Chunks)
		{
			fwrite(reinterpret_cast<char*>(&m_Chunk->chunk_id), CHUNK_ID_SIZE, 1, file);
			fwrite(reinterpret_cast<char*>(&m_Chunk->chunkSize), sizeof(m_Chunk->chunkSize), 1, file);
			fwrite(reinterpret_cast<char*>(Add(m_Chunk, sizeof(Chunk_Data))), m_Chunk->chunkSize, 1, file);
			logger::log_info(R"(<WaveReader> Saved chunk %s"%.4s"%s with size %s"%i"%s to file: (%s"%s%s").)", logger::COLOR_YELLOW, m_Chunk->chunk_id, logger::COLOR_WHITE, logger::COLOR_YELLOW, m_Chunk->chunkSize, logger::COLOR_WHITE, logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		}

		fclose(file);
		file = nullptr;
		logger::log_success(R"(<WaveReader> Saved file: (%s"%s%s").)", logger::COLOR_YELLOW, a_FilePath, logger::COLOR_WHITE);
		return WAVE_SAVING_STATUS::STATUS_SUCCESSFUL;
	}
}
