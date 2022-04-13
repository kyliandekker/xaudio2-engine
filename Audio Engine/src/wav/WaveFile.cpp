#include <algorithm>

#include <xaudio2_engine/wav/WaveFile.h>

#include <xaudio2_engine/wav/WavConverter.h>
#include <xaudio2_engine/utils/Logger.h>

constexpr uint32_t WAVE_FORMAT_PCM = 1;
constexpr uint32_t WAVE_CHANNELS_MONO = 1;
constexpr uint32_t WAVE_CHANNELS_STEREO = 2;

WaveFile::WaveFile() = default;

WaveFile::WaveFile(const WaveFile &rhs)
{
    m_Looping = rhs.m_Looping;
    m_Volume = rhs.m_Volume;
    m_File = rhs.m_File;
    m_WavFile = rhs.m_WavFile;
    m_SoundTitle = rhs.m_SoundTitle;
}

WaveFile::WaveFile(const char* a_FilePath)
{
    m_WavFile = {};

    m_SoundTitle = std::string(a_FilePath);

    // Open the file.
    fopen_s(&m_File, a_FilePath, "rb");
    if (!m_File)
    {
        logger::log_error("<Wav> Failed opening file: \"%s\".", a_FilePath);
        return;
    }

    uint32_t total_chunksize = 0;
    while (!feof(m_File))
    {
        unsigned char chunkid[4];

        uint32_t chunksize;

        // Read what chunk id the file has.
        fread(&chunkid, sizeof(chunkid), 1, m_File);

        // Check chunk size (file size).
        fread(&chunksize, sizeof(chunksize), 1, m_File);

        /*
         * RIFF CHUNK
         */
        if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "RIFF") == 0)
        {
            total_chunksize += 4;
            unsigned char format[4];

            // Format (should be WAVE).
            fread(&format, sizeof(format), 1, m_File);

            if (strcmp(std::string(&format[0], &format[0] + std::size(format)).c_str(), "WAVE") != 0)
            {
                logger::log_error("<Wav> (\"%s\") is not a WAV file. (%s).", m_SoundTitle.c_str(), format);
                return;
            }

            SetRIFFChunk(chunkid, chunksize, format);
        }
        else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "fmt ") == 0)
        {
            total_chunksize += sizeof(Chunk);
            total_chunksize += chunksize;

            uint16_t audioFormat;
            uint16_t numChannels;
            uint32_t sampleRate;
            uint32_t byteRate;
            uint16_t blockAlign;
            uint16_t bitsPerSample;

            // audio format (should be 1, other values indicate compression).
            fread(&audioFormat, sizeof(audioFormat), 1, m_File);

            // numChannels (mono is 1, stereo is 2).
            fread(&numChannels, sizeof(numChannels), 1, m_File);

            // sampleRate
            fread(&sampleRate, sizeof(sampleRate), 1, m_File);

            // byteRate
            fread(&byteRate, sizeof(byteRate), 1, m_File);

            // blockAlign
            fread(&blockAlign, sizeof(blockAlign), 1, m_File);

            // bitsPerSample
            fread(&bitsPerSample, sizeof(bitsPerSample), 1, m_File);

            SetFMTChunk(chunkid, chunksize, audioFormat, numChannels, sampleRate, byteRate, blockAlign, bitsPerSample);

            // NOTE: Sometimes the fmt chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
            uint32_t expected_chunksize = sizeof(FMT_Chunk) - sizeof(Chunk);
        	if (expected_chunksize < chunksize)
            {
                logger::log_info("<Wav> (\"%s\") Bigger fmt chunk than usual, skipping rest of data.", m_SoundTitle.c_str());
                fseek(m_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
            }
        }
        else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "data") == 0)
        {
            total_chunksize += sizeof(Chunk);
            total_chunksize += chunksize;

            // Actual data.
            unsigned char* data = static_cast<unsigned char*>(malloc(sizeof(data) * chunksize)); // set aside sound buffer space.
            fread(data, sizeof(data), chunksize, m_File); // read in our whole sound data chunk.

            SetDataChunk(chunkid, chunksize, data);

            free(data);
        }
        else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "acid") == 0)
        {
            total_chunksize += sizeof(Chunk);
            total_chunksize += chunksize;

            uint32_t type_of_file;
            uint16_t root_note;
            uint32_t num_of_beats;
            uint16_t meter_denominator;
            uint16_t meter_numerator;
            float tempo;

            fread(&type_of_file, sizeof(type_of_file), 1, m_File);

            fread(&root_note, sizeof(root_note), 1, m_File);

            fseek(m_File, 6, SEEK_CUR);

            fread(&num_of_beats, sizeof(num_of_beats), 1, m_File);

            fread(&meter_denominator, sizeof(meter_denominator), 1, m_File);

            fread(&meter_numerator, sizeof(meter_numerator), 1, m_File);

            fread(&tempo, sizeof(tempo), 1, m_File);

            SetAcidChunk(chunkid, chunksize, type_of_file, root_note, num_of_beats, meter_denominator, meter_numerator, tempo);

            // NOTE: Sometimes the acid chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
            uint32_t expected_chunksize = sizeof(ACID_Chunk) - sizeof(Chunk);
            if (expected_chunksize < chunksize)
            {
                logger::log_info("<Wav> (\"%s\") Bigger acid chunk than usual, skipping rest of data.", m_SoundTitle.c_str());
                fseek(m_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
            }
        }
        else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "gudf") == 0)
        {
			printf("%s\n", std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str());
            printf("%i\n", chunksize);
            fseek(m_File, chunksize, SEEK_CUR);
        }
        else if (strcmp(std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), "bext") == 0)
        {
			total_chunksize += sizeof(Chunk);
            total_chunksize += chunksize;

            char description[256];
            char originator[32];
            char originator_reference[32];
            char origination_date[10];
            char origination_time[8];
            uint32_t time_reference_low;
            uint32_t time_reference_high;
            uint16_t version;
            unsigned char umid[64];
            uint16_t loudness_value;
            uint16_t loudness_range;
            uint16_t max_true_peak_level;
            uint16_t max_momentary_loudness;
            uint16_t max_short_term_loudness;
            unsigned char reserved[180];

            fread(&description, sizeof(description), 1, m_File);

            fread(&originator, sizeof(originator), 1, m_File);

            fread(&originator_reference, sizeof(originator_reference), 1, m_File);

            fread(&origination_date, sizeof(origination_date), 1, m_File);

            fread(&origination_time, sizeof(origination_time), 1, m_File);

            fread(&time_reference_low, sizeof(time_reference_low), 1, m_File);

            fread(&time_reference_high, sizeof(time_reference_high), 1, m_File);

            fread(&version, sizeof(version), 1, m_File);

            fread(&umid, sizeof(umid), 1, m_File);

            fread(&loudness_value, sizeof(loudness_value), 1, m_File);

            fread(&loudness_range, sizeof(loudness_range), 1, m_File);

            fread(&max_true_peak_level, sizeof(max_true_peak_level), 1, m_File);

            fread(&max_momentary_loudness, sizeof(max_momentary_loudness), 1, m_File);

            fread(&max_short_term_loudness, sizeof(max_short_term_loudness), 1, m_File);

            fread(&reserved, sizeof(reserved), 1, m_File);

            SetBextChunk(chunkid, chunksize, description, originator, originator_reference, origination_date, origination_time, time_reference_low, time_reference_high, version, umid, loudness_value, loudness_range, max_true_peak_level, max_momentary_loudness, max_short_term_loudness, reserved);

            // NOTE: Sometimes the bext chunk is bigger than defined on my sources. Here is a fix so that the rest of the wav file can be read as normal.
            uint32_t expected_chunksize = sizeof(BEXT_Chunk) - sizeof(Chunk);
            if (expected_chunksize < chunksize)
            {
                logger::log_info("<Wav> (\"%s\") Bigger bext chunk than usual, skipping rest of data.", m_SoundTitle.c_str());
                fseek(m_File, static_cast<int32_t>(chunksize - expected_chunksize), SEEK_CUR);
            }
        }
        else
        {
			total_chunksize += sizeof(Chunk);
	        total_chunksize += chunksize;

			Chunk otherChunk = {};
            memcpy(m_WavFile.dataChunk.chunkId, chunkid, 4 * sizeof(unsigned char));
            m_WavFile.dataChunk.chunkSize = chunksize;
			m_WavFile.otherChunks.push_back(otherChunk);

            logger::log_info("<Wav> (\"%s\") Found subchunk %s with size %i. Skipping.", m_SoundTitle.c_str(), std::string(&chunkid[0], &chunkid[0] + std::size(chunkid)).c_str(), chunksize);

            fseek(m_File, static_cast<int32_t>(chunksize), SEEK_CUR);
        }
    }

    if (m_WavFile.fmtChunk.bitsPerSample == 32)
    {
        logger::log_info("<Wav> (\"%s\") Wav file is 32bit. Converting now.", m_SoundTitle.c_str());

        Convert32To16();
    }
    else if (m_WavFile.fmtChunk.bitsPerSample == 24)
    {
        logger::log_info("<Wav> (\"%s\") Wav file is 24bit. Converting now.", m_SoundTitle.c_str());

        Convert24To16();
    }

    if (m_WavFile.fmtChunk.numChannels == WAVE_CHANNELS_MONO)
    {
        m_WavFile.fmtChunk.numChannels = WAVE_CHANNELS_STEREO;

        unsigned char* array_16 = wav::wav_converter::ConvertMonoToStereo(m_WavFile.dataChunk.data, m_WavFile.dataChunk.chunkSize, m_WavFile.fmtChunk.blockAlign);
        free(m_WavFile.dataChunk.data);
        m_WavFile.dataChunk.data = array_16;
    }

    m_WavFile.fmtChunk.audioFormat = WAVE_FORMAT_PCM;
    m_WavFile.fmtChunk.blockAlign = m_WavFile.fmtChunk.numChannels * m_WavFile.fmtChunk.bitsPerSample / 8;
    m_WavFile.fmtChunk.byteRate = m_WavFile.fmtChunk.sampleRate * m_WavFile.fmtChunk.numChannels * m_WavFile.fmtChunk.bitsPerSample / 8;
    m_WavFile.riffChunk.chunkSize = CalculateRiffChunkSize();

    m_WavFile.bufferSize = m_WavFile.fmtChunk.bitsPerSample / 8;

    fclose(m_File);
}

