#include <algorithm>
#include <utility>

#include <wave/WaveFile.h>
#include <utils/Logger.h>

#include "wave/WaveConverter.h"

namespace uaudio
{
    WaveFile::WaveFile() = default;

    WaveFile::WaveFile(const char *a_FilePath, std::vector<const char *> a_Chunks)
    {
        m_WaveFile = {};
        WaveReader::LoadSound(a_FilePath, m_WaveFile, m_File, std::move(a_Chunks));

        SetEndPosition(m_WaveFile.GetChunkSize(DATA_CHUNK_ID));
    }

    WaveFile::WaveFile(const WaveFile &rhs)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_WaveFile = rhs.m_WaveFile;
        m_StartPosition = rhs.m_StartPosition;
        m_EndPosition = rhs.m_EndPosition;
    }

    WaveFile &WaveFile::operator=(const WaveFile &rhs)
    {
        if (&rhs != this)
        {
            m_Looping = rhs.m_Looping;
            m_Volume = rhs.m_Volume;
            m_WaveFile = rhs.m_WaveFile;
            m_StartPosition = rhs.m_StartPosition;
            m_EndPosition = rhs.m_EndPosition;
        }
        return *this;
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
        if (a_StartingPoint + a_ElementCount >= m_WaveFile.GetChunkSize(DATA_CHUNK_ID))
        {
            const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WaveFile.GetChunkSize(DATA_CHUNK_ID));
            a_ElementCount = new_size;
        }
        a_Buffer = m_WaveFile.GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID).data + a_StartingPoint;
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
    /// <returns></returns>
    std::string WaveFile::FormatDuration(float a_Duration, bool a_Miliseconds)
    {
        const uint32_t hours = conversion::SecondsToHours(a_Duration);
        const uint32_t minutes = conversion::SecondsToMinutes(a_Duration);
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

    void WaveFile::SetEndPosition(uint32_t a_EndPosition)
    {
        m_EndPosition = a_EndPosition;
    }

    uint32_t WaveFile::GetEndPosition() const
    {
        return m_EndPosition;
    }

    void WaveFile::SetStartPosition(uint32_t a_StartPosition)
    {
        m_StartPosition = a_StartPosition;
    }

    uint32_t WaveFile::GetStartPosition() const
    {
        return m_StartPosition;
    }

    /// <summary>
    /// Returns the wav file.
    /// </summary>
    /// <returns></returns>
    const WaveFormat &WaveFile::GetWaveFormat() const
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
