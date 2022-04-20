#include "wave/WaveFormat.h"

namespace uaudio
{
    WaveFormat::WaveFormat(const WaveFormat& rhs) : m_FilePath(rhs.m_FilePath), m_Chunks(rhs.m_Chunks)
    { }

    WaveFormat::~WaveFormat()
    {
        // TODO: Remove chunks.
        //for (auto* chunk : m_Chunks)
        //    free(chunk);
    }

    WaveFormat& WaveFormat::operator=(const WaveFormat& rhs)
    {
        if (&rhs != this)
        {
            m_FilePath = std::string(rhs.m_FilePath);
            m_Chunks = rhs.m_Chunks;
        }
        return *this;
    }
}
