#define NOMINMAX
#include <xaudio2.h>
#include <algorithm>

#include "Audio/WaveFile.h"

#define LOG_INFO

WaveFile::WaveFile() = default;

WaveFile::WaveFile(const WaveFile & rhs)
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
    // Open the file. (use friend's file system later)
    fopen_s(&m_File, a_FilePath, "rb");
    if (!m_File)
    {
        std::string s = std::string("<Wav> Failed opening file: " + std::string(a_FilePath) + ".");
        LogError(s.c_str());
        return;
    }

    // Read what chunk id the file has.
    fread(&m_WavFile.chunkId, sizeof(m_WavFile.chunkId), 1, m_File);

    // Should be RIFF.
    if (strcmp(reinterpret_cast<const char*>(m_WavFile.chunkId), "RIFF") != 0)
    {
        std::string s = std::string("<Wav> File: " + std::string(a_FilePath) + " is not a RIFF file. (" + std::string(reinterpret_cast<char const*>(m_WavFile.chunkId)) + ")");
        LogError(s.c_str());
        return;
    }

    // Check chunk size (file size).
    fread(&m_WavFile.chunkSize, sizeof(m_WavFile.chunkSize), 1, m_File);

    // Format (should be WAVE).
    fread(&m_WavFile.format, sizeof(m_WavFile.format), 1, m_File);
    if (strcmp(reinterpret_cast<const char*>(m_WavFile.format), "WAVE") != 0)
    {
        std::string s = std::string("<Wav> File: " + std::string(a_FilePath) + " is not a WAV file. (" + std::string(reinterpret_cast<char const*>(m_WavFile.format)) + ")");
        LogError(s.c_str());
        return;
    }

    // Subchunk id (should be fmt).
    fread(&m_WavFile.subchunk1Id, sizeof(m_WavFile.subchunk1Id), 1, m_File);
    if (strcmp(reinterpret_cast<const char*>(m_WavFile.subchunk1Id), "fmt ") != 0)
    {
        std::string s = std::string("<Wav> File: " + std::string(a_FilePath) + " has no FMT chunk. (" + std::string(reinterpret_cast<char const*>(m_WavFile.subchunk1Id)) + ")");
        LogError(s.c_str());
        return;
    }

    // Subchunk size.
    fread(&m_WavFile.subchunk1Size, sizeof(m_WavFile.subchunk1Size), 1, m_File);

    // audio format (should be 1, other values indicate compression).
    fread(&m_WavFile.audioFormat, sizeof(m_WavFile.audioFormat), 1, m_File);

    // numChannels (mono is 1, stereo is 2).
    fread(&m_WavFile.numChannels, sizeof(m_WavFile.numChannels), 1, m_File);

    // sampleRate
    fread(&m_WavFile.sampleRate, sizeof(m_WavFile.sampleRate), 1, m_File);

    // byteRate
    fread(&m_WavFile.byteRate, sizeof(m_WavFile.byteRate), 1, m_File);

    // blockAlign
    fread(&m_WavFile.blockAlign, sizeof(m_WavFile.blockAlign), 1, m_File);

    // bitsPerSample
    fread(&m_WavFile.bitsPerSample, sizeof(m_WavFile.bitsPerSample), 1, m_File);

    // subchunk2Id
    fread(&m_WavFile.subchunk2Id, sizeof(m_WavFile.subchunk2Id), 1, m_File);

    // Data size.
    fread(&m_WavFile.subchunk2Size, sizeof(m_WavFile.subchunk2Size), 1, m_File);

    // Actual data.
    m_WavFile.data = static_cast<unsigned char*>(malloc(sizeof(m_WavFile.data) * m_WavFile.subchunk2Size)); // set aside sound buffer space
    fread(m_WavFile.data, sizeof(m_WavFile.data), m_WavFile.subchunk2Size, m_File); // read in our whole sound data chunk

    if (m_WavFile.bitsPerSample == 32)
        Convert32To16();
    else if (m_WavFile.bitsPerSample == 24)
        Convert24To16();

    m_WavFile.bufferSize = m_WavFile.bitsPerSample / 8;

    //for (uint32_t a = 0; a < m_WavFile.subchunk2Size; a++)
    //{
    //    printf("%hhu\n", m_WavFile.data[a]);
    //}

