#include <algorithm>

#include <wave/WaveFile.h>

#include <wave/WaveConverter.h>
#include <utils/Logger.h>

#include "wave/WaveReader.h"

namespace uaudio
{
    WaveFile::WaveFile() = default;

    WaveFile::WaveFile(const char *a_FilePath, WAVE_CONFIG a_Config) : m_Config(a_Config)
    {
        LoadSound(a_FilePath);
    }

    WaveFile::WaveFile(const WaveFile &rhs)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_WaveFile = rhs.m_WaveFile;
        m_Config = rhs.m_Config;
    }

    WaveFile &WaveFile::operator=(const WaveFile &rhs)
    {
        if (&rhs != this)
        {
            m_Looping = rhs.m_Looping;
            m_Volume = rhs.m_Volume;
            m_WaveFile = rhs.m_WaveFile;
            m_Config = rhs.m_Config;
        }
        return *this;
    }

    /// <summary>
    /// Loads the sound.
    /// </summary>
    /// <param name="a_FilePath">The path to the file.</param>
    void WaveFile::LoadSound(const char *a_FilePath)
    {
        m_WaveFile = {};

        WaveReader::LoadSound(a_FilePath, m_WaveFile, m_File, m_Config);
    }

    /// <summary>
    /// Saves the file.
    /// </summary>
    /// <param name="a_FilePath">The path to the file.</param>
    void WaveFile::Save(const char *a_FilePath)
    {
        m_WaveFile.CalculateRiffChunkSize(m_Config);

        FILE *file;

        // Open the file.
        fopen_s(&file, a_FilePath, "wb");
        if (file == nullptr)
        {
            logger::log_warning("<WaveReader> Failed saving file: (\"%s\").", a_FilePath);
            return;
        }

        if (m_Config.HasChunk(CHUNK_FLAG_RIFF))
            m_WaveFile.riffChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_FMT))
            m_WaveFile.fmtChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_DATA))
            m_WaveFile.dataChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_ACID))
            m_WaveFile.acidChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_BEXT))
            m_WaveFile.bextChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_FACT))
            m_WaveFile.factChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_SMPL))
            m_WaveFile.smplChunk.Write(file);
        if (m_Config.HasChunk(CHUNK_FLAG_CUE))
            m_WaveFile.cueChunk.Write(file);

        fclose(file);
        file = nullptr;
        logger::log_success("<WaveReader> Saved file: (\"%s\").", a_FilePath);
    }

    WaveFile::~WaveFile()
    {
    }

    /// <summary>
    /// Reads a part of the data array of the wave file.
    /// </summary>
    /// <param name="a_StartingPoint">The starting point of where to read from.</param>
    /// <param name="a_ElementCount">The element count of which to search for (will be reduced when reaching end of file)</param>
    /// <param name="a_Buffer">The buffer that will store the data.</param>
    void WaveFile::Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_Buffer) const
    {
        // NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
        if (a_StartingPoint + a_ElementCount >= m_WaveFile.dataChunk.chunkSize)
        {
            const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WaveFile.dataChunk.chunkSize);
            a_ElementCount = new_size;
        }
        a_Buffer = m_WaveFile.dataChunk.data + a_StartingPoint;
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
    /// <param name="a_Duration"></param>
    /// <returns></returns>
    std::string WaveFile::FormatDuration(float a_Duration, bool a_Miliseconds)
    {
        const uint32_t hours = static_cast<uint32_t>(a_Duration) / 3600;
        const uint32_t minutes = (static_cast<uint32_t>(a_Duration) - (hours * 3600)) / 60;
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
               std::string(seconds_string) + (a_Miliseconds ? ":" + std::string(milliseconds_string) : "");
    }

    /// <summary>
    /// Checks if a starting point is at or above the end of the file.
    /// </summary>
    /// <param name="a_StartingPoint"></param>
    /// <returns></returns>
    bool WaveFile::IsEndOfFile(uint32_t a_StartingPoint) const
    {
        return a_StartingPoint >= m_WaveFile.dataChunk.chunkSize;
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
    /// Returns the wav file.
    /// </summary>
    /// <returns></returns>
    const WaveFormat &WaveFile::GetWavFormat() const
    {
        return m_WaveFile;
    }

    /// <summary>
    /// Returns the sound title.
    /// </summary>
    /// <returns></returns>
    const char *WaveFile::GetSoundTitle() const
    {
        return m_WaveFile.m_FilePath.c_str();
    }
}