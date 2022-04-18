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

    Chunk& Chunk::operator=(const Chunk & rhs)
    {
        if (&rhs != this)
        {
            memcpy(chunkId, rhs.chunkId, sizeof(chunkId));
            chunkSize = rhs.chunkSize;
        }
        return *this;
    }

    uint32_t Chunk::GetSize()
    {
        return sizeof(Chunk);
    }

    uint32_t Chunk::GetRuntimeSize() const
    {
        return sizeof(Chunk);
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

    RIFF_Chunk &RIFF_Chunk::operator=(const RIFF_Chunk &rhs)
    {
        if (&rhs != this)
        {
            Chunk::operator=(rhs);
            memcpy(format, rhs.format, sizeof(format));
        }
        return *this;
    }

    void RIFF_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&format), sizeof(format), 1, a_File);
    }

    uint32_t RIFF_Chunk::GetSize()
    {
        return sizeof(RIFF_Chunk);
    }

    uint32_t RIFF_Chunk::GetRuntimeSize() const
    {
        return RIFF_Chunk::GetSize();
    }

    FMT_Chunk::FMT_Chunk() : Chunk() {}

    FMT_Chunk::FMT_Chunk(unsigned char a_ChunkID[CHUNK_ID_SIZE], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample) : Chunk(a_ChunkID, a_ChunkSize), audioFormat(a_AudioFormat), numChannels(a_NumChannels), sampleRate(a_SampleRate), byteRate(a_ByteRate), blockAlign(a_BlockAlign), bitsPerSample(a_BitsPerSample)
    {
    }

    FMT_Chunk::FMT_Chunk(const FMT_Chunk &rhs) : Chunk(rhs), audioFormat(rhs.audioFormat), numChannels(rhs.numChannels), sampleRate(rhs.sampleRate), byteRate(rhs.byteRate), blockAlign(rhs.blockAlign), bitsPerSample(rhs.bitsPerSample)
    {
    }

    void FMT_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&audioFormat), sizeof(audioFormat), 1, a_File);
        fwrite(reinterpret_cast<char*>(&numChannels), sizeof(numChannels), 1, a_File);
        fwrite(reinterpret_cast<char*>(&sampleRate), sizeof(sampleRate), 1, a_File);
        fwrite(reinterpret_cast<char*>(&byteRate), sizeof(byteRate), 1, a_File);
        fwrite(reinterpret_cast<char*>(&blockAlign), sizeof(blockAlign), 1, a_File);
        fwrite(reinterpret_cast<char*>(&bitsPerSample), sizeof(bitsPerSample), 1, a_File);
    }

    uint32_t FMT_Chunk::GetSize()
    {
        return sizeof(FMT_Chunk);
    }

    uint32_t FMT_Chunk::GetRuntimeSize() const
    {
        return FMT_Chunk::GetSize();
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

    void DATA_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        for (uint32_t i = 0; i < chunkSize; i++)
            fwrite(reinterpret_cast<char*>(&data[i]), 1, 1, a_File);
    }

    uint32_t DATA_Chunk::GetSize()
    {
        return Chunk::GetSize();
    }

    uint32_t DATA_Chunk::GetRuntimeSize() const
    {
        return DATA_Chunk::GetSize() + chunkSize;
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

    void ACID_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&type_of_file), sizeof(type_of_file), 1, a_File);
        fwrite(reinterpret_cast<char*>(&root_note), sizeof(root_note), 1, a_File);
        fwrite(reinterpret_cast<char*>(&unknown1), sizeof(unknown1), 1, a_File);
        fwrite(reinterpret_cast<char*>(&unknown2), sizeof(unknown2), 1, a_File);
        fwrite(reinterpret_cast<char*>(&num_of_beats), sizeof(num_of_beats), 1, a_File);
        fwrite(reinterpret_cast<char*>(&meter_denominator), sizeof(meter_denominator), 1, a_File);
        fwrite(reinterpret_cast<char*>(&meter_numerator), sizeof(meter_numerator), 1, a_File);
        fwrite(reinterpret_cast<char*>(&tempo), sizeof(tempo), 1, a_File);
    }

    uint32_t ACID_Chunk::GetSize()
    {
        return sizeof(ACID_Chunk);
    }

    uint32_t ACID_Chunk::GetRuntimeSize() const
    {
        return ACID_Chunk::GetSize();
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

    void BEXT_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&description), sizeof(description), 1, a_File);
        fwrite(reinterpret_cast<char*>(&originator), sizeof(originator), 1, a_File);
        fwrite(reinterpret_cast<char*>(&originator_reference), sizeof(originator_reference), 1, a_File);
        fwrite(reinterpret_cast<char*>(&origination_date), sizeof(origination_date), 1, a_File);
        fwrite(reinterpret_cast<char*>(&origination_time), sizeof(origination_time), 1, a_File);
        fwrite(reinterpret_cast<char*>(&time_reference_low), sizeof(time_reference_low), 1, a_File);
        fwrite(reinterpret_cast<char*>(&time_reference_high), sizeof(time_reference_high), 1, a_File);
        fwrite(reinterpret_cast<char*>(&version), sizeof(version), 1, a_File);
        fwrite(reinterpret_cast<char*>(&umid), sizeof(umid), 1, a_File);
        fwrite(reinterpret_cast<char*>(&loudness_value), sizeof(loudness_value), 1, a_File);
        fwrite(reinterpret_cast<char*>(&loudness_range), sizeof(loudness_range), 1, a_File);
        fwrite(reinterpret_cast<char*>(&max_true_peak_level), sizeof(max_true_peak_level), 1, a_File);
        fwrite(reinterpret_cast<char*>(&max_momentary_loudness), sizeof(max_momentary_loudness), 1, a_File);
        fwrite(reinterpret_cast<char*>(&max_short_term_loudness), sizeof(max_short_term_loudness), 1, a_File);
        fwrite(reinterpret_cast<char*>(&reserved), sizeof(reserved), 1, a_File);
    }

    uint32_t BEXT_Chunk::GetSize()
    {
        return sizeof(BEXT_Chunk);
    }

    uint32_t BEXT_Chunk::GetRuntimeSize() const
    {
        return BEXT_Chunk::GetSize();
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

    void FACT_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&sample_length), sizeof(sample_length), 1, a_File);
    }

    uint32_t FACT_Chunk::GetSize()
    {
        return sizeof(FACT_Chunk);
    }

    uint32_t FACT_Chunk::GetRuntimeSize() const
    {
        return FACT_Chunk::GetSize();
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

    void SMPL_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&manufacturer), sizeof(manufacturer), 1, a_File);
        fwrite(reinterpret_cast<char*>(&product), sizeof(product), 1, a_File);
        fwrite(reinterpret_cast<char*>(&sample_period), sizeof(sample_period), 1, a_File);
        fwrite(reinterpret_cast<char*>(&midi_unity_node), sizeof(midi_unity_node), 1, a_File);
        fwrite(reinterpret_cast<char*>(&midi_pitch_fraction), sizeof(midi_pitch_fraction), 1, a_File);
        fwrite(reinterpret_cast<char*>(&smpte_format), sizeof(smpte_format), 1, a_File);
        fwrite(reinterpret_cast<char*>(&smpte_offset), sizeof(smpte_offset), 1, a_File);
        fwrite(reinterpret_cast<char*>(&num_sample_loops), sizeof(num_sample_loops), 1, a_File);
        fwrite(reinterpret_cast<char*>(&sampler_data), sizeof(sampler_data), 1, a_File);
        for (uint32_t i = 0; i < num_sample_loops; i++)
        {
            fwrite(reinterpret_cast<char*>(&samples[i].cue_point_id), sizeof(samples[i].cue_point_id), 1, a_File);
            fwrite(reinterpret_cast<char*>(&samples[i].type), sizeof(samples[i].type), 1, a_File);
            fwrite(reinterpret_cast<char*>(&samples[i].start), sizeof(samples[i].start), 1, a_File);
            fwrite(reinterpret_cast<char*>(&samples[i].end), sizeof(samples[i].end), 1, a_File);
            fwrite(reinterpret_cast<char*>(&samples[i].fraction), sizeof(samples[i].fraction), 1, a_File);
            fwrite(reinterpret_cast<char*>(&samples[i].play_count), sizeof(samples[i].play_count), 1, a_File);
        }
    }

    uint32_t SMPL_Chunk::GetSize()
    {
        return sizeof(SMPL_Chunk) - sizeof(samples);
    }

    uint32_t SMPL_Chunk::GetRuntimeSize() const
    {
        return SMPL_Chunk::GetSize() + (num_sample_loops * sizeof(SMPL_Sample_Loop));
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

    uint32_t CUE_Chunk::GetRuntimeSize() const
    {
        return CUE_Chunk::GetSize() + (num_cue_points * sizeof(CUE_Point));
    }

    CUE_Chunk::~CUE_Chunk()
    {
        UAUDIO_DEFAULT_FREE(cue_points);
    }

    void CUE_Chunk::Write(FILE*& a_File)
    {
        fwrite(reinterpret_cast<char*>(&chunkId), sizeof(chunkId), 1, a_File);
        fwrite(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize), 1, a_File);
        fwrite(reinterpret_cast<char*>(&num_cue_points), sizeof(num_cue_points), 1, a_File);
        for (uint32_t i = 0; i < num_cue_points; i++)
        {
            fwrite(reinterpret_cast<char*>(&cue_points[i].id), sizeof(cue_points[i].id), 1, a_File);
            fwrite(reinterpret_cast<char*>(&cue_points[i].position), sizeof(cue_points[i].position), 1, a_File);
            fwrite(reinterpret_cast<char*>(&cue_points[i].data_chunk_id), sizeof(cue_points[i].chunk_start), 1, a_File);
            fwrite(reinterpret_cast<char*>(&cue_points[i].chunk_start), sizeof(cue_points[i].data_chunk_id), 1, a_File);
            fwrite(reinterpret_cast<char*>(&cue_points[i].block_start), sizeof(cue_points[i].block_start), 1, a_File);
            fwrite(reinterpret_cast<char*>(&cue_points[i].sample_offset), sizeof(cue_points[i].sample_offset), 1, a_File);
        }
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

    /// <summary>
    /// Recalculates the riff chunk size.
    /// </summary>
    void WaveFormat::CalculateRiffChunkSize(WAVE_CONFIG a_Config)
    {
        uint32_t size = 0;

        if (a_Config.HasChunk(CHUNK_FLAG_RIFF))
        {
            uint32_t riffSize = riffChunk.GetRuntimeSize() - sizeof(Chunk);
            size += riffSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_FMT))
        {
            uint32_t fmtSize = fmtChunk.GetRuntimeSize();
            size += fmtSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_DATA))
        {
            uint32_t dataSize = dataChunk.GetRuntimeSize();
            size += dataSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_ACID))
        {
            uint32_t acidSize = acidChunk.GetRuntimeSize();
            size += acidSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_BEXT))
        {
            uint32_t bextSize = bextChunk.GetRuntimeSize();
            size += bextSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_FACT))
        {
            uint32_t factSize = factChunk.GetRuntimeSize();
            size += factSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_SMPL))
        {
            uint32_t smplSize = smplChunk.GetRuntimeSize();
            size += smplSize;
        }
        if (a_Config.HasChunk(CHUNK_FLAG_CUE))
        {
            uint32_t cueSize = cueChunk.GetRuntimeSize();
            size += cueSize;
        }

        riffChunk.chunkSize = size;
    }
}