#ifdef LOG_INFO
    std::string s = std::string("(" + std::string(a_FilePath) + ") chunkId: " + std::string(&m_WavFile.chunkId[0], &m_WavFile.chunkId[0] + std::size(m_WavFile.chunkId)));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") chunkSize: " + std::to_string(m_WavFile.chunkSize));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") format: " + std::string(&m_WavFile.format[0], &m_WavFile.format[0] + std::size(m_WavFile.format)));
    LogInfo(s.c_str());
    if (m_WavFile.audioFormat != WAVE_FORMAT_PCM)
    {
        s = std::string("<Wav> File: " + std::string(a_FilePath) + " does not have the format 1 (PCM). This indicates there is probably some form of compression. (" + std::to_string(m_WavFile.audioFormat) + ")");
        LogInfo(s.c_str());
    }
    s = std::string("(" + std::string(a_FilePath) + ") subchunk1Id: " + std::string(&m_WavFile.subchunk1Id[0], &m_WavFile.subchunk1Id[0] + std::size(m_WavFile.subchunk1Id)));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") subchunk1Size: " + std::to_string(m_WavFile.subchunk1Size));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") audioFormat: " + std::to_string(m_WavFile.audioFormat));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") numChannels: " + std::to_string(m_WavFile.numChannels));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") sampleRate: " + std::to_string(m_WavFile.sampleRate));
    LogInfo(s.c_str());
    if (m_WavFile.sampleRate != 44100)
    {
        s = std::string("<Wav> File: " + std::string(a_FilePath) + " does not have a 44100 hz sample rate. (" + std::to_string(m_WavFile.sampleRate) + "hz )");
        LogInfo(s.c_str());
    }
    s = std::string("(" + std::string(a_FilePath) + ") byteRate: " + std::to_string(m_WavFile.byteRate));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") blockAlign: " + std::to_string(m_WavFile.blockAlign));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") bitsPerSample: " + std::to_string(m_WavFile.bitsPerSample));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") subchunk2Id: " + std::string(&m_WavFile.subchunk2Id[0], &m_WavFile.subchunk2Id[0] + std::size(m_WavFile.subchunk2Id)));
    LogInfo(s.c_str());
    s = std::string("(" + std::string(a_FilePath) + ") subchunk2Size: " + std::to_string(m_WavFile.subchunk2Size));
    LogInfo(s.c_str());
#endif
}

void WaveFile::Convert32To16()
{
    m_WavFile.bitsPerSample = 16;

    // Determine the size of a 16bit data array.
    // Chunksize divided by the size of a 32bit int (4) multiplied by the size of a 16bit int (2). 
    m_WavFile.subchunk2Size = m_WavFile.subchunk2Size / sizeof(uint32_t) * sizeof(uint16_t);
    uint16_t* array_16 = new uint16_t[m_WavFile.subchunk2Size];
    
    for (uint32_t a = 0; a < m_WavFile.subchunk2Size; a++)
    {
        // Skip 1 bit every time since we go from 32bit to 16bit.
        array_16[a] = *reinterpret_cast<uint16_t*>(m_WavFile.data + 2);

        // Add the size of a 32bit int (4) to move the data pointer.
        m_WavFile.data += sizeof(uint32_t);
    }
    m_WavFile.data = reinterpret_cast<unsigned char*>(array_16);
    m_WavFile.audioFormat = WAVE_FORMAT_PCM;
    m_WavFile.blockAlign = m_WavFile.numChannels * m_WavFile.bitsPerSample / 8;
    m_WavFile.byteRate = m_WavFile.sampleRate * m_WavFile.numChannels * m_WavFile.bitsPerSample / 8;
    m_WavFile.chunkSize = 138 + m_WavFile.subchunk2Size;
}

void WaveFile::Convert24To16()
{
    typedef struct { uint8_t byt[3]; } uint24_t;

    m_WavFile.bitsPerSample = 16;
    // Determine the size of a 16bit data array.
    // Chunksize divided by the size of a 24bit int (3) multiplied by the size of a 16bit int (2). 
    m_WavFile.subchunk2Size = m_WavFile.subchunk2Size / sizeof(uint24_t) * sizeof(uint16_t);
    uint16_t* array_16 = new uint16_t[m_WavFile.subchunk2Size];

    for (uint32_t a = 0; a < m_WavFile.subchunk2Size; a++)
    {
        // Skip 1 bit every time since we go from 24bit to 16bit.
        array_16[a] = *reinterpret_cast<uint16_t*>(m_WavFile.data + 1);

        // Add the size of a 24bit int (3) to move the data pointer.
        m_WavFile.data += sizeof(uint24_t);
    }
    m_WavFile.data = reinterpret_cast<unsigned char*>(array_16);
    m_WavFile.audioFormat = WAVE_FORMAT_PCM;
    m_WavFile.blockAlign = m_WavFile.numChannels * m_WavFile.bitsPerSample / 8;
    m_WavFile.byteRate = m_WavFile.sampleRate * m_WavFile.numChannels * m_WavFile.bitsPerSample / 8;
    m_WavFile.chunkSize = 138 + m_WavFile.subchunk2Size;
}

