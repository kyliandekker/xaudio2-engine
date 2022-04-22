#include <wave/WaveFile.h>
#include <utils/Logger.h>

#include "utils/Utils.h"
#include "wave/WaveConverter.h"
#include "wave/WaveReader.h"

namespace uaudio
{
    WaveFile::WaveFile() = default;

    WaveFile::WaveFile(const char* a_FilePath, const Wave_Config& a_WaveConfig)
    {
        m_WaveFormat = {};
        WaveReader::LoadSound(a_FilePath, m_WaveFormat, m_File, a_WaveConfig);

        ConvertLoadedSound(a_WaveConfig);
    }

    WaveFile::WaveFile(const WaveFile& rhs)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_WaveFormat = rhs.m_WaveFormat;
        m_StartPosition = rhs.m_StartPosition;
        m_EndPosition = rhs.m_EndPosition;
    }

    WaveFile& WaveFile::operator=(const WaveFile& rhs)
    {
        if (&rhs != this)
        {
            m_Looping = rhs.m_Looping;
            m_Volume = rhs.m_Volume;
            m_WaveFormat = rhs.m_WaveFormat;
            m_StartPosition = rhs.m_StartPosition;
            m_EndPosition = rhs.m_EndPosition;
        }
        return *this;
    }

    /// <summary>
    /// Reads a part of the data array of the wave file.
    /// </summary>
    /// <param name="a_StartingPoint">The starting point of where to read from.</param>
    /// <param name="a_ElementCount">The element count of which to search for (will be reduced when reaching end of file)</param>
    /// <param name="a_Buffer">The buffer that will store the data.</param>
    void WaveFile::Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_Buffer) const
    {
        // NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
        if (a_StartingPoint + a_ElementCount >= m_WaveFormat.GetChunkSize(DATA_CHUNK_ID))
        {
            const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WaveFormat.GetChunkSize(DATA_CHUNK_ID));
            a_ElementCount = new_size;
        }
        a_Buffer = m_WaveFormat.GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID).data + a_StartingPoint;
    }

    /// <summary>
    /// Returns the duration in seconds.
    /// </summary>
    /// <returns></returns>
    float WaveFile::GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate)
    {
        return static_cast<float>(a_ChunkSize) / static_cast<float>(a_ByteRate);
    }

    /// <summary>
    /// Returns the duration in minute:seconds.
    /// </summary>
    /// <param name="a_Duration">Duration in seconds.</param>
    /// <param name="a_Milliseconds">Whether or not milliseconds should be shown.</param>
    /// <returns></returns>
    std::string WaveFile::FormatDuration(float a_Duration, bool a_Milliseconds)
    {
        const uint32_t hours = utils::SecondsToHours(a_Duration);
        const uint32_t minutes = utils::SecondsToMinutes(a_Duration);
        const uint32_t seconds = static_cast<uint32_t>(a_Duration) % 60;
        const uint32_t total = (hours * 3600) + (minutes * 60) + seconds;
        const float milliseconds_float = a_Duration - static_cast<float>(total);
        const uint32_t milliseconds = static_cast<uint32_t>(milliseconds_float * 1000);

        char hours_string[32], minutes_string[32], seconds_string[32], milliseconds_string[32];
        sprintf_s(hours_string, "%02d", hours);
        sprintf_s(minutes_string, "%02d", minutes);
        sprintf_s(seconds_string, "%02d", seconds);
        sprintf_s(milliseconds_string, "%03d", milliseconds);
        return std::string(hours_string) +
            ":" +
            std::string(minutes_string) +
            ":" +
            std::string(seconds_string) + (a_Milliseconds ? ":" + std::string(milliseconds_string) : "");
    }

    /// <summary>
    /// Checks if a starting point is at or above the end of the file.
    /// </summary>
    /// <param name="a_StartingPoint"></param>
    /// <returns></returns>
    bool WaveFile::IsEndOfBuffer(uint32_t a_StartingPoint) const
    {
        return a_StartingPoint >= m_EndPosition;
    }

    /// <summary>
    /// Returns whether the sound needs to repeat itself.
    /// </summary>
    /// <returns></returns>
    bool WaveFile::IsLooping() const
    {
        return m_Looping;
    }

    /// <summary>
    /// Sets whether the sound should repeat itself.
    /// </summary>
    /// <param name="a_Looping"></param>
    void WaveFile::SetLooping(bool a_Looping)
    {
        m_Looping = a_Looping;
    }

    /// <summary>
    /// Sets the volume of the sound.
    /// </summary>
    /// <param name="a_Volume"></param>
    void WaveFile::SetVolume(float a_Volume)
    {
        m_Volume = a_Volume;
    }

    /// <summary>
    /// Returns the volume of the sound.
    /// </summary>
    /// <returns></returns>
    float WaveFile::GetVolume() const
    {
        return m_Volume;
    }

    /// <summary>
    /// Sets the end position of the wave file.
    /// </summary>
    /// <param name="a_EndPosition">The end position.</param>
    void WaveFile::SetEndPosition(uint32_t a_EndPosition)
    {
        a_EndPosition = utils::clamp<uint32_t>(a_EndPosition, 0, m_WaveFormat.GetChunkSize(DATA_CHUNK_ID));
        m_EndPosition = a_EndPosition;
    }

    /// <summary>
    /// Returns the end position of the wave file.
    /// </summary>
    /// <returns>The end position.</returns>
    uint32_t WaveFile::GetEndPosition() const
    {
        return m_EndPosition;
    }

    /// <summary>
    /// Sets the start position of the wave file.
    /// </summary>
    /// <param name="a_StartPosition">The start position.</param>
    void WaveFile::SetStartPosition(uint32_t a_StartPosition)
    {
        a_StartPosition = utils::clamp<uint32_t>(a_StartPosition, 0, m_WaveFormat.GetChunkSize(DATA_CHUNK_ID));
        m_StartPosition = a_StartPosition;
    }

    /// <summary>
    /// Returns the start position of the wave file.
    /// </summary>
    /// <returns>The start position.</returns>
    uint32_t WaveFile::GetStartPosition() const
    {
        return m_StartPosition;
    }

    /// <summary>
    /// Returns the wav file.
    /// </summary>
    /// <returns></returns>
    const WaveFormat& WaveFile::GetWaveFormat() const
    {
        return m_WaveFormat;
    }

    void WaveFile::ConvertLoadedSound(const Wave_Config& a_WaveConfig)
    {
        MonoStereoConfig(a_WaveConfig);
        BitsPerSampleConfig(a_WaveConfig);
        LoopPositionsConfig(a_WaveConfig);
    }

    /// <summary>
    /// Converts the audio data to the right channel setting if that has been stated in the config.
    /// </summary>
    /// <param name="a_WaveConfig">The wave config.</param>
    void WaveFile::MonoStereoConfig(const Wave_Config& a_WaveConfig)
    {
        FMT_Chunk fmt_chunk = m_WaveFormat.GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

        if (a_WaveConfig.numChannels == WAVE_CHANNELS_STEREO || a_WaveConfig.numChannels == WAVE_CHANNELS_MONO)
            if (fmt_chunk.numChannels != a_WaveConfig.numChannels)
            {
                Chunk_Data* data_chunk_data = nullptr;

                const DATA_Chunk data_chunk = m_WaveFormat.GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID);
                uint32_t new_size = m_WaveFormat.GetChunkSize(DATA_CHUNK_ID);

                unsigned char* data = nullptr;
                if (a_WaveConfig.numChannels == WAVE_CHANNELS_STEREO)
                    data = conversion::ConvertMonoToStereo(data_chunk.data, new_size, fmt_chunk.blockAlign);
                else if (a_WaveConfig.numChannels == WAVE_CHANNELS_MONO)
                    data = conversion::ConvertStereoToMono(data_chunk.data, new_size, fmt_chunk.blockAlign);

                data_chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(new_size + sizeof(Chunk_Data)));
                if (data_chunk_data != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(data_chunk_data->chunk_id, DATA_CHUNK_ID, CHUNK_ID_SIZE);
                    data_chunk_data->chunkSize = new_size;
                    UAUDIO_DEFAULT_MEMCPY(utils::add(data_chunk_data, sizeof(Chunk_Data)), data, new_size);
                }

                fmt_chunk.numChannels = a_WaveConfig.numChannels;
                fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
                fmt_chunk.byteRate = fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;

                Chunk_Data* fmt_chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(sizeof(FMT_Chunk) + sizeof(Chunk_Data)));
                if (fmt_chunk_data != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(fmt_chunk_data->chunk_id, FMT_CHUNK_ID, CHUNK_ID_SIZE);
                    fmt_chunk_data->chunkSize = m_WaveFormat.GetChunkSize(FMT_CHUNK_ID);
                    UAUDIO_DEFAULT_MEMCPY(utils::add(fmt_chunk_data, sizeof(Chunk_Data)), reinterpret_cast<const char*>(&fmt_chunk), sizeof(FMT_Chunk));
                }

                if (data_chunk_data != nullptr && fmt_chunk_data != nullptr)
                {
                    m_WaveFormat.RemoveChunk(FMT_CHUNK_ID);
                    m_WaveFormat.m_Chunks.push_back(fmt_chunk_data);

                    m_WaveFormat.RemoveChunk(DATA_CHUNK_ID);
                    m_WaveFormat.m_Chunks.push_back(data_chunk_data);
                }
            }
    }

    /// <summary>
    /// Sets looping points if that has been stated in the config.
    /// </summary>
    /// <param name="a_WaveConfig">The wave config.</param>
    void WaveFile::LoopPositionsConfig(const Wave_Config&)
    {
        SetStartPosition(0);
        SetEndPosition(m_WaveFormat.GetChunkSize(DATA_CHUNK_ID));
        //if (a_WaveConfig.setLoopPoints != LOOP_POINT_SETTING::LOOP_POINT_SETTING_NONE)
        //{
        //    const SMPL_Chunk smpl_chunk = m_WaveFormat.GetChunkFromData<SMPL_Chunk>(SMPL_CHUNK_ID);
        //    if (smpl_chunk.num_sample_loops > 0)
        //    {
        //        if (a_WaveConfig.setLoopPoints == LOOP_POINT_SETTING::LOOP_POINT_SETTING_BOTH || a_WaveConfig.setLoopPoints == LOOP_POINT_SETTING::LOOP_POINT_SETTING_START)
        //            SetStartPosition(smpl_chunk.samples[0].start * 2);
        //        if (a_WaveConfig.setLoopPoints == LOOP_POINT_SETTING::LOOP_POINT_SETTING_BOTH || a_WaveConfig.setLoopPoints == LOOP_POINT_SETTING::LOOP_POINT_SETTING_END)
        //            SetEndPosition(smpl_chunk.samples[0].end * 2);
        //    }
        //}
    }

    /// <summary>
    /// Converts the bits per sample if that has been stated in the config.
    /// </summary>
    /// <param name="a_WaveConfig">The wave config.</param>
    void WaveFile::BitsPerSampleConfig(const Wave_Config& a_WaveConfig)
    {
        FMT_Chunk fmt_chunk = m_WaveFormat.GetChunkFromData<FMT_Chunk>(FMT_CHUNK_ID);

        if (a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_16 || a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_24 || a_WaveConfig.bitsPerSample == WAVE_BITS_PER_SAMPLE_32)
	        if (fmt_chunk.bitsPerSample != a_WaveConfig.bitsPerSample)
	        {
	            Chunk_Data* data_chunk_data = nullptr;

	            const DATA_Chunk data_chunk = m_WaveFormat.GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID);
	            uint32_t data_chunk_size = m_WaveFormat.GetChunkSize(DATA_CHUNK_ID);

                switch (a_WaveConfig.bitsPerSample)
                {
					case WAVE_BITS_PER_SAMPLE_16:
	                {
		                if (fmt_chunk.bitsPerSample == WAVE_BITS_PER_SAMPLE_24)
		                {
                            data_chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(conversion::Calculate24To16Size(data_chunk_size) + sizeof(Chunk_Data)));
                            conversion::Convert24To16(reinterpret_cast<unsigned char*>(utils::add(data_chunk_data, sizeof(Chunk_Data))), data_chunk.data, data_chunk_size);
                            break;
		                }
                        else if (fmt_chunk.bitsPerSample == WAVE_BITS_PER_SAMPLE_32)
                        {
                            data_chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(conversion::Calculate32To16Size(data_chunk_size) + sizeof(Chunk_Data)));
                            conversion::Convert32To16(reinterpret_cast<unsigned char*>(utils::add(data_chunk_data, sizeof(Chunk_Data))), data_chunk.data, data_chunk_size);
                            break;
                        }
                        break;
	                }
                    default: return;
                }

                if (data_chunk_data != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(data_chunk_data->chunk_id, DATA_CHUNK_ID, CHUNK_ID_SIZE);
                    data_chunk_data->chunkSize = data_chunk_size;
                }
                fmt_chunk.audioFormat = WAV_FORMAT_PCM;
                fmt_chunk.bitsPerSample = a_WaveConfig.bitsPerSample;
                fmt_chunk.blockAlign = fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;
                fmt_chunk.byteRate = fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8;

                Chunk_Data* fmt_chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(sizeof(FMT_Chunk) + sizeof(Chunk_Data)));
                if (fmt_chunk_data != nullptr)
                {
                    UAUDIO_DEFAULT_MEMCPY(fmt_chunk_data->chunk_id, FMT_CHUNK_ID, CHUNK_ID_SIZE);
                    fmt_chunk_data->chunkSize = m_WaveFormat.GetChunkSize(FMT_CHUNK_ID);
                    UAUDIO_DEFAULT_MEMCPY(utils::add(fmt_chunk_data, sizeof(Chunk_Data)), reinterpret_cast<const char*>(&fmt_chunk), sizeof(FMT_Chunk));
                }

                if (data_chunk_data != nullptr)
                {
                    m_WaveFormat.RemoveChunk(FMT_CHUNK_ID);
                    m_WaveFormat.m_Chunks.push_back(fmt_chunk_data);

                    m_WaveFormat.RemoveChunk(DATA_CHUNK_ID);
                    m_WaveFormat.m_Chunks.push_back(data_chunk_data);
                }
	        }
    }

    /// <summary>
    /// Returns the sound title.
    /// </summary>
    /// <returns></returns>
    const char* WaveFile::GetSoundTitle() const
    {
        return m_WaveFormat.m_FilePath.c_str();
    }
}
