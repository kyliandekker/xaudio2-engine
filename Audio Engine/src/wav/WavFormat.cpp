#include <wav/WavFormat.h>

namespace uaudio
{
    Chunk::Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize)
    {
        memcpy(chunkId, a_ChunkID, sizeof(chunkId));
        chunkSize = a_ChunkSize;
    }

    Chunk::Chunk(const Chunk &rhs)
    {
        memcpy(chunkId, chunkId, sizeof(chunkId));
        chunkSize = rhs.chunkSize;
    }

    Chunk::~Chunk() = default;

    Chunk &Chunk::operator=(const Chunk &rhs)
    {
        if (&rhs != this)
        {
            memcpy(chunkId, chunkId, sizeof(chunkId));
            chunkSize = rhs.chunkSize;
        }
        return *this;
    }

    RIFF_Chunk::RIFF_Chunk() : Chunk() {}

    RIFF_Chunk::RIFF_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char a_Format[CHUNK_ID_SIZE]) : Chunk(a_ChunkID, a_ChunkSize)
    {
        memcpy(format, a_Format, sizeof(format));
    }

    RIFF_Chunk::RIFF_Chunk(const RIFF_Chunk &rhs) : Chunk(rhs)
    {
        memcpy(format, rhs.format, sizeof(format));
    }

    RIFF_Chunk::~RIFF_Chunk() = default;

    RIFF_Chunk &RIFF_Chunk::operator=(const RIFF_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            memcpy(chunkId, rhs.format, sizeof(format));
        }
        return *this;
    }

    FMT_Chunk::FMT_Chunk() : Chunk() {}

    FMT_Chunk::FMT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample) : Chunk(a_ChunkID, a_ChunkSize), audioFormat(a_AudioFormat), numChannels(a_NumChannels), sampleRate(a_SampleRate), byteRate(a_ByteRate), blockAlign(a_BlockAlign), bitsPerSample(a_BitsPerSample)
    {
    }

    FMT_Chunk::FMT_Chunk(const FMT_Chunk &rhs) : Chunk(rhs)
    {
        audioFormat = rhs.audioFormat;
        numChannels = rhs.numChannels;
        sampleRate = rhs.sampleRate;
        byteRate = rhs.byteRate;
        blockAlign = rhs.blockAlign;
        bitsPerSample = rhs.bitsPerSample;
    }

    FMT_Chunk::~FMT_Chunk() = default;

    FMT_Chunk &FMT_Chunk::operator=(const FMT_Chunk &rhs)
    {
        Chunk::operator=(rhs);
        if (&rhs != this)
        {
            audioFormat = rhs.audioFormat;
            numChannels = rhs.numChannels;
            sampleRate = rhs.sampleRate;
            byteRate = rhs.byteRate;
            blockAlign = rhs.blockAlign;
            bitsPerSample = rhs.bitsPerSample;
        }
        return *this;
    }

    DATA_Chunk::DATA_Chunk() : Chunk() {}

    DATA_Chunk::DATA_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, unsigned char *a_Data) : Chunk(a_ChunkID, a_ChunkSize)
    {
        data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(sizeof(data) * a_ChunkSize)); // Set aside sound buffer space.

        if (data != nullptr)
            memcpy(data, a_Data, sizeof(unsigned char) * a_ChunkSize);
    }

    DATA_Chunk::DATA_Chunk(const DATA_Chunk &rhs) : Chunk(rhs)
    {
        data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(sizeof(data) * rhs.chunkSize)); // Set aside sound buffer space.

        if (data != nullptr)
            memcpy(data, rhs.data, sizeof(unsigned char) * rhs.chunkSize);
    }

    DATA_Chunk::~DATA_Chunk()
    {
        UAUDIO_DEFAULT_FREE(data);
    }

    DATA_Chunk &DATA_Chunk::operator=(const DATA_Chunk &rhs)
    {
        Chunk::operator=(rhs);
        if (&rhs != this)
        {
            data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(sizeof(rhs.data) * rhs.chunkSize)); // Set aside sound buffer space.

            if (data != nullptr)
                memcpy(data, rhs.data, sizeof(unsigned char) * rhs.chunkSize);
        }
        return *this;
    }

    ACID_Chunk::ACID_Chunk() : Chunk() {}

    ACID_Chunk::ACID_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint16_t a_Unknown1, float a_Unknown2, uint32_t a_NumBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo) : Chunk(a_ChunkID, a_ChunkSize),
                                                                                                                                                                                                                                                                          typeOfFile(a_TypeOfFile),
                                                                                                                                                                                                                                                                          rootNote(a_RootNote),
                                                                                                                                                                                                                                                                          unknown1(a_Unknown1),
                                                                                                                                                                                                                                                                          unknown2(a_Unknown2),
                                                                                                                                                                                                                                                                          num_of_beats(a_NumBeats),
                                                                                                                                                                                                                                                                          meter_denominator(a_MeterDenominator),
                                                                                                                                                                                                                                                                          meter_numerator(a_MeterNumerator),
                                                                                                                                                                                                                                                                          tempo(a_Tempo)
    {
    }

    ACID_Chunk::ACID_Chunk(const ACID_Chunk &rhs) : Chunk(rhs)
    {
        typeOfFile = rhs.typeOfFile;
        rootNote = rhs.rootNote;
        unknown1 = rhs.unknown1;
        unknown2 = rhs.unknown2;
        num_of_beats = rhs.num_of_beats;
        meter_denominator = rhs.meter_denominator;
        meter_numerator = rhs.meter_numerator;
        tempo = rhs.tempo;
    }

    ACID_Chunk::~ACID_Chunk() = default;

    ACID_Chunk &ACID_Chunk::operator=(const ACID_Chunk &rhs)
    {
        Chunk::operator=(rhs);
        if (&rhs != this)
        {
            typeOfFile = rhs.typeOfFile;
            rootNote = rhs.rootNote;
            unknown1 = rhs.unknown1;
            unknown2 = rhs.unknown2;
            num_of_beats = rhs.num_of_beats;
            meter_denominator = rhs.meter_denominator;
            meter_numerator = rhs.meter_numerator;
            tempo = rhs.tempo;
        }
        return *this;
    }

    BEXT_Chunk::BEXT_Chunk() : Chunk() {}

    BEXT_Chunk::BEXT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180]) : Chunk(a_ChunkID, a_ChunkSize),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             time_reference_low(a_TimeReferenceLow),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             time_reference_high(a_TimeReferenceHigh),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             version(a_Version),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             loudness_value(a_LoudnessValue),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             loudness_range(a_LoudnessRange),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             max_true_peak_level(a_MaxTruePeakLevel),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             max_momentary_loudness(a_MaxMomentaryLoudness),
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             max_short_term_loudness(a_MaxShortTermLoudness)
    {
        memcpy(description, a_Description, sizeof(description));
        memcpy(originator, a_Originator, sizeof(originator));
        memcpy(originator_reference, a_OriginatorReference, sizeof(originator_reference));
        memcpy(origination_date, a_OriginationDate, sizeof(origination_date));
        memcpy(origination_time, a_OriginationTime, sizeof(origination_time));
        memcpy(umid, a_Umid, sizeof(origination_time));
        memcpy(reserved, a_Reserved, sizeof(reserved));
    }

    BEXT_Chunk::BEXT_Chunk(const BEXT_Chunk &rhs) : Chunk(rhs)
    {
        memcpy(description, rhs.description, sizeof(description));
        memcpy(originator, rhs.originator, sizeof(originator));
        memcpy(originator_reference, rhs.originator_reference, sizeof(originator_reference));
        memcpy(origination_date, rhs.origination_date, sizeof(origination_date));
        memcpy(origination_time, rhs.origination_time, sizeof(origination_time));

        time_reference_low = rhs.time_reference_low;
        time_reference_high = rhs.time_reference_high;
        version = rhs.version;

        memcpy(umid, rhs.umid, sizeof(origination_time));

        loudness_value = rhs.loudness_value;
        loudness_range = rhs.loudness_range;
        max_true_peak_level = rhs.max_true_peak_level;
        max_momentary_loudness = rhs.max_momentary_loudness;
        max_short_term_loudness = rhs.max_short_term_loudness;

        memcpy(reserved, rhs.reserved, sizeof(reserved));
    }

    BEXT_Chunk::~BEXT_Chunk() = default;

    BEXT_Chunk &BEXT_Chunk::operator=(const BEXT_Chunk &rhs)
    {
        Chunk::operator=(rhs);
        if (&rhs != this)
        {
            memcpy(description, rhs.description, sizeof(description));
            memcpy(originator, rhs.originator, sizeof(originator));
            memcpy(originator_reference, rhs.originator_reference, sizeof(originator_reference));
            memcpy(origination_date, rhs.origination_date, sizeof(origination_date));
            memcpy(origination_time, rhs.origination_time, sizeof(origination_time));

            time_reference_low = rhs.time_reference_low;
            time_reference_high = rhs.time_reference_high;
            version = rhs.version;

            memcpy(umid, rhs.umid, sizeof(origination_time));

            loudness_value = rhs.loudness_value;
            loudness_range = rhs.loudness_range;
            max_true_peak_level = rhs.max_true_peak_level;
            max_momentary_loudness = rhs.max_momentary_loudness;
            max_short_term_loudness = rhs.max_short_term_loudness;

            memcpy(reserved, rhs.reserved, sizeof(reserved));
        }
        return *this;
    }

    WavFormat::WavFormat(const WavFormat &rhs)
    {
        riffChunk = rhs.riffChunk;
        fmtChunk = rhs.fmtChunk;
        dataChunk = rhs.dataChunk;
        acidChunk = rhs.acidChunk;
        bextChunk = rhs.bextChunk;

        m_FilePath = rhs.m_FilePath;

        for (auto &chunk : rhs.otherChunks)
            otherChunks.push_back(chunk);

        filledRiffChunk = rhs.filledRiffChunk;
        filledFmtChunk = rhs.filledFmtChunk;
        filledDataChunk = rhs.filledDataChunk;
        filledAcidChunk = rhs.filledAcidChunk;
        filledBextChunk = rhs.filledBextChunk;
    }

    WavFormat::~WavFormat() = default;

    WavFormat &WavFormat::operator=(const WavFormat &rhs)
    {
        if (&rhs != this)
        {
            riffChunk = rhs.riffChunk;
            fmtChunk = rhs.fmtChunk;
            dataChunk = rhs.dataChunk;
            acidChunk = rhs.acidChunk;
            bextChunk = rhs.bextChunk;

            m_FilePath = rhs.m_FilePath;

            for (auto &chunk : rhs.otherChunks)
                otherChunks.push_back(chunk);

            filledRiffChunk = rhs.filledRiffChunk;
            filledFmtChunk = rhs.filledFmtChunk;
            filledDataChunk = rhs.filledDataChunk;
            filledAcidChunk = rhs.filledAcidChunk;
            filledBextChunk = rhs.filledBextChunk;
        }
        return *this;
    }

    /// <summary>
    /// Calculates the buffer size.
    /// </summary>
    /// <returns>The buffer size.</returns>
    uint32_t WavFormat::GetBufferSize() const
    {
        ASSERT(filledFmtChunk, "Cannot retrieve buffer size: format chunk not set (\"%s\").", m_FilePath.c_str());
        return fmtChunk.bitsPerSample / 8;
    }

    /// <summary>
    /// Calculates the bit rate in bits per second.
    /// </summary>
    /// <returns>Bit rate in bits per second.</returns>
    uint32_t WavFormat::GetBitRate() const
    {
        return fmtChunk.numChannels * fmtChunk.bitsPerSample * fmtChunk.sampleRate;
    }
}