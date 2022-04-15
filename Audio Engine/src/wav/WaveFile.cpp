#include <algorithm>

#include <wav/WaveFile.h>

#include <wav/WaveConverter.h>
#include <utils/Logger.h>

#include "wav/WaveReader.h"

namespace uaudio
{
    WaveFile::WaveFile() = default;

    WaveFile::WaveFile(const char* a_FilePath)
    {
        LoadSound(a_FilePath);
    }

    WaveFile::WaveFile(const WaveFile& rhs)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_File = rhs.m_File;
        m_WavFile = rhs.m_WavFile;
    }

    WaveFile& WaveFile::operator=(const WaveFile& rhs)
    {
        if (&rhs != this)
        {
            m_Looping = rhs.m_Looping;
            m_Volume = rhs.m_Volume;
            m_File = rhs.m_File;
            m_WavFile = rhs.m_WavFile;
        }
        return *this;
    }

    /// <summary>
    /// Loads the sound.
    /// </summary>
    /// <param name="a_FilePath">The path to the file.</param>
    void WaveFile::LoadSound(const char* a_FilePath)
    {
        m_WavFile = {};

        m_WavFile = WaveReader::LoadSound(a_FilePath);
    }

    WaveFile::~WaveFile()
    {
        if (m_File != nullptr)
            fclose(m_File);
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
        if (a_StartingPoint + a_ElementCount >= m_WavFile.dataChunk.chunkSize)
        {
            const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WavFile.dataChunk.chunkSize);
            a_ElementCount = new_size;
        }
        a_Buffer = m_WavFile.dataChunk.data + a_StartingPoint;
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
    std::string WaveFile::FormatDuration(float a_Duration)
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
            std::string(seconds_string) +
            ":" +
            std::string(milliseconds_string);
    }

    /// <summary>
    /// Checks if a starting point is at or above the end of the file.
    /// </summary>
    /// <param name="a_StartingPoint"></param>
    /// <returns></returns>
    bool WaveFile::IsEndOfFile(uint32_t a_StartingPoint) const
    {
        return a_StartingPoint >= m_WavFile.dataChunk.chunkSize;
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
    const WavFormat& WaveFile::GetWavFormat() const
    {
        return m_WavFile;
    }

    /// <summary>
    /// Returns the sound title.
    /// </summary>
    /// <returns></returns>
    const char* WaveFile::GetSoundTitle() const
    {
        return m_WavFile.m_FilePath.c_str();
    }
}