#include <algorithm>
#include <filesystem>
#include <iostream>

#include <wave/WaveReader.h>

#include <wave/WaveConverter.h>
#include <utils/Logger.h>

#include "wave/WaveFile.h"

namespace uaudio
{
	/// <summary>
	/// Loads the sound.
	/// </summary>
	/// <param name="a_FilePath">The path to the file.</param>
	/// <param name="a_WavFormat">The wave format.</param>
	/// <param name="a_File">The pointer to the file.</param>
	/// <returns>Return 0 on success.</returns>
	WAVE_LOADING_STATUS WaveReader::LoadSound(const char *a_FilePath, WaveFormat &a_WavFormat, FILE *&a_File, WAVE_CONFIG a_WavConfig)
	{
		// Ensure the RIFF, FMT and DATA chunk are in there to avoid crashes.
		a_WavConfig.m_Flags = a_WavConfig.m_Flags | UAUDIO_DEFAULT_CHUNKS;

		// Reset the wave format.
		a_WavFormat = WaveFormat();

		a_WavFormat.m_FilePath = std::string(a_FilePath);

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
			logger::log_warning("<WaveReader> Failed opening file: (\"%s\").", a_FilePath);
			return WAVE_LOADING_STATUS::STATUS_FAILED_OPENING_FILE;
		}

		fseek(a_File, 0, SEEK_END);
		long total_size = ftell(a_File);
		rewind(a_File);

		unsigned char previousChunkid[4] = {};

