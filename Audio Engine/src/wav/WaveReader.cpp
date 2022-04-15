#include <algorithm>

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

        uint32_t total_chunksize = 0;
        unsigned char previousChunkid[4] = {};
        while (!feof(file))
        {
            unsigned char chunkid[4];

            if (strcmp(std::string(&previousChunkid[0], &previousChunkid[0] + std::size(previousChunkid)).c_str(), std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str()) == 0)
            {
                logger::log_error("<WaveReader> Failed to load wave file (\"%s\").", a_FilePath);
                return wav_format;
                break;
            }

            memcpy(previousChunkid, chunkid, sizeof(chunkid));

            uint32_t chunksize;

            // Read what chunk id the file has.
            fread(&chunkid, sizeof(chunkid), 1, file);

            // Check chunk size (file size).
            fread(&chunksize, sizeof(chunksize), 1, file);

            /*
             * RIFF CHUNK
             */
            if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "RIFF") == 0)
            {
                total_chunksize += 4;
                unsigned char format[4];

                // Format (should be WAVE).
                fread(&format, sizeof(format), 1, file);

                if (strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), "WAVE") != 0)
                {
                    ASSERT(strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), "WAVE") != 0, "<Wav> (\"%s\") is not a WAV file. (%s).", a_FilePath, format);
                    return wav_format;
                }

                SetRIFFChunk(wav_format, chunkid, chunksize, format);
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "fmt ") == 0)
            {
                total_chunksize += sizeof(Chunk);
                total_chunksize += chunksize;

                uint16_t audioFormat;
                uint16_t numChannels;
                uint32_t sampleRate;
                uint32_t byteRate;
                uint16_t blockAlign;
                uint16_t bitsPerSample;

                // audio format (should be 1, other values indicate compression).
                fread(&audioFormat, sizeof(audioFormat), 1, file);

                // numChannels (mono is 1, stereo is 2).
                fread(&numChannels, sizeof(numChannels), 1, file);

                // sampleRate
                fread(&sampleRate, sizeof(sampleRate), 1, file);

                // byteRate
                fread(&byteRate, sizeof(byteRate), 1, file);

                // blockAlign
                fread(&blockAlign, sizeof(blockAlign), 1, file);

                // bitsPerSample
                fread(&bitsPerSample, sizeof(bitsPerSample), 1, file);

                SetFMTChunk(wav_format, chunkid, chunksize, audioFormat, numChannels, sampleRate, byteRate, blockAlign, bitsPerSample);

                // NOTE: Sometimes the fmt chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
                uint32_t expected_chunksize = sizeof(FMT_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger fmt chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "data") == 0)
            {
                total_chunksize += sizeof(Chunk);
                total_chunksize += chunksize;

                // Actual data.
                unsigned char *data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(sizeof(data) * chunksize)); // set aside sound buffer space.
                fread(data, sizeof(data), chunksize, file);                                                         // read in our whole sound data chunk.

                SetDataChunk(wav_format, chunkid, chunksize, data);

                free(data);
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "acid") == 0)
            {
                total_chunksize += sizeof(Chunk);
                total_chunksize += chunksize;

                uint32_t type_of_file;
                uint16_t root_note;
                uint32_t num_of_beats;
                uint16_t meter_denominator;
                uint16_t meter_numerator;
                float tempo;

                fread(&type_of_file, sizeof(type_of_file), 1, file);

                fread(&root_note, sizeof(root_note), 1, file);

                fseek(file, 6, SEEK_CUR);

                fread(&num_of_beats, sizeof(num_of_beats), 1, file);

                fread(&meter_denominator, sizeof(meter_denominator), 1, file);

                fread(&meter_numerator, sizeof(meter_numerator), 1, file);

                fread(&tempo, sizeof(tempo), 1, file);

                SetAcidChunk(wav_format, chunkid, chunksize, type_of_file, root_note, num_of_beats, meter_denominator, meter_numerator, tempo);

                // NOTE: Sometimes the acid chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
                uint32_t expected_chunksize = sizeof(ACID_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger acid chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "gudf") == 0)
            {
                printf("%s\n", std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str());
                printf("%i\n", chunksize);
                fseek(file, static_cast<int32_t>(chunksize), SEEK_CUR);
            }
            else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "bext") == 0)
            {
                total_chunksize += sizeof(Chunk);
                total_chunksize += chunksize;

                char description[256];
                char originator[32];
                char originator_reference[32];
                char origination_date[10];
                char origination_time[8];
                uint32_t time_reference_low;
                uint32_t time_reference_high;
                uint16_t version;
                unsigned char umid[64];
                uint16_t loudness_value;
                uint16_t loudness_range;
                uint16_t max_true_peak_level;
                uint16_t max_momentary_loudness;
                uint16_t max_short_term_loudness;
                unsigned char reserved[180];

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

                // NOTE: Sometimes the bext chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
                uint32_t expected_chunksize = sizeof(BEXT_Chunk) - sizeof(Chunk);
                if (expected_chunksize < chunksize)
                {
                    logger::log_info("<WaveReader> (\"%s\") Bigger bext chunk than usual, skipping rest of data.", a_FilePath);
                    fseek(file, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
                }
            }
            else
            {
                total_chunksize += sizeof(Chunk);
                total_chunksize += chunksize;

                Chunk otherChunk = {};
                memcpy(wav_format.dataChunk.chunkId, chunkid, 4 * sizeof(unsigned char));
                wav_format.dataChunk.chunkSize = chunksize;
                wav_format.otherChunks.push_back(otherChunk);

                logger::log_info("<WaveReader> (\"%s\") Found subchunk %s with size %i. Skipping.", a_FilePath, std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), chunksize);

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

        logger::log_info("<Wav> (\"%s\") chunkId: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.riffChunk.chunkId[0], &a_WavFormat.riffChunk.chunkId[0] + std::size(a_WavFormat.riffChunk.chunkId)).c_str());
        logger::log_info("<Wav> (\"%s\") chunkSize: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.riffChunk.chunkSize);
        logger::log_info("<Wav> (\"%s\") format: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.riffChunk.format[0], &a_WavFormat.riffChunk.format[0] + std::size(a_WavFormat.riffChunk.format)).c_str());
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

        logger::log_info("<Wav> (\"%s\") subchunk1Id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.fmtChunk.chunkId[0], &a_WavFormat.fmtChunk.chunkId[0] + std::size(a_WavFormat.fmtChunk.chunkId)).c_str());
        logger::log_info("<Wav> (\"%s\") subchunk1Size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.chunkSize);
        logger::log_info("<Wav> (\"%s\") audioFormat: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
        if (a_WavFormat.fmtChunk.audioFormat != WAVE_FORMAT_PCM)
            logger::log_error("<Wav> (\"%s\") does not have the format 1 (PCM). This indicates there is probably some form of compression (%i).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.audioFormat);
        logger::log_info("<Wav> (\"%s\") numChannels: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.numChannels);
        logger::log_info("<Wav> (\"%s\") sampleRate: %ihz.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
        if (a_WavFormat.fmtChunk.sampleRate != 44100)
            logger::log_error("<Wav> (\"%s\") does not have a 44100 hz sample rate (%ihz).", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.sampleRate);
        logger::log_info("<Wav> (\"%s\") byteRate: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.byteRate);
        logger::log_info("<Wav> (\"%s\") blockAlign: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.blockAlign);
        logger::log_info("<Wav> (\"%s\") bitsPerSample: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.fmtChunk.bitsPerSample);
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
        a_WavFormat.dataChunk.data = reinterpret_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(sizeof(a_WavFormat.dataChunk.data) * a_ChunkSize)); // Set aside sound buffer space.

        if (a_WavFormat.dataChunk.data != nullptr)
            memcpy(a_WavFormat.dataChunk.data, a_Data, sizeof(a_WavFormat.dataChunk.data) * a_ChunkSize);

        a_WavFormat.filledDataChunk = true;

        logger::log_info("<Wav> (\"%s\") subchunk2Id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.dataChunk.chunkId[0], &a_WavFormat.dataChunk.chunkId[0] + std::size(a_WavFormat.dataChunk.chunkId)).c_str());
        logger::log_info("<Wav> (\"%s\") subchunk2Size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.dataChunk.chunkSize);
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
        a_WavFormat.acidChunk.typeOfFile = a_TypeOfFile;
        a_WavFormat.acidChunk.rootNote = a_RootNote;
        a_WavFormat.acidChunk.num_of_beats = a_NumOfBeats;
        a_WavFormat.acidChunk.meter_denominator = a_MeterDenominator;
        a_WavFormat.acidChunk.meter_numerator = a_MeterNumerator;
        a_WavFormat.acidChunk.tempo = a_Tempo;

        a_WavFormat.filledAcidChunk = true;

        logger::log_info("<Wav> (\"%s\") subchunk3Id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.acidChunk.chunkId[0], &a_WavFormat.acidChunk.chunkId[0] + std::size(a_WavFormat.acidChunk.chunkId)).c_str());
        logger::log_info("<Wav> (\"%s\") subchunk3Size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.chunkSize);
        logger::log_info("<Wav> (\"%s\") type_of_file: 0x%x.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.typeOfFile);
        logger::log_info("<Wav> (\"%s\") root_note: 0x%x.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.rootNote);
        logger::log_info("<Wav> (\"%s\") num_of_beats: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.num_of_beats);
        logger::log_info("<Wav> (\"%s\") meter_denominator: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.meter_denominator);
        logger::log_info("<Wav> (\"%s\") meter_numerator: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.meter_numerator);
        logger::log_info("<Wav> (\"%s\") tempo: %fbpm.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.acidChunk.tempo);
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

        logger::log_info("<Wav> (\"%s\") subchunk4Id: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.chunkId[0], &a_WavFormat.bextChunk.chunkId[0] + std::size(a_WavFormat.bextChunk.chunkId)).c_str());
        logger::log_info("<Wav> (\"%s\") subchunk4Size: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.chunkSize);
        logger::log_info("<Wav> (\"%s\") description: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.description[0], &a_WavFormat.bextChunk.description[0] + std::size(a_WavFormat.bextChunk.description)).c_str());
        logger::log_info("<Wav> (\"%s\") originator: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.originator[0], &a_WavFormat.bextChunk.originator[0] + std::size(a_WavFormat.bextChunk.originator)).c_str());
        logger::log_info("<Wav> (\"%s\") originator_reference: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.originator_reference[0], &a_WavFormat.bextChunk.originator_reference[0] + std::size(a_WavFormat.bextChunk.originator_reference)).c_str());
        logger::log_info("<Wav> (\"%s\") origination_date: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.origination_date[0], &a_WavFormat.bextChunk.origination_date[0] + std::size(a_WavFormat.bextChunk.origination_date)).c_str());
        logger::log_info("<Wav> (\"%s\") origination_time: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.origination_time[0], &a_WavFormat.bextChunk.origination_time[0] + std::size(a_WavFormat.bextChunk.origination_time)).c_str());
        logger::log_info("<Wav> (\"%s\") time_reference_low: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.time_reference_low);
        logger::log_info("<Wav> (\"%s\") time_reference_high: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.time_reference_high);
        logger::log_info("<Wav> (\"%s\") version: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.version);
        logger::log_info("<Wav> (\"%s\") umid: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.umid[0], &a_WavFormat.bextChunk.umid[0] + std::size(a_WavFormat.bextChunk.umid)).c_str());
        logger::log_info("<Wav> (\"%s\") loudness_value: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.loudness_value);
        logger::log_info("<Wav> (\"%s\") loudness_range: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.loudness_range);
        logger::log_info("<Wav> (\"%s\") max_true_peak_level: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_true_peak_level);
        logger::log_info("<Wav> (\"%s\") max_momentary_loudness: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_momentary_loudness);
        logger::log_info("<Wav> (\"%s\") max_short_term_loudness: %i.", a_WavFormat.m_FilePath.c_str(), a_WavFormat.bextChunk.max_short_term_loudness);
        logger::log_info("<Wav> (\"%s\") reserved: %s.", a_WavFormat.m_FilePath.c_str(), std::string(&a_WavFormat.bextChunk.reserved[0], &a_WavFormat.bextChunk.reserved[0] + std::size(a_WavFormat.bextChunk.reserved)).c_str());
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