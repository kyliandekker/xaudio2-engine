#include <wave/WaveFormat.h>

#include "utils/Logger.h"

namespace uaudio
{
    Chunk::Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize)
    {
        memcpy(chunkId, a_ChunkID, sizeof(chunkId));
        chunkSize = a_ChunkSize;
    }

    Chunk::Chunk(const Chunk &rhs)
    {
        memcpy(chunkId, rhs.chunkId, sizeof(chunkId));
        chunkSize = rhs.chunkSize;
    }

    uint32_t Chunk::GetSize()
    {
        return sizeof(Chunk);
    }

    Chunk::~Chunk() = default;

    Chunk &Chunk::operator=(const Chunk &rhs)
    {
        if (&rhs != this)
        {
            memcpy(chunkId, rhs.chunkId, sizeof(chunkId));
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
            memcpy(format, rhs.format, sizeof(format));
        }
        return *this;
    }

    uint32_t RIFF_Chunk::GetSize()
    {
        return sizeof(RIFF_Chunk);
    }

    FMT_Chunk::FMT_Chunk() : Chunk() {}

    FMT_Chunk::FMT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample) : Chunk(a_ChunkID, a_ChunkSize), audioFormat(a_AudioFormat), numChannels(a_NumChannels), sampleRate(a_SampleRate), byteRate(a_ByteRate), blockAlign(a_BlockAlign), bitsPerSample(a_BitsPerSample)
    {
    }

    FMT_Chunk::FMT_Chunk(const FMT_Chunk &rhs) : Chunk(rhs), audioFormat(rhs.audioFormat), numChannels(rhs.numChannels), sampleRate(rhs.sampleRate), byteRate(rhs.byteRate), blockAlign(rhs.blockAlign), bitsPerSample(rhs.bitsPerSample)
    {
    }

    FMT_Chunk::~FMT_Chunk() = default;

    uint32_t FMT_Chunk::GetSize()
    {
        return sizeof(FMT_Chunk);
    }

    FMT_Chunk &FMT_Chunk::operator=(const FMT_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
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
        data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(a_ChunkSize)); // Set aside sound buffer space.

        if (data != nullptr)
            memcpy(data, a_Data, a_ChunkSize);
    }

    DATA_Chunk::DATA_Chunk(const DATA_Chunk &rhs) : Chunk(rhs)
    {
        data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(rhs.chunkSize)); // Set aside sound buffer space.

        if (data != nullptr)
            memcpy(data, rhs.data, rhs.chunkSize);
    }

    DATA_Chunk::~DATA_Chunk()
    {
        UAUDIO_DEFAULT_FREE(data);
    }

    uint32_t DATA_Chunk::GetSize()
    {
        return 0;
    }

    DATA_Chunk &DATA_Chunk::operator=(const DATA_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            if (rhs.data != nullptr)
            {
                data = static_cast<unsigned char *>(UAUDIO_DEFAULT_ALLOC(rhs.chunkSize)); // Set aside sound buffer space.

                if (data != nullptr)
                    memcpy(data, rhs.data, rhs.chunkSize);
            }
        }
        return *this;
    }

    ACID_Chunk::ACID_Chunk() : Chunk() {}

    ACID_Chunk::ACID_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint16_t a_Unknown1, float a_Unknown2, uint32_t a_NumBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo) : Chunk(a_ChunkID, a_ChunkSize),
                                                                                                                                                                                                                                                                          type_of_file(a_TypeOfFile),
                                                                                                                                                                                                                                                                          root_note(a_RootNote),
                                                                                                                                                                                                                                                                          unknown1(a_Unknown1),
                                                                                                                                                                                                                                                                          unknown2(a_Unknown2),
                                                                                                                                                                                                                                                                          num_of_beats(a_NumBeats),
                                                                                                                                                                                                                                                                          meter_denominator(a_MeterDenominator),
                                                                                                                                                                                                                                                                          meter_numerator(a_MeterNumerator),
                                                                                                                                                                                                                                                                          tempo(a_Tempo)
    {
    }

    ACID_Chunk::ACID_Chunk(const ACID_Chunk &rhs) : Chunk(rhs), type_of_file(rhs.type_of_file), root_note(rhs.root_note), unknown1(rhs.unknown1), unknown2(rhs.unknown2), num_of_beats(rhs.num_of_beats), meter_denominator(rhs.meter_denominator), meter_numerator(rhs.meter_numerator), tempo(rhs.tempo)
    {
    }

    ACID_Chunk::~ACID_Chunk() = default;

    uint32_t ACID_Chunk::GetSize()
    {
        return sizeof(ACID_Chunk);
    }

    ACID_Chunk &ACID_Chunk::operator=(const ACID_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            type_of_file = rhs.type_of_file;
            root_note = rhs.root_note;
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

    BEXT_Chunk::BEXT_Chunk(const BEXT_Chunk &rhs) : Chunk(rhs),
                                                    time_reference_low(rhs.time_reference_low),
                                                    time_reference_high(rhs.time_reference_high),
                                                    version(rhs.version),
                                                    loudness_value(rhs.loudness_value),
                                                    loudness_range(rhs.loudness_range),
                                                    max_true_peak_level(rhs.max_true_peak_level),
                                                    max_momentary_loudness(rhs.max_momentary_loudness),
                                                    max_short_term_loudness(rhs.max_short_term_loudness)
    {
        memcpy(description, rhs.description, sizeof(description));
        memcpy(originator, rhs.originator, sizeof(originator));
        memcpy(originator_reference, rhs.originator_reference, sizeof(originator_reference));
        memcpy(origination_date, rhs.origination_date, sizeof(origination_date));
        memcpy(origination_time, rhs.origination_time, sizeof(origination_time));

        memcpy(umid, rhs.umid, sizeof(origination_time));

        memcpy(reserved, rhs.reserved, sizeof(reserved));
    }

    BEXT_Chunk::~BEXT_Chunk() = default;

    uint32_t BEXT_Chunk::GetSize()
    {
        return sizeof(BEXT_Chunk);
    }

    BEXT_Chunk &BEXT_Chunk::operator=(const BEXT_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
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

    FACT_Chunk::FACT_Chunk() : Chunk()
    {
    }

    FACT_Chunk::FACT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_SampleLength) : Chunk(a_ChunkID, a_ChunkSize)
    {
        sample_length = a_SampleLength;
    }

    FACT_Chunk::FACT_Chunk(const FACT_Chunk &rhs) : Chunk(rhs)
    {
        sample_length = rhs.sample_length;
    }

    uint32_t FACT_Chunk::GetSize()
    {
        return sizeof(FACT_Chunk);
    }

    FACT_Chunk &FACT_Chunk::operator=(const FACT_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            sample_length = rhs.sample_length;
        }
        return *this;
    }

    SMPL_Sample_Loop::SMPL_Sample_Loop()
    {
    }

    SMPL_Sample_Loop::SMPL_Sample_Loop(uint32_t a_CuePointId, uint32_t a_Type, uint32_t a_Start, uint32_t a_End, uint32_t a_Fraction, uint32_t a_PlayCount) : cue_point_id(a_CuePointId),
                                                                                                                                                              type(a_Type),
                                                                                                                                                              start(a_Start),
                                                                                                                                                              end(a_End),
                                                                                                                                                              fraction(a_Fraction),
                                                                                                                                                              play_count(a_PlayCount)
    {
    }

    SMPL_Sample_Loop::SMPL_Sample_Loop(const SMPL_Sample_Loop &rhs) : cue_point_id(rhs.cue_point_id),
                                                                      type(rhs.type),
                                                                      start(rhs.start),
                                                                      end(rhs.end),
                                                                      fraction(rhs.fraction),
                                                                      play_count(rhs.play_count)
    {
    }

    SMPL_Sample_Loop &SMPL_Sample_Loop::operator=(const SMPL_Sample_Loop &rhs)
    {
        if (&rhs != this)
        {
            cue_point_id = rhs.cue_point_id;
            type = rhs.type;
            start = rhs.start;
            end = rhs.end;
            fraction = rhs.fraction;
            play_count = rhs.play_count;
        }
        return *this;
    }

    SMPL_Chunk::SMPL_Chunk() : Chunk()
    {
    }

    SMPL_Chunk::SMPL_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_Manufacturer, uint32_t a_Product, uint32_t a_SamplePeriod, uint32_t a_MidiUnityNode, uint32_t a_MidiPitchFraction, uint32_t a_Smpte_Format, uint32_t a_Smpte_Offset, uint32_t a_NumSampleLoops, uint32_t a_SamplerData, SMPL_Sample_Loop *a_Samples) : Chunk(a_ChunkID, a_ChunkSize),
                                                                                                                                                                                                                                                                                                                                                           manufacturer(a_Manufacturer),
                                                                                                                                                                                                                                                                                                                                                           product(a_Product),
                                                                                                                                                                                                                                                                                                                                                           sample_period(a_SamplePeriod),
                                                                                                                                                                                                                                                                                                                                                           midi_unity_node(a_MidiUnityNode),
                                                                                                                                                                                                                                                                                                                                                           midi_pitch_fraction(a_MidiPitchFraction),
                                                                                                                                                                                                                                                                                                                                                           smpte_format(a_Smpte_Format),
                                                                                                                                                                                                                                                                                                                                                           smpte_offset(a_Smpte_Offset),
                                                                                                                                                                                                                                                                                                                                                           num_sample_loops(a_NumSampleLoops),
                                                                                                                                                                                                                                                                                                                                                           sampler_data(a_SamplerData)
    {
        if (a_Samples != nullptr)
        {
            samples = static_cast<SMPL_Sample_Loop *>(UAUDIO_DEFAULT_ALLOC(sizeof(SMPL_Sample_Loop) * num_sample_loops));

            // TODO: FIX THESE.
            for (uint32_t i = 0; i < num_sample_loops; i++)
                if (samples != nullptr)
                    samples[i] = a_Samples[i];
        }
    }

    SMPL_Chunk::SMPL_Chunk(const SMPL_Chunk &rhs) : Chunk(rhs),
                                                    manufacturer(rhs.manufacturer),
                                                    product(rhs.product),
                                                    sample_period(rhs.sample_period),
                                                    midi_unity_node(rhs.midi_unity_node),
                                                    midi_pitch_fraction(rhs.midi_pitch_fraction),
                                                    smpte_format(rhs.smpte_format),
                                                    smpte_offset(rhs.smpte_offset),
                                                    num_sample_loops(rhs.num_sample_loops),
                                                    sampler_data(rhs.sampler_data)
    {
        if (rhs.samples != nullptr)
        {
            samples = static_cast<SMPL_Sample_Loop *>(UAUDIO_DEFAULT_ALLOC(sizeof(SMPL_Sample_Loop) * num_sample_loops));

            for (uint32_t i = 0; i < num_sample_loops; i++)
                if (samples != nullptr)
                    samples[i] = rhs.samples[i];
        }
    }

    SMPL_Chunk::~SMPL_Chunk()
    {
        UAUDIO_DEFAULT_FREE(samples);
    }

    uint32_t SMPL_Chunk::GetSize()
    {
        return sizeof(SMPL_Chunk) - sizeof(SMPL_Sample_Loop*);
    }

    SMPL_Chunk &SMPL_Chunk::operator=(const SMPL_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            manufacturer = rhs.manufacturer;
            product = rhs.product;
            sample_period = rhs.sample_period;
            midi_unity_node = rhs.midi_unity_node;
            midi_pitch_fraction = rhs.midi_pitch_fraction;
            smpte_format = rhs.smpte_format;
            smpte_offset = rhs.smpte_offset;
            num_sample_loops = rhs.num_sample_loops;
            sampler_data = rhs.sampler_data;

            if (rhs.samples != nullptr)
            {
                samples = static_cast<SMPL_Sample_Loop *>(UAUDIO_DEFAULT_ALLOC(sizeof(SMPL_Sample_Loop) * num_sample_loops));

                for (uint32_t i = 0; i < num_sample_loops; i++)
                    if (samples != nullptr)
                        samples[i] = rhs.samples[i];
            }
        }
        return *this;
    }

    CUE_Point::CUE_Point()
    {
    }

    CUE_Point::CUE_Point(uint32_t a_Id, uint32_t a_Position, uint32_t a_DataChunkId, uint32_t a_ChunkStart, uint32_t a_BlockStart, uint32_t a_SampleOffset) : id(a_Id), position(a_Position), data_chunk_id(a_DataChunkId), chunk_start(a_ChunkStart), block_start(a_BlockStart), sample_offset(a_SampleOffset)
    {
    }

    CUE_Point::CUE_Point(const CUE_Point &rhs) : id(rhs.id), position(rhs.position), data_chunk_id(rhs.data_chunk_id), chunk_start(rhs.chunk_start), block_start(rhs.block_start), sample_offset(rhs.sample_offset)
    {
    }

    CUE_Point &CUE_Point::operator=(const CUE_Point &rhs)
    {
        if (&rhs != this)
        {
            id = rhs.id;
            position = rhs.position;
            data_chunk_id = rhs.data_chunk_id;
            chunk_start = rhs.chunk_start;
            block_start = rhs.block_start;
            sample_offset = rhs.sample_offset;
        }
        return *this;
    }

    CUE_Chunk::CUE_Chunk() : Chunk()
    {
    }

    CUE_Chunk::CUE_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint32_t a_NumCuePoints, CUE_Point *a_CuePoints) : Chunk(a_ChunkID, a_ChunkSize), num_cue_points(a_NumCuePoints)
    {
        cue_points = static_cast<CUE_Point *>(UAUDIO_DEFAULT_ALLOC(sizeof(CUE_Point) * num_cue_points));

        for (uint32_t i = 0; i < num_cue_points; i++)
            if (cue_points != nullptr)
                cue_points[i] = a_CuePoints[i];
    }

    CUE_Chunk::CUE_Chunk(const CUE_Chunk &rhs) : Chunk(rhs), num_cue_points(rhs.num_cue_points)
    {
        cue_points = static_cast<CUE_Point *>(UAUDIO_DEFAULT_ALLOC(sizeof(CUE_Point) * num_cue_points));

        for (uint32_t i = 0; i < num_cue_points; i++)
            if (cue_points != nullptr)
                cue_points[i] = rhs.cue_points[i];
    }

    uint32_t CUE_Chunk::GetSize()
    {
        return sizeof(CUE_Chunk) - sizeof(CUE_Point*);
    }

    CUE_Chunk::~CUE_Chunk()
    {
        UAUDIO_DEFAULT_FREE(cue_points);
    }

    CUE_Chunk &CUE_Chunk::operator=(const CUE_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            num_cue_points = rhs.num_cue_points;

            cue_points = static_cast<CUE_Point *>(UAUDIO_DEFAULT_ALLOC(sizeof(CUE_Point) * num_cue_points));

            for (uint32_t i = 0; i < num_cue_points; i++)
                if (cue_points != nullptr)
                    cue_points[i] = rhs.cue_points[i];
        }
        return *this;
    }

    WaveFormat::WaveFormat(const WaveFormat &rhs)
    {
        riffChunk = rhs.riffChunk;
        fmtChunk = rhs.fmtChunk;
        dataChunk = rhs.dataChunk;
        acidChunk = rhs.acidChunk;
        bextChunk = rhs.bextChunk;
        factChunk = rhs.factChunk;
        smplChunk = rhs.smplChunk;
        cueChunk = rhs.cueChunk;

        m_FilePath = rhs.m_FilePath;

        for (auto &chunk : rhs.otherChunks)
            otherChunks.push_back(chunk);

        filledRiffChunk = rhs.filledRiffChunk;
        filledFmtChunk = rhs.filledFmtChunk;
        filledDataChunk = rhs.filledDataChunk;
        filledAcidChunk = rhs.filledAcidChunk;
        filledBextChunk = rhs.filledBextChunk;
        filledFactChunk = rhs.filledFactChunk;
        filledSmplChunk = rhs.filledSmplChunk;
        filledCueChunk = rhs.filledCueChunk;
    }

    WaveFormat::~WaveFormat() = default;

    WaveFormat &WaveFormat::operator=(const WaveFormat &rhs)
    {
        if (&rhs != this)
        {
            riffChunk = rhs.riffChunk;
            fmtChunk = rhs.fmtChunk;
            dataChunk = rhs.dataChunk;
            acidChunk = rhs.acidChunk;
            bextChunk = rhs.bextChunk;
            factChunk = rhs.factChunk;
            smplChunk = rhs.smplChunk;
            cueChunk = rhs.cueChunk;

            m_FilePath = rhs.m_FilePath;

            for (auto &chunk : rhs.otherChunks)
                otherChunks.push_back(chunk);

            filledRiffChunk = rhs.filledRiffChunk;
            filledFmtChunk = rhs.filledFmtChunk;
            filledDataChunk = rhs.filledDataChunk;
            filledAcidChunk = rhs.filledAcidChunk;
            filledBextChunk = rhs.filledBextChunk;
            filledFactChunk = rhs.filledFactChunk;
            filledSmplChunk = rhs.filledSmplChunk;
            filledCueChunk = rhs.filledCueChunk;
        }
        return *this;
    }

    /// <summary>
    /// Calculates the buffer size.
    /// </summary>
    /// <returns>The buffer size.</returns>
    uint32_t WaveFormat::GetBufferSize() const
    {
        logger::ASSERT(filledFmtChunk, "Cannot retrieve buffer size: format chunk not set (\"%s\").", m_FilePath.c_str());
        return fmtChunk.bitsPerSample / 8;
    }

    /// <summary>
    /// Calculates the bit rate in bits per second.
    /// </summary>
    /// <returns>Bit rate in bits per second.</returns>
    uint32_t WaveFormat::GetBitRate() const
    {
        return fmtChunk.numChannels * fmtChunk.bitsPerSample * fmtChunk.sampleRate;
    }
}
