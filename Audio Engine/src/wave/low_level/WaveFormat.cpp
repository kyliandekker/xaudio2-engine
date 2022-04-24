#include <uaudio/wave/low_level/WaveFormat.h>

#include <uaudio/Includes.h>

#include "uaudio/wave/high_level/WaveChunks.h"
#include "uaudio/wave/low_level/WaveConverter.h"

namespace uaudio
{
    WaveFormat::WaveFormat(const WaveFormat &rhs)
    {
        SetFileName(rhs.m_FilePath);
        for (auto *chunk : rhs.m_Chunks)
        {
            WaveChunkData *chunk_data = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(chunk->chunkSize + sizeof(WaveChunkData)));
            if (chunk_data != nullptr)
            {
                UAUDIO_DEFAULT_MEMCPY(chunk_data, chunk, chunk->chunkSize + sizeof(WaveChunkData));
                m_Chunks.push_back(chunk_data);
            }
        }
    }

    WaveFormat::~WaveFormat()
    {
        for (int32_t i = static_cast<uint32_t>(m_Chunks.size()) - 1; i > -1; i--)
            UAUDIO_DEFAULT_FREE(m_Chunks[i]);
        m_Chunks.clear();

        UAUDIO_DEFAULT_FREE(m_FilePath);
    }

    WaveFormat &WaveFormat::operator=(const WaveFormat &rhs)
    {
        if (&rhs != this)
        {
            SetFileName(rhs.m_FilePath);
            for (auto *chunk : rhs.m_Chunks)
            {
                WaveChunkData *chunk_data = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(chunk->chunkSize + sizeof(WaveChunkData)));
                if (chunk_data != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(chunk_data, chunk, chunk->chunkSize + sizeof(WaveChunkData));
                    m_Chunks.push_back(chunk_data);
                }
            }
        }
        return *this;
    }

    /// <summary>
    /// Sets the file path.
    /// </summary>
    /// <param name="a_FilePath">The file path it needs to be set to.</param>
    void WaveFormat::SetFileName(const char *a_FilePath)
    {
        UAUDIO_DEFAULT_FREE(m_FilePath);
        if (a_FilePath != nullptr)
        {
            const size_t len = strlen(a_FilePath);
            m_FilePath = reinterpret_cast<char *>(UAUDIO_DEFAULT_ALLOC(len + 1));
            if (m_FilePath != nullptr)
            {
                strcpy_s(m_FilePath, len + 1, a_FilePath);
                m_FilePath[len] = '\0';
            }
        }
    }

    /// <summary>
    /// Converts the sound according to the wave config.
    /// </summary>
    /// <param name="a_WaveConfig">The config containing specific loading instructions.</param>
    void WaveFormat::ConfigConversion(WaveConfig &a_WaveConfig)
    {
        BitsPerSampleConvert(a_WaveConfig);
        MonoStereoConvert(a_WaveConfig);
    }

    /// <summary>
    /// Converts the bits per sample if that has been stated in the config.
    /// </summary>
    /// <param name="a_WaveConfig">The config containing specific loading instructions.</param>
    void WaveFormat::BitsPerSampleConvert(WaveConfig &a_WaveConfig)
    {
        FMT_Chunk fmt_chunk = GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

        if (a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_16 || a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_24 || a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_32)
            if (fmt_chunk.bitsPerSample != a_WaveConfig.bitsPerSample)
            {
                WaveChunkData *data_WaveChunkData = nullptr;

                const DATA_Chunk data_chunk = GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID);
                uint32_t data_chunk_size = GetChunkSize(DATA_CHUNK_ID);

                switch (a_WaveConfig.bitsPerSample)
                {
	                case WAVE_BITS_PER_SAMPLE_16:
	                {
	                    if (fmt_chunk.bitsPerSample == WAVE_BITS_PER_SAMPLE_24)
	                    {
	                        data_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(conversion::Calculate24To16Size(data_chunk_size) + sizeof(WaveChunkData)));
	                        conversion::Convert24To16(reinterpret_cast<unsigned char *>(utils::add(data_WaveChunkData, sizeof(WaveChunkData))), data_chunk.data, data_chunk_size);

	                        // const SMPL_Chunk smpl_chunk = GetChunkFromData<SMPL_Chunk>(SMPL_CHUNK_ID);
	                        // m_StartPosition = conversion::Calculate24To16Size(smpl_chunk.samples[0].start) * sizeof(uint24_t);
	                        // m_EndPosition = conversion::Calculate24To16Size(smpl_chunk.samples[0].end) * sizeof(uint24_t);
	                        break;
	                    }
	                    else if (fmt_chunk.bitsPerSample == WAVE_BITS_PER_SAMPLE_32)
	                    {
	                        data_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(conversion::Calculate32To16Size(data_chunk_size) + sizeof(WaveChunkData)));
	                        conversion::Convert32To16(reinterpret_cast<unsigned char *>(utils::add(data_WaveChunkData, sizeof(WaveChunkData))), data_chunk.data, data_chunk_size);

	                        // const SMPL_Chunk smpl_chunk = GetChunkFromData<SMPL_Chunk>(SMPL_CHUNK_ID);
	                        // m_StartPosition = conversion::Calculate32To16Size(smpl_chunk.samples[0].start) * sizeof(uint32_t);
	                        // m_EndPosition = conversion::Calculate32To16Size(smpl_chunk.samples[0].end) * sizeof(uint32_t);
	                        break;
	                    }
	                    break;
	                }
	                default:
	                    return;
                }

                // TODO: Maybe go through all chunks to update positions in loop points?

                if (data_WaveChunkData != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(data_WaveChunkData->chunk_id, DATA_CHUNK_ID, CHUNK_ID_SIZE);
                    data_WaveChunkData->chunkSize = data_chunk_size;
                }
                fmt_chunk.audioFormat = WAV_FORMAT_PCM;
                fmt_chunk.bitsPerSample = a_WaveConfig.bitsPerSample;
                fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
                fmt_chunk.byteRate = fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;

                WaveChunkData *fmt_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(sizeof(FMT_Chunk) + sizeof(WaveChunkData)));
                if (fmt_WaveChunkData != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(fmt_WaveChunkData->chunk_id, FMT_CHUNK_ID, CHUNK_ID_SIZE);
                    fmt_WaveChunkData->chunkSize = GetChunkSize(FMT_CHUNK_ID);
                    UAUDIO_DEFAULT_MEMCPY(utils::add(fmt_WaveChunkData, sizeof(WaveChunkData)), reinterpret_cast<const char *>(&fmt_chunk), sizeof(FMT_Chunk));
                }

                if (data_WaveChunkData != nullptr)
                {
                    RemoveChunk(FMT_CHUNK_ID);
                    AddChunk(fmt_WaveChunkData);

                    RemoveChunk(DATA_CHUNK_ID);
                    AddChunk(data_WaveChunkData);
                }
            }
    }

    /// <summary>
    /// Converts the audio data to the right channel setting if that has been stated in the config.
    /// </summary>
    /// <param name="a_WaveConfig">The config containing specific loading instructions.</param>
    void WaveFormat::MonoStereoConvert(WaveConfig &a_WaveConfig)
    {
        FMT_Chunk fmt_chunk = GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

        // Check if the number of channels in the config is mono or stereo (anything other than 1 or 2 means unspecified)
        if (a_WaveConfig.numChannels == WAVE_CHANNELS_STEREO || a_WaveConfig.numChannels == WAVE_CHANNELS_MONO)
            // Check if the number of channels in the config and the number of channels in the chunk is equal.
            if (fmt_chunk.numChannels != a_WaveConfig.numChannels)
            {
                WaveChunkData *data_WaveChunkData = nullptr;
                const DATA_Chunk data_chunk = GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID);
                uint32_t data_chunk_size = GetChunkSize(DATA_CHUNK_ID);

                if (a_WaveConfig.numChannels == WAVE_CHANNELS_STEREO)
                {
                    data_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(conversion::CalculateMonoToStereoSize(data_chunk_size) + sizeof(WaveChunkData)));
                    conversion::ConvertMonoToStereo(reinterpret_cast<unsigned char *>(utils::add(data_WaveChunkData, sizeof(WaveChunkData))), data_chunk.data, data_chunk_size, fmt_chunk.blockAlign);
                }
                else if (a_WaveConfig.numChannels == WAVE_CHANNELS_MONO)
                {
                    data_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(conversion::CalculateStereoToMonoSize(data_chunk_size) + sizeof(WaveChunkData)));
                    conversion::ConvertStereoToMono(reinterpret_cast<unsigned char *>(utils::add(data_WaveChunkData, sizeof(WaveChunkData))), data_chunk.data, data_chunk_size, fmt_chunk.blockAlign);
                }

                if (data_WaveChunkData != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(data_WaveChunkData->chunk_id, DATA_CHUNK_ID, CHUNK_ID_SIZE);
                    data_WaveChunkData->chunkSize = data_chunk_size;
                }

                fmt_chunk.numChannels = a_WaveConfig.numChannels;
                fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
                fmt_chunk.byteRate = fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;

                WaveChunkData *fmt_WaveChunkData = reinterpret_cast<WaveChunkData *>(UAUDIO_DEFAULT_ALLOC(sizeof(FMT_Chunk) + sizeof(WaveChunkData)));
                if (fmt_WaveChunkData != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(fmt_WaveChunkData->chunk_id, FMT_CHUNK_ID, CHUNK_ID_SIZE);
                    fmt_WaveChunkData->chunkSize = GetChunkSize(FMT_CHUNK_ID);
                    UAUDIO_DEFAULT_MEMCPY(utils::add(fmt_WaveChunkData, sizeof(WaveChunkData)), reinterpret_cast<const char *>(&fmt_chunk), sizeof(FMT_Chunk));
                }

                if (data_WaveChunkData != nullptr && fmt_WaveChunkData != nullptr)
                {
                    RemoveChunk(FMT_CHUNK_ID);
                    AddChunk(fmt_WaveChunkData);

                    RemoveChunk(DATA_CHUNK_ID);
                    AddChunk(data_WaveChunkData);
                }
            }
    }
}