WaveFile::~WaveFile()
{
    // TODO: Delete data.
    //delete[] m_WavFile.data;
    if (m_File)
        fclose(m_File);
}

WaveFile& WaveFile::operator=(const WaveFile & rhs)
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
/// Returns the duration of a FFT in ms/block.
/// </summary>
/// <param name="a_BlockLength"></param>
/// <returns></returns>
float WaveFile::GetFFTDuration(int a_BlockLength) const
{
    return a_BlockLength / (m_WavFile.sampleRate / 1000);
}

/// <summary>
/// Returns the frequency resolution of a FFT in Hz.
/// </summary>
/// <param name="a_BlockLength"></param>
/// <returns></returns>
float WaveFile::GetFFTFrequencyResolution(int a_BlockLength) const
{
    return (1 / GetFFTDuration(a_BlockLength) * 1000);
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
    if (a_StartingPoint + a_ElementCount >= m_WavFile.subchunk2Size)
    {
        int newsize = a_ElementCount - ((a_StartingPoint + a_ElementCount) - m_WavFile.subchunk2Size);
        a_ElementCount = newsize;
    }
    a_Buffer = m_WavFile.data + a_StartingPoint;
}

/// <summary>
/// Returns the duration in seconds.
/// </summary>
/// <returns></returns>
float WaveFile::GetDuration() const
{
    return static_cast<float>(m_WavFile.subchunk2Size) / static_cast<float>(m_WavFile.byteRate);
}

/// <summary>
/// Returns the duration in minute:seconds.
/// </summary>
/// <param name="a_Duration"></param>
/// <returns></returns>
std::string WaveFile::FormatDuration(float a_Duration)
{
	const unsigned int hours = static_cast<int>(a_Duration) / 3600;
	const unsigned int minutes = (static_cast<int>(a_Duration) - (hours * 3600)) / 60;
	const unsigned int seconds = static_cast<int>(a_Duration) % 60;
	const unsigned int total = (hours * 3600) + (minutes * 60) + seconds;
	const float milisecondsfloat = a_Duration - static_cast<float>(total);
    const unsigned int miliseconds = static_cast<int>(milisecondsfloat * 1000);

    char hoursstring[32], minutesstring[32], secondsstring[32], milisecondsstring[32];
    sprintf_s(hoursstring, "%02d", hours);
    sprintf_s(minutesstring, "%02d", minutes);
    sprintf_s(secondsstring, "%02d", seconds);
    sprintf_s(milisecondsstring, "%03d", miliseconds);
    return 
        std::string(hoursstring) +
        ":" +
        std::string(minutesstring) +
        ":" + 
        std::string(secondsstring) +
        ":" + 
        std::string(milisecondsstring);
}

/// <summary>
/// Checks if a starting point is at or above the end of the file.
/// </summary>
/// <param name="a_StartingPoint"></param>
/// <returns></returns>
bool WaveFile::IsEndOfFile(uint32_t a_StartingPoint) const
{
    return a_StartingPoint >= m_WavFile.subchunk2Size;
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

bool WaveFile::GetChunk(std::vector<std::complex<double>>& signal)
{
    signal.clear();

    int startIndex = std::max((m_prevEnd - 128), 0);

    int endIndex = startIndex + 256;

    m_prevEnd = endIndex;

    int byteStep = m_WavFile.numChannels * m_WavFile.byteRate;

    int byteStartIndex = startIndex * byteStep;

    int byteEndIndex = endIndex * byteStep;

    int currentIndex = byteStartIndex;
    short int currentSample;
    while((currentIndex < byteEndIndex) && (currentIndex < (m_WavFile.subchunk2Size - 1)))
    {
	    if (m_WavFile.byteRate > 1)
	    {
            currentSample = (m_WavFile.data[currentIndex] << 8) + m_WavFile.data[currentIndex];
	    }
        else
        {
            currentSample = m_WavFile.data[currentIndex];
        }

        std::complex<double> signalSample(static_cast<double>(currentSample), 0.0);

        signal.push_back(signalSample);

        currentIndex += byteStep;
    }

    if (currentIndex >= m_WavFile.subchunk2Size)
        return false;
	return true;
}

/// <summary>
/// Returns the sound title.
/// </summary>
/// <returns></returns>
const char* WaveFile::GetSoundTitle() const
{
    return m_SoundTitle.c_str();
}

void WaveFile::LogInfo(const char* a_Info)
{
    printf("%s\n", a_Info);
}

void WaveFile::LogError(const char* a_Error)
{
    printf("%s\n", a_Error);
}