void WaveFile::AddAcidChunk(float a_Tempo)
{
    if (!m_WavFile.filledAcidChunk)
    {
        std::ifstream infile(m_SoundTitle.c_str(), std::ios_base::binary);
        std::ofstream outfile;
        std::string str;

        auto a_NewFilePath = std::string(std::string(m_SoundTitle.c_str()) + std::string("new.wav"));
        outfile.open(a_NewFilePath, std::ios_base::binary); // append instead of overwrite

        RIFF_Chunk riffchunk = {};
        char riffchunkId[4] = {
            'R',
            'I',
            'F',
            'F',
        };
        memcpy(riffchunk.chunkId, riffchunkId, sizeof(m_WavFile.riffChunk.chunkId));
        riffchunk.chunkSize = m_WavFile.riffChunk.chunkSize + sizeof(ACID_Chunk);
        char wavefmt[4] = {
            'W',
            'A',
            'V',
            'E',
        };
        memcpy(riffchunk.format, wavefmt, sizeof(m_WavFile.riffChunk.format));
        outfile.write((char*)&riffchunk.chunkId, sizeof(riffchunk.chunkId));
        outfile.write((char*)&riffchunk.chunkSize, sizeof(riffchunk.chunkSize));
        outfile.write((char*)&riffchunk.format, sizeof(riffchunk.format));

        ACID_Chunk acidchunk = {};
        char acidchunkId[4] = {
            'a',
            'c',
            'i',
            'd',
        };
        memcpy(acidchunk.chunkId, acidchunkId, sizeof(m_WavFile.fmtChunk.chunkId));
        acidchunk.chunkSize = sizeof(ACID_Chunk) - sizeof(Chunk);
        acidchunk.tempo = a_Tempo;
        outfile.write((char*)&acidchunk.chunkId, sizeof(acidchunk.chunkId));
        outfile.write((char*)&acidchunk.chunkSize, sizeof(acidchunk.chunkSize));
        outfile.write((char*)&acidchunk.type_of_file, sizeof(acidchunk.type_of_file));
        outfile.write((char*)&acidchunk.root_note, sizeof(acidchunk.root_note));
        outfile.write((char*)&acidchunk.unknown1, sizeof(acidchunk.unknown1));
        outfile.write((char*)&acidchunk.unknown2, sizeof(acidchunk.unknown2));
        outfile.write((char*)&acidchunk.num_of_beats, sizeof(acidchunk.num_of_beats));
        outfile.write((char*)&acidchunk.meter_denominator, sizeof(acidchunk.meter_denominator));
        outfile.write((char*)&acidchunk.meter_numerator, sizeof(acidchunk.meter_numerator));
        outfile.write((char*)&acidchunk.tempo, sizeof(acidchunk.tempo));

        infile.seekg(sizeof(RIFF_Chunk));
        while (std::getline(infile, str))
        {
            outfile.write(str.c_str(), str.size());
            outfile << std::endl;
        }

        logger::log_info("<Wav> (\"%s\") Successfully added tempo info to file. New file created: %s.", m_SoundTitle.c_str(), a_NewFilePath.c_str());
        outfile.close();
    }
}

