#include "tools/SoundsTool.h"

#include "wav/WaveFile.h"

#include <imgui/imgui.h>

SoundsTool::SoundsTool(uaudio::AudioSystem& a_AudioSystem, uaudio::SoundSystem& a_SoundSystem) : BaseTool(0, "Sounds", "Sounds"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
}

void SoundsTool::Render()
{
    for (auto* sound : m_SoundSystem.GetSounds())
        RenderSound(*sound);
}

void SoundsTool::RenderSound(uaudio::WaveFile& a_WaveFile)
{
    if (ImGui::CollapsingHeader(a_WaveFile.GetSoundTitle()))
    {
        ImGui::Indent(IMGUI_INDENT);

        if (ImGui::CollapsingHeader(std::string(std::string("File Info###FileInfo_") + a_WaveFile.GetSoundTitle()).c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);

            if (ImGui::CollapsingHeader(std::string(std::string("Deeper File Info###DeeperFileInfo_") + a_WaveFile.GetSoundTitle()).c_str()))
            {
                ImGui::Indent(IMGUI_INDENT);
                ShowValue("Block Align: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.blockAlign)).c_str());
                ShowValue("Format: ", std::string(std::to_string(a_WaveFile.GetWavFormat().dataChunk.chunkSize)).c_str());
                ImGui::Unindent(16.0f);
            }
            ShowValue("Name: ", a_WaveFile.GetSoundTitle());
            ShowValue("Format: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.audioFormat).c_str());
            ShowValue("Number Of Channels: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.numChannels) + (a_WaveFile.GetWavFormat().fmtChunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
            ShowValue("Sample Rate: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.sampleRate).c_str());
            ShowValue("Byte Rate: ", std::string(std::to_string(a_WaveFile.GetWavFormat().fmtChunk.byteRate) + " Hz").c_str());
            ShowValue("Bits Per Sample: ", std::to_string(a_WaveFile.GetWavFormat().fmtChunk.bitsPerSample).c_str());
            ShowValue("Duration (sec): ", std::string(std::to_string(uaudio::WaveFile::GetDuration(a_WaveFile.GetWavFormat().dataChunk.chunkSize, a_WaveFile.GetWavFormat().fmtChunk.byteRate)) + " secs").c_str());
            if (strcmp(std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_date[0], &a_WaveFile.GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_date)).c_str(), "") != 0)
                ShowValue("Origination Date: ", std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_date[0], &a_WaveFile.GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_date)).c_str());
            if (strcmp(std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_time[0], &a_WaveFile.GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_time)).c_str(), "") != 0)
                ShowValue("Origination Time: ", std::string(&a_WaveFile.GetWavFormat().bextChunk.origination_time[0], &a_WaveFile.GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile.GetWavFormat().bextChunk.origination_time)).c_str());
            ShowValue("Duration: ", uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_WaveFile.GetWavFormat().dataChunk.chunkSize, a_WaveFile.GetWavFormat().fmtChunk.byteRate)).c_str());
            if (a_WaveFile.GetWavFormat().filledAcidChunk)
                ShowValue("Tempo: ", std::to_string(a_WaveFile.GetWavFormat().acidChunk.tempo).c_str());

            std::string play_button = std::string(PLAY) + " Play##Play_Sound_" + a_WaveFile.GetSoundTitle();
            if (ImGui::Button(play_button.c_str()))
            {
                m_AudioSystem.Play(a_WaveFile);
            }
            std::string remove_sound = std::string(MINUS) + " Remove##Remove_Sound_" + a_WaveFile.GetSoundTitle();
            if (ImGui::Button(remove_sound.c_str()))
            {
                //m_Sounds.erase(m_Sounds.begin() + i);
                //m_Channels.erase(m_Channels.begin() + i);
            }

            ImGui::Unindent(IMGUI_INDENT);
        }
        ImGui::Unindent(IMGUI_INDENT);
    }
}