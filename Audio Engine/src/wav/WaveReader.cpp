#include <algorithm>
#include <filesystem>
#include <iostream>

#include <wav/WaveReader.h>

#include <wav/WaveConverter.h>
#include <utils/Logger.h>

#include "wav/WaveFile.h"

namespace uaudio
{
    /// <summary>
    /// Loads the sound.
    /// </summary>
    /// <param name="a_FilePath">The path to the file.</param>
    WavFormat WaveReader::LoadSound(const char *a_FilePath)
    {
        WavFormat wav_format = WavFormat();

        wav_format.m_FilePath = std::string(a_FilePath);

        // Open the file.
        FILE *file = nullptr;
        fopen_s(&file, a_FilePath, "rb");
        if (file == nullptr)
        {
            logger::log_error("<WaveReader> Failed opening file: \"%s\".", a_FilePath);
            return wav_format;
        }

        fseek(file, 0, SEEK_END);
        long total_size = ftell(file);
        rewind(file);

        unsigned char previousChunkid[4] = {};
        while (total_size != ftell(file))
        {
            unsigned char chunkid[4] = {};

            //if (strcmp(std::string(&previousChunkid[0], &previousChunkid[0] + std::size(previousChunkid)).c_str(), std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str()) == 0)
            //{
            //    logger::log_error("<WaveReader> Failed to load wave file (\"%s\").", a_FilePath);
            //    return wav_format;
            //    break;
            //}

            memcpy(previousChunkid, chunkid, sizeof(chunkid));

            uint32_t chunksize = 0;

            // Read what chunk id the file has.
            fread(&chunkid, sizeof(chunkid), 1, file);

            // Check chunk size (file size).
            fread(&chunksize, sizeof(chunksize), 1, file);

            /*
             * RIFF CHUNK
             */
            if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "RIFF") == 0)
            {
                unsigned char format[4] = {};

                // Format (should be WAVE).
                fread(&format, sizeof(format), 1, file);

                if (strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), "WAVE") != 0)
                {
                    ASSERT(strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), "WAVE") != 0, "<WavReader> (\"%s\") is not a WAV file. (%s).", a_FilePath, format);
                    return wav_format;
                }

                SetRIFFChunk(wav_format, chunkid, chunksize, format);
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "fmt ") == 0)
            {
                uint16_t audioFormat = 0;
                uint16_t numChannels = 0;
                uint32_t sampleRate = 0;
                uint32_t byteRate = 0;
                uint16_t blockAlign = 0;
                uint16_t bitsPerSample = 0;

                fread(&audioFormat, sizeof(audioFormat), 1, file);
                fread(&numChannels, sizeof(numChannels), 1, file);
                fread(&sampleRate, sizeof(sampleRate), 1, file);
                fread(&byteRate, sizeof(byteRate), 1, file);
                fread(&blockAlign, sizeof(blockAlign), 1, file);
                fread(&bitsPerSample, sizeof(bitsPerSample), 1, file);

                SetFMTChunk(wav_format, chunkid, chunksize, audioFormat, numChannels, sampleRate, byteRate, blockAlign, bitsPerSample);

                // NOTE: Sometimes the fmt chunk is bigger than defined on my sources. 
                uint32_t expected_chunksize = sizeof(FMT_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger fmt chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "data") == 0)
            {
                // Actual data.
                unsigned char *data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(chunksize)); // set aside sound buffer space.
                fread(data, 1, chunksize, file); // read in our whole sound data chunk.

                SetDataChunk(wav_format, chunkid, chunksize, data);

                free(data);
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "acid") == 0)
            {
                uint32_t type_of_file = 0x5;
                uint16_t root_note = 0x3c;
                uint32_t num_of_beats = 0;
                uint16_t meter_denominator = 4;
                uint16_t meter_numerator = 4;
                float tempo = 0.0f;

                fread(&type_of_file, sizeof(type_of_file), 1, file);
                fread(&root_note, sizeof(root_note), 1, file);
                fseek(file, 6, SEEK_CUR);
                fread(&num_of_beats, sizeof(num_of_beats), 1, file);
                fread(&meter_denominator, sizeof(meter_denominator), 1, file);
                fread(&meter_numerator, sizeof(meter_numerator), 1, file);
                fread(&tempo, sizeof(tempo), 1, file);

                SetAcidChunk(wav_format, chunkid, chunksize, type_of_file, root_note, num_of_beats, meter_denominator, meter_numerator, tempo);

                // NOTE: Sometimes the acid chunk is bigger than defined on my sources.
                uint32_t expected_chunksize = sizeof(ACID_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger acid chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "bext") == 0)
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

                fread(&description, sizeof(description), 1, file);
                fread(&originator, sizeof(originator), 1, file);
                fread(&originator_reference, sizeof(originator_reference), 1, file);
                fread(&origination_date, sizeof(origination_date), 1, file);
                fread(&origination_time, sizeof(origination_time), 1, file);
                fread(&time_reference_low, sizeof(time_reference_low), 1, file);
                fread(&time_reference_high, sizeof(time_reference_high), 1, file);
                fread(&version, sizeof(version), 1, file);
                fread(&umid, sizeof(umid), 1, file);
                fread(&loudness_value, sizeof(loudness_value), 1, file);
                fread(&loudness_range, sizeof(loudness_range), 1, file);
                fread(&max_true_peak_level, sizeof(max_true_peak_level), 1, file);
                fread(&max_momentary_loudness, sizeof(max_momentary_loudness), 1, file);
                fread(&max_short_term_loudness, sizeof(max_short_term_loudness), 1, file);
                fread(&reserved, sizeof(reserved), 1, file);

                SetBextChunk(wav_format, chunkid, chunksize, description, originator, originator_reference, origination_date, origination_time, time_reference_low, time_reference_high, version, umid, loudness_value, loudness_range, max_true_peak_level, max_momentary_loudness, max_short_term_loudness, reserved);

                // NOTE: Sometimes the bext chunk is bigger than defined on my sources.
                uint32_t expected_chunksize = sizeof(BEXT_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger bext chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "fact") == 0)
            {
                uint32_t sample_length = 0;

                fread(&sample_length, sizeof(sample_length), 1, file);

                SetFACTChunk(wav_format, chunkid, chunksize, sample_length);

                // NOTE: Sometimes the fact chunk is bigger than defined on my sources. 
                uint32_t expected_chunksize = sizeof(FACT_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger fact chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "smpl") == 0)
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

                fread(&manufacturer, sizeof(manufacturer), 1, file);
                fread(&product, sizeof(product), 1, file);
                fread(&sample_period, sizeof(sample_period), 1, file);
                fread(&midi_unity_node, sizeof(midi_unity_node), 1, file);
                fread(&midi_pitch_fraction, sizeof(midi_pitch_fraction), 1, file);
                fread(&smpte_format, sizeof(smpte_format), 1, file);
                fread(&smpte_offset, sizeof(smpte_offset), 1, file);
                fread(&num_sample_loops, sizeof(num_sample_loops), 1, file);
                fread(&sampler_data, sizeof(sampler_data), 1, file);

                SetSMPLChunk(wav_format, chunkid, chunksize, manufacturer, product, sample_period, midi_unity_node, midi_pitch_fraction, smpte_format, smpte_offset, num_sample_loops, sampler_data);

                // NOTE: Sometimes the smpl chunk is bigger than defined on my sources. 
                uint32_t expected_chunksize = sizeof(SMPL_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger smpl chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else
            {
                Chunk otherChunk = {};
                memcpy(otherChunk.chunkId, chunkid, CHUNK_ID_SIZE);
                otherChunk.chunkSize = chunksize;
                wav_format.otherChunks.push_back(otherChunk);

                logger::log_info("<WavReader> (\"%s\") ** Found %s Chunk with size %i **", a_FilePath, std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), chunksize);

                fseek(file, static_cast<int32_t>(chunksize), SEEK_CUR);
            }
        }

        if (wav_format.fmtChunk.bitsPerSample == 32)
        {
            logger::log_info("<WaveReader> (\"%s\") Wav file is 32bit. Converting now.", a_FilePath);

            Convert32To16(wav_format);
        }
        else if (wav_format.fmtChunk.bitsPerSample == 24)
        {
            logger::log_info("<WaveReader> (\"%s\") Wav file is 24bit. Converting now.", a_FilePath);

            Convert24To16(wav_format);
        }

        if (wav_format.fmtChunk.numChannels == WAVE_CHANNELS_MONO)
        {
            wav_format.fmtChunk.numChannels = WAVE_CHANNELS_STEREO;

            unsigned char *array_16 = conversion::ConvertMonoToStereo(wav_format.dataChunk.data, wav_format.dataChunk.chunkSize, wav_format.fmtChunk.blockAlign);
            free(wav_format.dataChunk.data);
            wav_format.dataChunk.data = array_16;
        }

        wav_format.fmtChunk.audioFormat = WAVE_FORMAT_PCM;
        wav_format.fmtChunk.blockAlign = wav_format.fmtChunk.numChannels * wav_format.fmtChunk.bitsPerSample / 8;
        wav_format.fmtChunk.byteRate = wav_format.fmtChunk.sampleRate * wav_format.fmtChunk.numChannels * wav_format.fmtChunk.bitsPerSample / 8;
        CalculateRiffChunkSize(wav_format);

        fclose(file);
        file = nullptr;

        return wav_format;
    }

    /// <summary>
    /// Calculates the RIFF chunk size (This is the size of the entire file in bytes minus 8 bytes for the two fields not included)
    /// </summary>
    /// <param name="a_WavFormat">The wav format that needs to be recalculated.</param>
    /// <returns></returns>
    uint32_t WaveReader::CalculateRiffChunkSize(WavFormat &a_WavFormat)
    {
        uint32_t size = 0;

        // Riff
        if (a_WavFormat.filledRiffChunk)
            size += sizeof(a_WavFormat.riffChunk.format);

        // Fmt
        if (a_WavFormat.filledFmtChunk)
        {
            size += sizeof(Chunk);
            size += a_WavFormat.fmtChunk.chunkSize;
        }

        // Data
        if (a_WavFormat.filledDataChunk)
        {
            size += sizeof(Chunk);
            size += a_WavFormat.dataChunk.chunkSize;
        }

        // Acid
        if (a_WavFormat.filledAcidChunk)
        {
            size += sizeof(Chunk);
            size += a_WavFormat.acidChunk.chunkSize;
        }

        // Bext
        if (a_WavFormat.filledBextChunk)
        {
            size += sizeof(Chunk);
            size += a_WavFormat.bextChunk.chunkSize;
        }

        // Fact
        if (a_WavFormat.filledFactChunk)
        {
            size += sizeof(Chunk);
            size += a_WavFormat.factChunk.chunkSize;
        }

        for (auto &otherChunk : a_WavFormat.otherChunks)
        {
            size += sizeof(Chunk);
            size += otherChunk.chunkSize;
        }
        return size + 102;
    }

    /// <summary>
    /// Sets the riff chunk.
    /// </summary>
    /// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
    /// <param name="a_ChunkId">The chunk id (should be 'RIFF').</param>
    /// <param name="a_ChunkSize">The chunk size (this is the size of the entire file - 36).</param>
    /// <param name="a_Format">The format (which should be 'WAVE').</param>
    void WaveReader::SetRIFFChunk(WavFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[4])
    {
        memcpy(a_WavFormat.riffChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.riffChunk.chunkId));
        a_WavFormat.riffChunk.chunkSize = a_ChunkSize;
        memcpy(a_WavFormat.riffChunk.format, a_Format, sizeof(a_WavFormat.riffChunk.format));

        a_WavFormat.filledRiffChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found RIFF Chunk **", a_WavFormat.m_FilePath.c_str());
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
    void WaveReader::SetFMTChunk(WavFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample)
    {
        memcpy(a_WavFormat.fmtChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.fmtChunk.chunkId));
        a_WavFormat.fmtChunk.chunkSize = a_ChunkSize;
        a_WavFormat.fmtChunk.audioFormat = a_AudioFormat;
        a_WavFormat.fmtChunk.numChannels = a_NumChannels;
        a_WavFormat.fmtChunk.sampleRate = a_SampleRate;
        a_WavFormat.fmtChunk.byteRate = a_ByteRate;
        a_WavFormat.fmtChunk.blockAlign = a_BlockAlign;
        a_WavFormat.fmtChunk.bitsPerSample = a_BitsPerSample;

        a_WavFormat.filledFmtChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found FMT Chunk **", a_WavFormat.m_FilePath.c_str());
        logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.fmtChunk.chunkId[0], &a_WavFormat.fmtChunk.chunkId[0] + std::size(a_WavFormat.fmtChunk.chunkId)).c_str());
        logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.chunkSize);
        logger::log_info("<WavReader> (\"%s\") audioFormat: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
        if (a_WavFormat.fmtChunk.audioFormat != WAVE_FORMAT_PCM)
            logger::log_error("<WavReader> (\"%s\") does not have the format 1 (PCM). This indicates there is probably some form of compression (%i).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
        logger::log_info("<WavReader> (\"%s\") numChannels: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.numChannels);
        logger::log_info("<WavReader> (\"%s\") sampleRate: %ihz.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
        if (a_WavFormat.fmtChunk.sampleRate != 44100)
            logger::log_error("<WavReader> (\"%s\") does not have a 44100 hz sample rate (%ihz).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
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
    void WaveReader::SetDataChunk(WavFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char *a_Data)
    {
        memcpy(a_WavFormat.dataChunk.chunkId, a_ChunkId, CHUNK_ID_SIZE);
        a_WavFormat.dataChunk.chunkSize = a_ChunkSize;

        // Actual data.
        a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(a_ChunkSize)); // Set aside sound buffer space.

        if (a_WavFormat.dataChunk.data != nullptr)
            memcpy(a_WavFormat.dataChunk.data, a_Data, a_ChunkSize);

        a_WavFormat.filledDataChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found DATA Chunk **", a_WavFormat.m_FilePath.c_str());
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
    void WaveReader::SetAcidChunk(WavFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo)
    {
        memcpy(a_WavFormat.acidChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.acidChunk.chunkId));
        a_WavFormat.acidChunk.chunkSize = a_ChunkSize;
        a_WavFormat.acidChunk.type_of_file = a_TypeOfFile;
        a_WavFormat.acidChunk.root_note = a_RootNote;
        a_WavFormat.acidChunk.num_of_beats = a_NumOfBeats;
        a_WavFormat.acidChunk.meter_denominator = a_MeterDenominator;
        a_WavFormat.acidChunk.meter_numerator = a_MeterNumerator;
        a_WavFormat.acidChunk.tempo = a_Tempo;

        a_WavFormat.filledAcidChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found ACID Chunk **", a_WavFormat.m_FilePath.c_str());
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
    void WaveReader::SetBextChunk(WavFormat &a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180])
    {
        memcpy(a_WavFormat.bextChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.bextChunk.chunkId));
        a_WavFormat.bextChunk.chunkSize = a_ChunkSize;
        memcpy(a_WavFormat.bextChunk.description, a_Description, sizeof(a_WavFormat.bextChunk.description));
        memcpy(a_WavFormat.bextChunk.originator, a_Originator, sizeof(a_WavFormat.bextChunk.originator));
        memcpy(a_WavFormat.bextChunk.originator_reference, a_OriginatorReference, sizeof(a_WavFormat.bextChunk.originator_reference));
        memcpy(a_WavFormat.bextChunk.origination_date, a_OriginationDate, sizeof(a_WavFormat.bextChunk.origination_date));
        memcpy(a_WavFormat.bextChunk.origination_time, a_OriginationTime, sizeof(a_WavFormat.bextChunk.origination_time));
        for (char &j : a_WavFormat.bextChunk.origination_time)
            if (j == '-')
                j = ':';
        a_WavFormat.bextChunk.time_reference_low = a_TimeReferenceLow;
        a_WavFormat.bextChunk.time_reference_high = a_TimeReferenceHigh;
        a_WavFormat.bextChunk.version = a_Version;
        memcpy(a_WavFormat.bextChunk.umid, a_Umid, sizeof(a_WavFormat.bextChunk.umid));
        a_WavFormat.bextChunk.loudness_value = a_LoudnessValue;
        a_WavFormat.bextChunk.loudness_range = a_LoudnessRange;
        a_WavFormat.bextChunk.max_true_peak_level = a_MaxTruePeakLevel;
        a_WavFormat.bextChunk.max_momentary_loudness = a_MaxMomentaryLoudness;
        a_WavFormat.bextChunk.max_short_term_loudness = a_MaxShortTermLoudness;
        memcpy(a_WavFormat.bextChunk.reserved, a_Reserved, sizeof(a_WavFormat.bextChunk.reserved));

        a_WavFormat.filledBextChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found BEXT Chunk **", a_WavFormat.m_FilePath.c_str());
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
    void WaveReader::SetFACTChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_SampleLength)
    {
        memcpy(a_WavFormat.factChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.factChunk.chunkId));
        a_WavFormat.factChunk.chunkSize = a_ChunkSize;
        a_WavFormat.factChunk.sample_length = a_SampleLength;

        a_WavFormat.filledFactChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found FACT Chunk **", a_WavFormat.m_FilePath.c_str());
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
    void WaveReader::SetSMPLChunk(WavFormat& a_WavFormat, unsigned char a_ChunkId[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_Manufacturer, uint32_t a_Product, uint32_t a_SamplePeriod, uint32_t a_MidiUnityNode, uint32_t a_MidiPitchFraction, uint32_t a_SmpteFormat, uint32_t a_SmpteOffset, uint32_t a_NumSampleLoops, uint32_t a_SamplerData)
    {
        memcpy(a_WavFormat.smplChunk.chunkId, a_ChunkId, sizeof(a_WavFormat.smplChunk.chunkId));
        a_WavFormat.smplChunk.chunkSize = a_ChunkSize;
        a_WavFormat.smplChunk.manufacturer = a_Manufacturer;
        a_WavFormat.smplChunk.product = a_Product;
        a_WavFormat.smplChunk.sample_period = a_SamplePeriod;
        a_WavFormat.smplChunk.midi_unity_node = a_MidiUnityNode;
        a_WavFormat.smplChunk.midi_pitch_fraction = a_MidiPitchFraction;
        a_WavFormat.smplChunk.smpte_format = a_SmpteFormat;
        a_WavFormat.smplChunk.smpte_offset = a_SmpteOffset;
        a_WavFormat.smplChunk.num_sample_loops = a_NumSampleLoops;
        a_WavFormat.smplChunk.sampler_data = a_SamplerData;

        a_WavFormat.filledSmplChunk = true;

        logger::log_info("<WavReader> (\"%s\") ** Found SMPL Chunk **", a_WavFormat.m_FilePath.c_str());
        logger::log_info("<WavReader> (\"%s\") chunk id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.smplChunk.chunkId[0], &a_WavFormat.smplChunk.chunkId[0] + std::size(a_WavFormat.smplChunk.chunkId)).c_str());
        logger::log_info("<WavReader> (\"%s\") chunk size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.chunkSize);
        logger::log_info("<WavReader> (\"%s\") manufacturer: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.manufacturer);
        logger::log_info("<WavReader> (\"%s\") product: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.product);
        logger::log_info("<WavReader> (\"%s\") sample_period: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.sample_period);
        logger::log_info("<WavReader> (\"%s\") midi_unity_node: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.midi_unity_node);
        logger::log_info("<WavReader> (\"%s\") midi_pitch_fraction: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.midi_pitch_fraction);
        logger::log_info("<WavReader> (\"%s\") smpte_format: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.smpte_format);
        logger::log_info("<WavReader> (\"%s\") smpte_offset: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.smpte_offset);
        logger::log_info("<WavReader> (\"%s\") num_sample_loops: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.num_sample_loops);
        logger::log_info("<WavReader> (\"%s\") sampler_data: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.smplChunk.sampler_data);
    }

    /// <summary>
    /// Converts 32 bit pcm data to 16 bit pcm data.
    /// </summary>
    /// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
    void WaveReader::Convert32To16(WavFormat &a_WavFormat)
    {
        a_WavFormat.fmtChunk.bitsPerSample = 16;

        uint16_t *array_16 = conversion::Convert32To16(a_WavFormat.dataChunk.data, a_WavFormat.dataChunk.chunkSize);
        UAUDIO_DEFAULT_FREE(a_WavFormat.dataChunk.data);
        a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
    }

    /// <summary>
    /// Converts 24 bit pcm data to 16 bit pcm data.
    /// </summary>
    /// <param name="a_WavFormat">The wav format that needs to be set/changed.</param>
    void WaveReader::Convert24To16(WavFormat &a_WavFormat)
    {
        a_WavFormat.fmtChunk.bitsPerSample = 16;

        uint16_t *array_16 = conversion::Convert24To16(a_WavFormat.dataChunk.data, a_WavFormat.dataChunk.chunkSize);
        UAUDIO_DEFAULT_FREE(a_WavFormat.dataChunk.data);
        a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
    }
}