WaveFile::~WaveFile()
{
    free(m_WavFile.dataChunk.data);
    if (m_File)
        fclose(m_File);
}

uint32_t WaveFile::CalculateRiffChunkSize()
{
    uint32_t size = 0;

    // Riff
    if (m_WavFile.filledRiffChunk)
    {
        size += sizeof(m_WavFile.riffChunk.format);
    }

    // Fmt
    if (m_WavFile.filledFmtChunk)
    {
        size += sizeof(Chunk);
        size += m_WavFile.fmtChunk.chunkSize;
    }

    // Data
    if (m_WavFile.filledDataChunk)
    {
        size += sizeof(Chunk);
        size += m_WavFile.dataChunk.chunkSize;
    }

    // Acid
    if (m_WavFile.filledAcidChunk)
    {
        size += sizeof(Chunk);
        size += m_WavFile.acidChunk.chunkSize;
    }

    // Bext
    if (m_WavFile.filledBextChunk)
    {
        size += sizeof(Chunk);
        size += m_WavFile.bextChunk.chunkSize;
    }

    for (size_t i = 0; i < m_WavFile.otherChunks.size(); i++)
    {
        size += sizeof(Chunk);
        size += m_WavFile.otherChunks[i].chunkSize;
    }
    return size + 102;
}

