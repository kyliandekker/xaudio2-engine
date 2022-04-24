#include <uaudio/wave/high_level/WaveFile.h>

#include <uaudio/utils/Utils.h>
#include <uaudio/wave/low_level/WaveConverter.h>
#include <uaudio/wave/low_level/WaveReader.h>

#include "uaudio/wave/high_level/WaveChunks.h"

namespace uaudio
{
    WaveFile::WaveFile() = default;

    WaveFile::WaveFile(const char *a_FilePath, const WaveConfig &a_WaveConfig)
    {
        m_WaveFormat = {};
        WaveReader::LoadSound(a_FilePath, m_WaveFormat, m_File, a_WaveConfig);
    }

    WaveFile::WaveFile(const WaveFile &rhs)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_WaveFormat = rhs.m_WaveFormat;
        m_StartPosition = rhs.m_StartPosition;
        m_EndPosition = rhs.m_EndPosition;
    }

    WaveFile &WaveFile::operator=(const WaveFile &rhs)
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
    /// <param name="a_DataBuffer">The buffer that will store the data.</param>
    void WaveFile::Read(uint32_t a_StartingPoint, uint32_t &a_ElementCount, unsigned char *&a_DataBuffer) const
    {
        // NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
        if (a_StartingPoint + a_ElementCount >= m_WaveFormat.GetChunkSize(DATA_CHUNK_ID))
        {
            const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WaveFormat.GetChunkSize(DATA_CHUNK_ID));
            a_ElementCount = new_size;
        }
        a_DataBuffer = m_WaveFormat.GetChunkFromData<DATA_Chunk>(DATA_CHUNK_ID).data + a_StartingPoint;
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
    const WaveFormat &WaveFile::GetWaveFormat() const
    {
        return m_WaveFormat;
    }
}
