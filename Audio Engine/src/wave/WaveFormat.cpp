#include "wave/WaveFormat.h"

#include "Includes.h"

namespace uaudio
{
    WaveFormat::WaveFormat(const WaveFormat& rhs) : m_FilePath(rhs.m_FilePath)
    {
        for (auto* chunk : rhs.m_Chunks)
        {
            Chunk_Data* chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(chunk->chunkSize + sizeof(Chunk_Data)));
            if (chunk_data != nullptr)
            {
                memcpy(chunk_data, chunk, chunk->chunkSize + sizeof(Chunk_Data));
                m_Chunks.push_back(chunk_data);
            }
        }
    }

    WaveFormat::~WaveFormat()
    {
        for (int32_t i = static_cast<uint32_t>(m_Chunks.size()) - 1; i > -1; i--)
            UAUDIO_DEFAULT_FREE(m_Chunks[i]);
        m_Chunks.clear();
    }

    WaveFormat& WaveFormat::operator=(const WaveFormat& rhs)
    {
        if (&rhs != this)
        {
            m_FilePath = std::string(rhs.m_FilePath);
            for (auto* chunk : rhs.m_Chunks)
            {
                Chunk_Data* chunk_data = reinterpret_cast<Chunk_Data*>(UAUDIO_DEFAULT_ALLOC(chunk->chunkSize + sizeof(Chunk_Data)));
                if (chunk_data != nullptr)
                {
                    memcpy(chunk_data, chunk, chunk->chunkSize + sizeof(Chunk_Data));
                    m_Chunks.push_back(chunk_data);
                }
            }
        }
        return *this;
    }
}