/// <summary>
/// Sets the riff chunk.
/// </summary>
/// <param name="a_ChunkId">The chunk id.</param>
/// <param name="a_ChunkSize">The chunk size.</param>
/// <param name="a_Format">The format.</param>
void WaveFile::SetRIFFChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, unsigned char a_Format[4])
{
    memcpy(m_WavFile.riffChunk.chunkId, a_ChunkId, 4 * sizeof(unsigned char));
    m_WavFile.riffChunk.chunkSize = a_ChunkSize;
    memcpy(m_WavFile.riffChunk.format, a_Format, 4 * sizeof(unsigned char));

    m_WavFile.filledRiffChunk = true;

    logger::log_info("<Wav> (\"%s\") chunkId: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.riffChunk.chunkId[0], &m_WavFile.riffChunk.chunkId[0] + std::size(m_WavFile.riffChunk.chunkId)).c_str());
    logger::log_info("<Wav> (\"%s\") chunkSize: %i.", m_SoundTitle.c_str(), m_WavFile.riffChunk.chunkSize);
    logger::log_info("<Wav> (\"%s\") format: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.riffChunk.format[0], &m_WavFile.riffChunk.format[0] + std::size(m_WavFile.riffChunk.format)).c_str());
}

/// <summary>
/// Sets the format chunk.
/// </summary>
/// <param name="a_ChunkId">The chunk id.</param>
/// <param name="a_ChunkSize">The chunk size.</param>
/// <param name="a_AudioFormat">The audio format.</param>
/// <param name="a_NumChannels">The number of channels.</param>
/// <param name="a_SampleRate">The sample rate.</param>
/// <param name="a_ByteRate">The byte rate.</param>
/// <param name="a_BlockAlign">The block align.</param>
/// <param name="a_BitsPerSample">The bits per sample.</param>
void WaveFile::SetFMTChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, uint16_t a_AudioFormat, uint16_t a_NumChannels, uint32_t a_SampleRate, uint32_t a_ByteRate, uint16_t a_BlockAlign, uint16_t a_BitsPerSample)
{
    memcpy(m_WavFile.fmtChunk.chunkId, a_ChunkId, sizeof(m_WavFile.fmtChunk.chunkId));
    m_WavFile.fmtChunk.chunkSize = a_ChunkSize;
    m_WavFile.fmtChunk.audioFormat = a_AudioFormat;
    m_WavFile.fmtChunk.numChannels = a_NumChannels;
    m_WavFile.fmtChunk.sampleRate = a_SampleRate;
    m_WavFile.fmtChunk.byteRate = a_ByteRate;
    m_WavFile.fmtChunk.blockAlign = a_BlockAlign;
    m_WavFile.fmtChunk.bitsPerSample = a_BitsPerSample;

    m_WavFile.filledFmtChunk = true;

    logger::log_info("<Wav> (\"%s\") subchunk1Id: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.fmtChunk.chunkId[0], &m_WavFile.fmtChunk.chunkId[0] + std::size(m_WavFile.fmtChunk.chunkId)).c_str());
    logger::log_info("<Wav> (\"%s\") subchunk1Size: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.chunkSize);
    logger::log_info("<Wav> (\"%s\") audioFormat: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.audioFormat);
    if (m_WavFile.fmtChunk.audioFormat != WAVE_FORMAT_PCM)
    {
        logger::log_error("<Wav> (\"%s\") does not have the format 1 (PCM). This indicates there is probably some form of compression (%i).", m_SoundTitle.c_str(), m_WavFile.fmtChunk.audioFormat);
    }
    logger::log_info("<Wav> (\"%s\") numChannels: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.numChannels);
    logger::log_info("<Wav> (\"%s\") sampleRate: %ihz.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.sampleRate);
    if (m_WavFile.fmtChunk.sampleRate != 44100)
    {
        logger::log_error("<Wav> (\"%s\") does not have a 44100 hz sample rate (%ihz).", m_SoundTitle.c_str(), m_WavFile.fmtChunk.sampleRate);
    }
    logger::log_info("<Wav> (\"%s\") byteRate: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.byteRate);
    logger::log_info("<Wav> (\"%s\") blockAlign: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.blockAlign);
    logger::log_info("<Wav> (\"%s\") bitsPerSample: %i.", m_SoundTitle.c_str(), m_WavFile.fmtChunk.bitsPerSample);
}

/// <summary>
/// Sets the data chunk.
/// </summary>
/// <param name="a_ChunkId">The chunk id.</param>
/// <param name="a_ChunkSize">The chunk size.</param>
/// <param name="a_Data">The data.</param>
void WaveFile::SetDataChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, unsigned char* a_Data)
{
    memcpy(m_WavFile.dataChunk.chunkId, a_ChunkId, 4 * sizeof(unsigned char));
    m_WavFile.dataChunk.chunkSize = a_ChunkSize;

    // Actual data.
    m_WavFile.dataChunk.data = static_cast<unsigned char *>(malloc(sizeof(m_WavFile.dataChunk.data) * a_ChunkSize)); // Set aside sound buffer space.

    // TODO: Fix this.
	memcpy(m_WavFile.dataChunk.data, a_Data, sizeof(m_WavFile.dataChunk.data) * a_ChunkSize);

    m_WavFile.filledDataChunk = true;

    logger::log_info("<Wav> (\"%s\") subchunk2Id: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.dataChunk.chunkId[0], &m_WavFile.dataChunk.chunkId[0] + std::size(m_WavFile.dataChunk.chunkId)).c_str());
    logger::log_info("<Wav> (\"%s\") subchunk2Size: %i.", m_SoundTitle.c_str(), m_WavFile.dataChunk.chunkSize);
}

/// <summary>
/// Sets the acid chunk.
/// </summary>
/// <param name="a_ChunkId">The chunk id.</param>
/// <param name="a_ChunkSize">The chunk size.</param>
/// <param name="a_TypeOfFile">The type of file.</param>
/// <param name="a_RootNote">The root note.</param>
/// <param name="a_NumOfBeats">The number of beats.</param>
/// <param name="a_MeterDenominator">The meter denominator.</param>
/// <param name="a_MeterNumerator">The meter numerator.</param>
/// <param name="a_Tempo">The tempo.</param>
void WaveFile::SetAcidChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, uint32_t a_TypeOfFile, uint16_t a_RootNote, uint32_t a_NumOfBeats, uint16_t a_MeterDenominator, uint16_t a_MeterNumerator, float a_Tempo)
{
    memcpy(m_WavFile.acidChunk.chunkId, a_ChunkId, sizeof(m_WavFile.acidChunk.chunkId));
    m_WavFile.acidChunk.chunkSize = a_ChunkSize;
    m_WavFile.acidChunk.type_of_file = a_TypeOfFile;
    m_WavFile.acidChunk.root_note = a_RootNote;
    m_WavFile.acidChunk.num_of_beats = a_NumOfBeats;
    m_WavFile.acidChunk.meter_denominator = a_MeterDenominator;
    m_WavFile.acidChunk.meter_numerator = a_MeterNumerator;
    m_WavFile.acidChunk.tempo = a_Tempo;

    m_WavFile.filledAcidChunk = true;

    logger::log_info("<Wav> (\"%s\") subchunk3Id: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.acidChunk.chunkId[0], &m_WavFile.acidChunk.chunkId[0] + std::size(m_WavFile.acidChunk.chunkId)).c_str());
    logger::log_info("<Wav> (\"%s\") subchunk3Size: %i.", m_SoundTitle.c_str(), m_WavFile.acidChunk.chunkSize);
    logger::log_info("<Wav> (\"%s\") type_of_file: 0x%x.", m_SoundTitle.c_str(), m_WavFile.acidChunk.type_of_file);
    logger::log_info("<Wav> (\"%s\") root_note: 0x%x.", m_SoundTitle.c_str(), m_WavFile.acidChunk.root_note);
    logger::log_info("<Wav> (\"%s\") num_of_beats: %i.", m_SoundTitle.c_str(), m_WavFile.acidChunk.num_of_beats);
    logger::log_info("<Wav> (\"%s\") meter_denominator: %i.", m_SoundTitle.c_str(), m_WavFile.acidChunk.meter_denominator);
    logger::log_info("<Wav> (\"%s\") meter_numerator: %i.", m_SoundTitle.c_str(), m_WavFile.acidChunk.meter_numerator);
    logger::log_info("<Wav> (\"%s\") tempo: %fbpm.", m_SoundTitle.c_str(), m_WavFile.acidChunk.tempo);
}

/// <summary>
/// Sets the bext chunk.
/// </summary>
/// <param name="a_ChunkId">The chunk id.</param>
/// <param name="a_ChunkSize">The chunk size.</param>
/// <param name="a_Description">The description.</param>
/// <param name="a_Originator">The originator.</param>
/// <param name="a_OriginatorReference">The originator reference.</param>
/// <param name="a_OriginationDate">Creation date.</param>
/// <param name="a_OriginationTime">Creation time.</param>
/// <param name="a_TimeReferenceLow">The low time reference.</param>
/// <param name="a_TimeReferenceHigh">The high time reference.</param>
/// <param name="a_Version">The version.</param>
/// <param name="a_Umid">The umid.</param>
/// <param name="a_LoudnessValue">The loudness value.</param>
/// <param name="a_LoudnessRange">The loudness range.</param>
/// <param name="a_MaxTruePeakLevel">The max true peak level.</param>
/// <param name="a_MaxMomentaryLoudness">The max momentary loudness value.</param>
/// <param name="a_MaxShortTermLoudness">The max short term loudness value.</param>
/// <param name="a_Reserved">The reserved value.</param>
void WaveFile::SetBextChunk(unsigned char a_ChunkId[4], uint32_t a_ChunkSize, char a_Description[256], char a_Originator[32], char a_OriginatorReference[32], char a_OriginationDate[10], char a_OriginationTime[8], uint32_t a_TimeReferenceLow, uint32_t a_TimeReferenceHigh, uint16_t a_Version, unsigned char a_Umid[64], uint16_t a_LoudnessValue, uint16_t a_LoudnessRange, uint16_t a_MaxTruePeakLevel, uint16_t a_MaxMomentaryLoudness, uint16_t a_MaxShortTermLoudness, unsigned char a_Reserved[180])
{
    memcpy(m_WavFile.bextChunk.chunkId, a_ChunkId, sizeof(m_WavFile.bextChunk.chunkId));
    m_WavFile.bextChunk.chunkSize = a_ChunkSize;
    memcpy(m_WavFile.bextChunk.description, a_Description, sizeof(m_WavFile.bextChunk.description));
    memcpy(m_WavFile.bextChunk.originator, a_Originator, sizeof(m_WavFile.bextChunk.originator));
    memcpy(m_WavFile.bextChunk.originator_reference, a_OriginatorReference, sizeof(m_WavFile.bextChunk.originator_reference));
    memcpy(m_WavFile.bextChunk.origination_date, a_OriginationDate, sizeof(m_WavFile.bextChunk.origination_date));
    memcpy(m_WavFile.bextChunk.origination_time, a_OriginationTime, sizeof(m_WavFile.bextChunk.origination_time));
    for (char &j : m_WavFile.bextChunk.origination_time)
        if (j == '-')
            j = ':';
    m_WavFile.bextChunk.time_reference_low = a_TimeReferenceLow;
    m_WavFile.bextChunk.time_reference_high = a_TimeReferenceHigh;
    m_WavFile.bextChunk.version = a_Version;
    memcpy(m_WavFile.bextChunk.umid, a_Umid, sizeof(m_WavFile.bextChunk.umid));
    m_WavFile.bextChunk.loudness_value = a_LoudnessValue;
    m_WavFile.bextChunk.loudness_range = a_LoudnessRange;
    m_WavFile.bextChunk.max_true_peak_level = a_MaxTruePeakLevel;
    m_WavFile.bextChunk.max_momentary_loudness = a_MaxMomentaryLoudness;
    m_WavFile.bextChunk.max_short_term_loudness = a_MaxShortTermLoudness;
    memcpy(m_WavFile.bextChunk.reserved, a_Reserved, sizeof(m_WavFile.bextChunk.reserved));

    m_WavFile.filledBextChunk = true;

    logger::log_info("<Wav> (\"%s\") subchunk4Id: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.chunkId[0], &m_WavFile.bextChunk.chunkId[0] + std::size(m_WavFile.bextChunk.chunkId)).c_str());
    logger::log_info("<Wav> (\"%s\") subchunk4Size: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.chunkSize);
    logger::log_info("<Wav> (\"%s\") description: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.description[0], &m_WavFile.bextChunk.description[0] + std::size(m_WavFile.bextChunk.description)).c_str());
    logger::log_info("<Wav> (\"%s\") originator: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.originator[0], &m_WavFile.bextChunk.originator[0] + std::size(m_WavFile.bextChunk.originator)).c_str());
    logger::log_info("<Wav> (\"%s\") originator_reference: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.originator_reference[0], &m_WavFile.bextChunk.originator_reference[0] + std::size(m_WavFile.bextChunk.originator_reference)).c_str());
    logger::log_info("<Wav> (\"%s\") origination_date: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.origination_date[0], &m_WavFile.bextChunk.origination_date[0] + std::size(m_WavFile.bextChunk.origination_date)).c_str());
    logger::log_info("<Wav> (\"%s\") origination_time: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.origination_time[0], &m_WavFile.bextChunk.origination_time[0] + std::size(m_WavFile.bextChunk.origination_time)).c_str());
    logger::log_info("<Wav> (\"%s\") time_reference_low: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.time_reference_low);
    logger::log_info("<Wav> (\"%s\") time_reference_high: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.time_reference_high);
    logger::log_info("<Wav> (\"%s\") version: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.version);
    logger::log_info("<Wav> (\"%s\") umid: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.umid[0], &m_WavFile.bextChunk.umid[0] + std::size(m_WavFile.bextChunk.umid)).c_str());
    logger::log_info("<Wav> (\"%s\") loudness_value: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.loudness_value);
    logger::log_info("<Wav> (\"%s\") loudness_range: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.loudness_range);
    logger::log_info("<Wav> (\"%s\") max_true_peak_level: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.max_true_peak_level);
    logger::log_info("<Wav> (\"%s\") max_momentary_loudness: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.max_momentary_loudness);
    logger::log_info("<Wav> (\"%s\") max_short_term_loudness: %i.", m_SoundTitle.c_str(), m_WavFile.bextChunk.max_short_term_loudness);
    logger::log_info("<Wav> (\"%s\") reserved: %s.", m_SoundTitle.c_str(), std::string(&m_WavFile.bextChunk.reserved[0], &m_WavFile.bextChunk.reserved[0] + std::size(m_WavFile.bextChunk.reserved)).c_str());
}

/// <summary>
/// Converts 32 bit pcm data to 16 bit pcm data.
/// </summary>
void WaveFile::Convert32To16()
{
    m_WavFile.fmtChunk.bitsPerSample = 16;

    uint16_t *array_16 = wav::wav_converter::Convert32To16(m_WavFile.dataChunk.data, m_WavFile.dataChunk.chunkSize);
    free(m_WavFile.dataChunk.data);
    m_WavFile.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
}

/// <summary>
/// Converts 24 bit pcm data to 16 bit pcm data.
/// </summary>
void WaveFile::Convert24To16()
{
    m_WavFile.fmtChunk.bitsPerSample = 16;

    uint16_t *array_16 = wav::wav_converter::Convert24To16(m_WavFile.dataChunk.data, m_WavFile.dataChunk.chunkSize);
    free(m_WavFile.dataChunk.data);
    m_WavFile.dataChunk.data = reinterpret_cast<unsigned char *>(array_16);
}

WaveFile &WaveFile::operator=(const WaveFile &rhs)
{
    if (&rhs != this)
    {
        m_Looping = rhs.m_Looping;
        m_Volume = rhs.m_Volume;
        m_File = rhs.m_File;
        m_WavFile = rhs.m_WavFile;
        m_SoundTitle = rhs.m_SoundTitle;
    }
    return *this;
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
const WavFormat &WaveFile::GetWavFormat() const
{
    return m_WavFile;
}

/// <summary>
/// Returns the sound title.
/// </summary>
/// <returns></returns>
const char *WaveFile::GetSoundTitle() const
{
    return m_SoundTitle.c_str();
}