		// Check if file has reached eof.
		while (total_size != ftell(a_File))
		{
			unsigned char chunkid[4] = {};

			memcpy(previousChunkid, chunkid, sizeof(chunkid));

			uint32_t chunksize = 0;

			// Read what chunk id the a_File has.
			fread(&chunkid, sizeof(chunkid), 1, a_File);

			// Failsafe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (strcmp(std::string(&previousChunkid[0], &previousChunkid[0] + std::size(previousChunkid)).c_str(), std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str()) == 0)
			{
				logger::log_warning("<WaveReader> Failed to load wave a_File (\"%s\").", a_FilePath);
				return WAVE_LOADING_STATUS::STATUS_FAILED_LOADING_CHUNK;
				break;
			}

			// Check chunk size (chunk size).
			fread(&chunksize, sizeof(chunksize), 1, a_File);

			/*
			 * RIFF CHUNK
			 */
			if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_RIFF) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), RIFF_CHUNK_ID) == 0)
			{
				unsigned char format[4] = {};

				// Format (should be always be WAVE).
				fread(&format, sizeof(format), 1, a_File);

				if (strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), FMT_CHUNK_FORMAT) != 0)
				{
					logger::ASSERT(strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), FMT_CHUNK_FORMAT) != 0, "<WavReader> (\"%s\") is not a WAV a_File. (%s).", a_FilePath, format);
					return WAVE_LOADING_STATUS::STATUS_FAILED_NO_WAVE_FILE;
				}

				SetRIFFChunk(a_WavFormat, chunkid, chunksize, format);
			}
			/*
			 * FMT CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_FMT) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), FMT_CHUNK_ID) == 0)
			{
				uint16_t audioFormat = 0;
				uint16_t numChannels = 0;
				uint32_t sampleRate = 0;
				uint32_t byteRate = 0;
				uint16_t blockAlign = 0;
				uint16_t bitsPerSample = 0;

				fread(&audioFormat, sizeof(audioFormat), 1, a_File);
				fread(&numChannels, sizeof(numChannels), 1, a_File);
				fread(&sampleRate, sizeof(sampleRate), 1, a_File);
				fread(&byteRate, sizeof(byteRate), 1, a_File);
				fread(&blockAlign, sizeof(blockAlign), 1, a_File);
				fread(&bitsPerSample, sizeof(bitsPerSample), 1, a_File);

				SetFMTChunk(a_WavFormat, chunkid, chunksize, audioFormat, numChannels, sampleRate, byteRate, blockAlign, bitsPerSample);

				// NOTE: Sometimes the fmt chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<FMT_Chunk>();
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger fmt chunk than usual, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * DATA CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_DATA) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), DATA_CHUNK_ID) == 0)
			{
				// Actual data.
				unsigned char *data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(chunksize)); // set aside sound buffer space.
				fread(data, 1, chunksize, a_File);													 // read in our whole sound data chunk.

				SetDataChunk(a_WavFormat, chunkid, chunksize, data);

				free(data);
			}
			/*
			 * ACID CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_ACID) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), ACID_CHUNK_ID) == 0)
			{
				uint32_t type_of_a_File = 0x5;
				uint16_t root_note = 0x3c;
				uint32_t num_of_beats = 0;
				uint16_t meter_denominator = 4;
				uint16_t meter_numerator = 4;
				float tempo = 0.0f;

				fread(&type_of_a_File, sizeof(type_of_a_File), 1, a_File);
				fread(&root_note, sizeof(root_note), 1, a_File);
				fseek(a_File, 6, SEEK_CUR);
				fread(&num_of_beats, sizeof(num_of_beats), 1, a_File);
				fread(&meter_denominator, sizeof(meter_denominator), 1, a_File);
				fread(&meter_numerator, sizeof(meter_numerator), 1, a_File);
				fread(&tempo, sizeof(tempo), 1, a_File);

				SetAcidChunk(a_WavFormat, chunkid, chunksize, type_of_a_File, root_note, num_of_beats, meter_denominator, meter_numerator, tempo);

				// NOTE: Sometimes the acid chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<ACID_Chunk>();
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger acid chunk than usual, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * BEXT CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_BEXT) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), BEXT_CHUNK_ID) == 0)
			{
				char description[256] = {};
				char originator[32] = {};
				char originator_reference[32] = {};
				char origination_date[10] = {};
				char origination_time[8] = {};
				uint32_t time_reference_low = 0;
				uint32_t time_reference_high = 0;
				uint16_t version = 0;
				unsigned char umid[64] = {};
				uint16_t loudness_value = 0;
				uint16_t loudness_range = 0;
				uint16_t max_true_peak_level = 0;
				uint16_t max_momentary_loudness = 0;
				uint16_t max_short_term_loudness = 0;
				unsigned char reserved[180] = {};

				fread(&description, sizeof(description), 1, a_File);
				fread(&originator, sizeof(originator), 1, a_File);
				fread(&originator_reference, sizeof(originator_reference), 1, a_File);
				fread(&origination_date, sizeof(origination_date), 1, a_File);
				fread(&origination_time, sizeof(origination_time), 1, a_File);
				fread(&time_reference_low, sizeof(time_reference_low), 1, a_File);
				fread(&time_reference_high, sizeof(time_reference_high), 1, a_File);
				fread(&version, sizeof(version), 1, a_File);
				fread(&umid, sizeof(umid), 1, a_File);
				fread(&loudness_value, sizeof(loudness_value), 1, a_File);
				fread(&loudness_range, sizeof(loudness_range), 1, a_File);
				fread(&max_true_peak_level, sizeof(max_true_peak_level), 1, a_File);
				fread(&max_momentary_loudness, sizeof(max_momentary_loudness), 1, a_File);
				fread(&max_short_term_loudness, sizeof(max_short_term_loudness), 1, a_File);
				fread(&reserved, sizeof(reserved), 1, a_File);

				SetBextChunk(a_WavFormat, chunkid, chunksize, description, originator, originator_reference, origination_date, origination_time, time_reference_low, time_reference_high, version, umid, loudness_value, loudness_range, max_true_peak_level, max_momentary_loudness, max_short_term_loudness, reserved);

				// NOTE: Sometimes the bext chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<BEXT_Chunk>();
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger bext chunk than usual, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * FACT CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_FACT) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), FACT_CHUNK_ID) == 0)
			{
				uint32_t sample_length = 0;

				fread(&sample_length, sizeof(sample_length), 1, a_File);

				SetFACTChunk(a_WavFormat, chunkid, chunksize, sample_length);

				// NOTE: Sometimes the fact chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<FACT_Chunk>();
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger fact chunk than usual, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * SMPL CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_SMPL) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), SMPL_CHUNK_ID) == 0)
			{
				uint32_t manufacturer = 0;
				uint32_t product = 0;
				uint32_t sample_period = 0;
				uint32_t midi_unity_node = 0;
				uint32_t midi_pitch_fraction = 0;
				uint32_t smpte_format = 0;
				uint32_t smpte_offset = 0;
				uint32_t num_sample_loops = 0;
				uint32_t sampler_data = 0;
				SMPL_Sample_Loop *samples = nullptr;

				fread(&manufacturer, sizeof(manufacturer), 1, a_File);
				fread(&product, sizeof(product), 1, a_File);
				fread(&sample_period, sizeof(sample_period), 1, a_File);
				fread(&midi_unity_node, sizeof(midi_unity_node), 1, a_File);
				fread(&midi_pitch_fraction, sizeof(midi_pitch_fraction), 1, a_File);
				fread(&smpte_format, sizeof(smpte_format), 1, a_File);
				fread(&smpte_offset, sizeof(smpte_offset), 1, a_File);
				fread(&num_sample_loops, sizeof(num_sample_loops), 1, a_File);
				fread(&sampler_data, sizeof(sampler_data), 1, a_File);

				samples = static_cast<SMPL_Sample_Loop *>(UAUDIO_DEFAULT_ALLOC(sizeof(SMPL_Sample_Loop) * num_sample_loops));

				for (uint32_t i = 0; i < num_sample_loops; i++)
				{
					fread(&samples[i].cue_point_id, sizeof(samples[i].cue_point_id), 1, a_File);
					fread(&samples[i].type, sizeof(samples[i].type), 1, a_File);
					fread(&samples[i].start, sizeof(samples[i].start), 1, a_File);
					fread(&samples[i].end, sizeof(samples[i].end), 1, a_File);
					fread(&samples[i].fraction, sizeof(samples[i].fraction), 1, a_File);
					fread(&samples[i].play_count, sizeof(samples[i].play_count), 1, a_File);
				}

				SetSMPLChunk(a_WavFormat, chunkid, chunksize, manufacturer, product, sample_period, midi_unity_node, midi_pitch_fraction, smpte_format, smpte_offset, num_sample_loops, sampler_data, samples);

				// NOTE: Sometimes the smpl chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<SMPL_Chunk>() + (num_sample_loops * sizeof(SMPL_Sample_Loop));
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger smpl chunk than expected, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * CUE CHUNK
			 */
			else if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_CUE) && strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), CUE_CHUNK_ID) == 0)
			{
				uint32_t num_cue_points = 0;
				CUE_Point *cue_points = nullptr;

				fread(&num_cue_points, sizeof(num_cue_points), 1, a_File);

				cue_points = static_cast<CUE_Point *>(UAUDIO_DEFAULT_ALLOC(sizeof(CUE_Point) * num_cue_points));

				for (uint32_t i = 0; i < num_cue_points; i++)
				{
					fread(&cue_points[i].id, sizeof(cue_points[i].id), 1, a_File);
					fread(&cue_points[i].position, sizeof(cue_points[i].position), 1, a_File);
					fread(&cue_points[i].data_chunk_id, sizeof(cue_points[i].data_chunk_id), 1, a_File);
					fread(&cue_points[i].chunk_start, sizeof(cue_points[i].chunk_start), 1, a_File);
					fread(&cue_points[i].block_start, sizeof(cue_points[i].block_start), 1, a_File);
					fread(&cue_points[i].sample_offset, sizeof(cue_points[i].sample_offset), 1, a_File);
				}

				SetCUEChunk(a_WavFormat, chunkid, chunksize, num_cue_points, cue_points);
				
				// NOTE: Sometimes the cue chunk is bigger than defined on my sources.
				uint32_t expected_chunksize = GetChunkSize<CUE_Chunk>() + (num_cue_points * sizeof(CUE_Point));
				if (expected_chunksize < chunksize)
				{
					logger::log_info("<WaveReader> (\"%s\") Bigger cue chunk than expected, skipping rest of data.", a_FilePath);
					fseek(a_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
				}
			}
			/*
			 * ANY UNSUPPORTED CHUNKS
			 */
			else
			{
				if (a_WavConfig.HasChunk(UAUDIO_CHUNK_FLAG::CHUNK_FLAG_UNSUPPORTED_CHUNKS))
				{
					Chunk otherChunk = {};
					memcpy(otherChunk.chunkId, chunkid, CHUNK_ID_SIZE);
					otherChunk.chunkSize = chunksize;
					a_WavFormat.otherChunks.push_back(otherChunk);
				}

				logger::print_cyan();
				logger::log_info("<WavReader> (\"%s\") ** Found unsupported %s Chunk with size %i **", a_FilePath, std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), chunksize);
				logger::print_white();

				fseek(a_File, static_cast<int32_t>(chunksize), SEEK_CUR);
			}
		}

		if (a_WavFormat.fmtChunk.bitsPerSample == WAVE_BPS_32)
		{
			logger::log_info("<WaveReader> (\"%s\") Wave file is 32bit. Converting now.", a_FilePath);

			Convert32To16(a_WavFormat);
		}
		else if (a_WavFormat.fmtChunk.bitsPerSample == WAVE_BPS_24)
		{
			logger::log_info("<WaveReader> (\"%s\") Wave file is 24bit. Converting now.", a_FilePath);

			Convert24To16(a_WavFormat);
		}

		if (a_WavFormat.fmtChunk.numChannels == WAVE_CHANNELS_MONO)
		{
			a_WavFormat.fmtChunk.numChannels = WAVE_CHANNELS_STEREO;

			unsigned char *array_16 = conversion::ConvertMonoToStereo(a_WavFormat.dataChunk.data, a_WavFormat.dataChunk.chunkSize, a_WavFormat.fmtChunk.blockAlign);
			free(a_WavFormat.dataChunk.data);
			a_WavFormat.dataChunk.data = array_16;
		}

		// Recalculate block align and byterate.
		a_WavFormat.fmtChunk.blockAlign = a_WavFormat.fmtChunk.numChannels * a_WavFormat.fmtChunk.bitsPerSample / 8;
		a_WavFormat.fmtChunk.byteRate = a_WavFormat.fmtChunk.sampleRate * a_WavFormat.fmtChunk.numChannels * a_WavFormat.fmtChunk.bitsPerSample / 8;

		fclose(a_File);
		a_File = nullptr;

		return WAVE_LOADING_STATUS::STATUS_SUCCESSFUL;
	}

	/// <summary>
	/// Sets the riff chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'RIFF').</param>
	/// <param name="a_ChunkSize">The chunk size (this is the size of the entire file - 36).</param>
	/// <param name="a_Format">The format (which should be 'WAVE').</param>
	void WaveReader::SetRIFFChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[4])
	{
		a_WavFormat.riffChunk = RIFF_Chunk(a_ChunkId, a_ChunkSize, a_Format);

		a_WavFormat.filledRiffChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found RIFF Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.riffChunk.chunkId[0], &a_WavFormat.riffChunk.chunkId[0] + std::size(a_WavFormat.riffChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.riffChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") format: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.riffChunk.format[0], &a_WavFormat.riffChunk.format[0] + std::size(a_WavFormat.riffChunk.format)).c_str());
	}

	/// <summary>
	/// Sets the format chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'fmt ').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_AudioFormat">The audio format.</param>
	/// <param name="a_NumChannels">The number of channels.</param>
	/// <param name="a_SampleRate">The sample rate.</param>
	/// <param name="a_ByteRate">The byte rate.</param>
	/// <param name="a_BlockAlign">The block align.</param>
	/// <param name="a_BitsPerSample">The bits per sample.</param>
	void WaveReader::SetFMTChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample)
	{
		a_WavFormat.fmtChunk = FMT_Chunk(a_ChunkId, a_ChunkSize, a_AudioFormat, a_NumChannels, a_SampleRate, a_ByteRate, a_BlockAlign, a_BitsPerSample);

		a_WavFormat.filledFmtChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found FMT Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.fmtChunk.chunkId[0], &a_WavFormat.fmtChunk.chunkId[0] + std::size(a_WavFormat.fmtChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") audioFormat: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
		if (a_WavFormat.fmtChunk.audioFormat != WAV_FORMAT_PCM)
			logger::log_warning("<WavReader> (\"%s\") does not have the format 1 (PCM). This indicates there is probably some form of compression (%i).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
		logger::log_info("<WavReader> (\"%s\") numChannels: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.numChannels);
		logger::log_info("<WavReader> (\"%s\") sampleRate: %ihz.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
		if (a_WavFormat.fmtChunk.sampleRate != WAVE_SAMPLE_RATE_44100)
			logger::log_warning("<WavReader> (\"%s\") does not have a 44100 hz sample rate (%ihz).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
		logger::log_info("<WavReader> (\"%s\") byteRate: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.byteRate);
		logger::log_info("<WavReader> (\"%s\") blockAlign: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.blockAlign);
		logger::log_info("<WavReader> (\"%s\") bitsPerSample: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.bitsPerSample);
	}

	/// <summary>
	/// Sets the data chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'data').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_Data">The data.</param>
	void WaveReader::SetDataChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char *a_Data)
	{
		a_WavFormat.dataChunk = DATA_Chunk(a_ChunkId, a_ChunkSize, a_Data);

		a_WavFormat.filledDataChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found DATA Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.dataChunk.chunkId[0], &a_WavFormat.dataChunk.chunkId[0] + std::size(a_WavFormat.dataChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.dataChunk.chunkSize);
	}

	/// <summary>
	/// Sets the acid chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'acid').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_TypeOfFile">The type of file.</param>
	/// <param name="a_RootNote">The root note.</param>
	/// <param name="a_NumOfBeats">The number of beats.</param>
	/// <param name="a_MeterDenominator">The meter denominator.</param>
	/// <param name="a_MeterNumerator">The meter numerator.</param>
	/// <param name="a_Tempo">The tempo.</param>
	void WaveReader::SetAcidChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo)
	{
		a_WavFormat.acidChunk = ACID_Chunk(a_ChunkId, a_ChunkSize, a_TypeOfFile, a_RootNote, 0, 0.0f, a_NumOfBeats, a_MeterDenominator, a_MeterNumerator, a_Tempo);

		a_WavFormat.filledAcidChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found ACID Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.acidChunk.chunkId[0], &a_WavFormat.acidChunk.chunkId[0] + std::size(a_WavFormat.acidChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") type_of_file: 0x%x.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.type_of_file);
		logger::log_info("<WavReader> (\"%s\") root_note: 0x%x.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.root_note);
		logger::log_info("<WavReader> (\"%s\") num_of_beats: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.num_of_beats);
		logger::log_info("<WavReader> (\"%s\") meter_denominator: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.meter_denominator);
		logger::log_info("<WavReader> (\"%s\") meter_numerator: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.meter_numerator);
		logger::log_info("<WavReader> (\"%s\") tempo: %fbpm.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.tempo);
	}

	/// <summary>
	/// Sets the bext chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'bext').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_Description">The description.</param>
	/// <param name="a_Originator">The originator.</param>
	/// <param name="a_OriginatorReference">The originator reference.</param>
	/// <param name="a_OriginationDate">Creation date.</param>
	/// <param name="a_OriginationTime">Creation time.</param>
	/// <param name="a_TimeReferenceLow">The low time reference.</param>
	/// <param name="a_TimeReferenceHigh">The high time reference.</param>
	/// <param name="a_Version">The version.</param>
	/// <param name="a_Umid">The umid.</param>
	/// <param name="a_LoudnessValue">The loudness value.</param>
	/// <param name="a_LoudnessRange">The loudness range.</param>
	/// <param name="a_MaxTruePeakLevel">The max true peak level.</param>
	/// <param name="a_MaxMomentaryLoudness">The max momentary loudness value.</param>
	/// <param name="a_MaxShortTermLoudness">The max short term loudness value.</param>
	/// <param name="a_Reserved">The reserved value.</param>
	void WaveReader::SetBextChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180])
	{
		a_WavFormat.bextChunk = BEXT_Chunk(a_ChunkId, a_ChunkSize, a_Description, a_Originator, a_OriginatorReference, a_OriginationDate, a_OriginationTime, a_TimeReferenceLow, a_TimeReferenceHigh, a_Version, a_Umid, a_LoudnessValue, a_LoudnessRange, a_MaxTruePeakLevel, a_MaxMomentaryLoudness, a_MaxShortTermLoudness, a_Reserved);

		a_WavFormat.filledBextChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found BEXT Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.chunkId[0], &a_WavFormat.bextChunk.chunkId[0] + std::size(a_WavFormat.bextChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") description: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.description[0], &a_WavFormat.bextChunk.description[0] + std::size(a_WavFormat.bextChunk.description)).c_str());
		logger::log_info("<WavReader> (\"%s\") originator: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.originator[0], &a_WavFormat.bextChunk.originator[0] + std::size(a_WavFormat.bextChunk.originator)).c_str());
		logger::log_info("<WavReader> (\"%s\") originator_reference: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.originator_reference[0], &a_WavFormat.bextChunk.originator_reference[0] + std::size(a_WavFormat.bextChunk.originator_reference)).c_str());
		logger::log_info("<WavReader> (\"%s\") origination_date: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.origination_date[0], &a_WavFormat.bextChunk.origination_date[0] + std::size(a_WavFormat.bextChunk.origination_date)).c_str());
		logger::log_info("<WavReader> (\"%s\") origination_time: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.origination_time[0], &a_WavFormat.bextChunk.origination_time[0] + std::size(a_WavFormat.bextChunk.origination_time)).c_str());
		logger::log_info("<WavReader> (\"%s\") time_reference_low: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.time_reference_low);
		logger::log_info("<WavReader> (\"%s\") time_reference_high: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.time_reference_high);
		logger::log_info("<WavReader> (\"%s\") version: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.version);
		logger::log_info("<WavReader> (\"%s\") umid: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.umid[0], &a_WavFormat.bextChunk.umid[0] + std::size(a_WavFormat.bextChunk.umid)).c_str());
		logger::log_info("<WavReader> (\"%s\") loudness_value: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.loudness_value);
		logger::log_info("<WavReader> (\"%s\") loudness_range: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.loudness_range);
		logger::log_info("<WavReader> (\"%s\") max_true_peak_level: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_true_peak_level);
		logger::log_info("<WavReader> (\"%s\") max_momentary_loudness: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_momentary_loudness);
		logger::log_info("<WavReader> (\"%s\") max_short_term_loudness: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_short_term_loudness);
		logger::log_info("<WavReader> (\"%s\") reserved: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.reserved[0], &a_WavFormat.bextChunk.reserved[0] + std::size(a_WavFormat.bextChunk.reserved)).c_str());
	}

	/// <summary>
	/// Sets the fact chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'bext').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_SampleLength">The sample length.</param>
	void WaveReader::SetFACTChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_SampleLength)
	{
		a_WavFormat.factChunk = FACT_Chunk(a_ChunkId, a_ChunkSize, a_SampleLength);

		a_WavFormat.filledFactChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found FACT Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.factChunk.chunkId[0], &a_WavFormat.factChunk.chunkId[0] + std::size(a_WavFormat.factChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.factChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") sample_length: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.factChunk.sample_length);
	}

	/// <summary>
	/// Sets the smpl chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'bext').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_Manufacturer">The manufacturer.</param>
	/// <param name="a_Product">The product.</param>
	/// <param name="a_SamplePeriod">The sample period.</param>
	/// <param name="a_MidiUnityNode">The midi unity node.</param>
	/// <param name="a_MidiPitchFraction">The midi pitch fraction.</param>
	/// <param name="a_SmpteFormat">The smpte format.</param>
	/// <param name="a_SmpteOffset">The smpte offset.</param>
	/// <param name="a_NumSampleLoops">The number of sample loops.</param>
	/// <param name="a_SamplerData">The sampler data.</param>
	/// <param name="a_Samples">The sample array.</param>
	void WaveReader::SetSMPLChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_Manufacturer, uint32_t a_Product, uint32_t a_SamplePeriod, uint32_t a_MidiUnityNode, uint32_t a_MidiPitchFraction, uint32_t a_SmpteFormat, uint32_t a_SmpteOffset, uint32_t a_NumSampleLoops, uint32_t a_SamplerData, SMPL_Sample_Loop *a_Samples)
	{
		a_WavFormat.smplChunk = SMPL_Chunk(a_ChunkId, a_ChunkSize, a_Manufacturer, a_Product, a_SamplePeriod, a_MidiUnityNode, a_MidiPitchFraction, a_SmpteFormat, a_SmpteOffset, a_NumSampleLoops, a_SamplerData, a_Samples);

		a_WavFormat.filledSmplChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found SMPL Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.smplChunk.chunkId[0], &a_WavFormat.smplChunk.chunkId[0] + std::size(a_WavFormat.smplChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") manufacturer: 0x%x.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.manufacturer);
		logger::log_info("<WavReader> (\"%s\") product: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.product);
		logger::log_info("<WavReader> (\"%s\") sample_period: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.sample_period);
		logger::log_info("<WavReader> (\"%s\") midi_unity_node: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.midi_unity_node);
		logger::log_info("<WavReader> (\"%s\") midi_pitch_fraction: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.midi_pitch_fraction);
		logger::log_info("<WavReader> (\"%s\") smpte_format: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.smpte_format);
		logger::log_info("<WavReader> (\"%s\") smpte_offset: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.smpte_offset);
		logger::log_info("<WavReader> (\"%s\") num_sample_loops: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.num_sample_loops);
		logger::log_info("<WavReader> (\"%s\") sampler_data: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.sampler_data);
		for (uint32_t i = 0; i < a_WavFormat.smplChunk.num_sample_loops; i++)
		{
			logger::print_cyan();
			logger::log_info("<WavReader> (\"%s\") ** SAMPLE LOOP %i **", a_WavFormat.m_FilePath.c_str(), i);
			logger::print_white();
			logger::log_info("<WavReader> (\"%s\") cue_point_id: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].cue_point_id);
			logger::log_info("<WavReader> (\"%s\") type: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].type);
			logger::log_info("<WavReader> (\"%s\") start: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].start);
			logger::log_info("<WavReader> (\"%s\") end: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].end);
			logger::log_info("<WavReader> (\"%s\") fraction: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].fraction);
			logger::log_info("<WavReader> (\"%s\") play_count: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.samples[i].play_count);
		}
	}

	/// <summary>
	/// Sets the cue chunk.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	/// <param name="a_ChunkId">The chunk id (should be 'bext').</param>
	/// <param name="a_ChunkSize">The chunk size.</param>
	/// <param name="a_NumCuePoints">The number of cue points.</param>
	/// <param name="a_QuePoints">The cue point array.</param>
	void WaveReader::SetCUEChunk(WaveFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_NumCuePoints, CUE_Point *a_QuePoints)
	{
		a_WavFormat.cueChunk = CUE_Chunk(a_ChunkId, a_ChunkSize, a_NumCuePoints, a_QuePoints);

		a_WavFormat.filledCueChunk = true;

		logger::print_cyan();
		logger::log_info("<WavReader> (\"%s\") ** Found CUE Chunk **", a_WavFormat.m_FilePath.c_str());
		logger::print_white();
		logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.cueChunk.chunkId[0], &a_WavFormat.cueChunk.chunkId[0] + std::size(a_WavFormat.cueChunk.chunkId)).c_str());
		logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.chunkSize);
		logger::log_info("<WavReader> (\"%s\") num_cue_points: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.num_cue_points);
		for (uint32_t i = 0; i < a_WavFormat.cueChunk.num_cue_points; i++)
		{
			logger::print_cyan();
			logger::log_info("<WavReader> (\"%s\") ** CUE POINT %i **", a_WavFormat.m_FilePath.c_str(), i);
			logger::print_white();
			logger::log_info("<WavReader> (\"%s\") id: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].id);
			logger::log_info("<WavReader> (\"%s\") position: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].position);
			logger::log_info("<WavReader> (\"%s\") data_chunk_id: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].data_chunk_id);
			logger::log_info("<WavReader> (\"%s\") chunk_start: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].chunk_start);
			logger::log_info("<WavReader> (\"%s\") block_start: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].block_start);
			logger::log_info("<WavReader> (\"%s\") sample_offset: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.cueChunk.cue_points[i].sample_offset);
		}
	}

	/// <summary>
	/// Converts 32 bit pcm data to 16 bit pcm data.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	void WaveReader::Convert32To16(WaveFormat &a_WavFormat)
	{
		a_WavFormat.fmtChunk.bitsPerSample = WAVE_BPS_16;

		uint16_t *array_16 = conversion::Convert32To16(a_WavFormat.dataChunk.data, a_WavFormat.dataChunk.chunkSize);
		UAUDIO_DEFAULT_FREE(a_WavFormat.dataChunk.data);
		a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
	}

	/// <summary>
	/// Converts 24 bit pcm data to 16 bit pcm data.
	/// </summary>
	/// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
	void WaveReader::Convert24To16(WaveFormat &a_WavFormat)
	{
		a_WavFormat.fmtChunk.bitsPerSample = WAVE_BPS_16;

		uint16_t *array_16 = conversion::Convert24To16(a_WavFormat.dataChunk.data, a_WavFormat.dataChunk.chunkSize);
		UAUDIO_DEFAULT_FREE(a_WavFormat.dataChunk.data);
		a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
	}
}