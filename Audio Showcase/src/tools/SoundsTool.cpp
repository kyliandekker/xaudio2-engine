#include "tools/SoundsTool.h"

#include "wave/WaveFile.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

SoundsTool::SoundsTool(uaudio::AudioSystem &a_AudioSystem, uaudio::SoundSystem &a_SoundSystem) : BaseTool(0, "Sounds", "Sounds"), m_AudioSystem(a_AudioSystem), m_SoundSystem(a_SoundSystem)
{
}

void SoundsTool::Render()
{
    for (auto hash : m_SoundSystem.GetSoundHashes())
        RenderSound(hash);
}

void SoundsTool::RenderSound(uaudio::Hash a_SoundHash)
{
    uaudio::WaveFile* a_WaveFile = m_SoundSystem.FindSound(a_SoundHash);
    ImGui::Text(a_WaveFile->GetSoundTitle());

    std::string play_button = std::string(PLAY) + " Play##Play_Sound_" + a_WaveFile->GetSoundTitle();
    if (ImGui::Button(play_button.c_str()))
    {
        m_AudioSystem.Play(*a_WaveFile);
    }
    std::string remove_sound = std::string(MINUS) + " Unload##Unload_Sound_" + a_WaveFile->GetSoundTitle();

    ImGui::SameLine();
    if (ImGui::Button(remove_sound.c_str()))
    {
        for (int32_t i = 0; i < m_AudioSystem.ChannelSize(); i++)
        {
            uaudio::xaudio2::XAudio2Channel* channel = m_AudioSystem.GetChannel(i);
            if (&channel->GetSound() == a_WaveFile)
            {
                channel->RemoveSound();
                channel->ResetPos();
                channel->Stop();
            }
        }
        m_SoundSystem.UnloadSound(a_SoundHash);
    }

    ImGui::SameLine();
    bool isLooping = a_WaveFile->IsLooping();
    std::string loop_button = std::string(RETRY) + "##Loop_Sound_" + std::to_string(a_SoundHash);
    if (ImGui::CheckboxButton(loop_button.c_str(), ImVec2(25, 25), &isLooping))
        a_WaveFile->SetLooping(isLooping);

    std::string extra_info = "Extra Info##ExtraInfo_Sound_" + std::to_string(a_SoundHash);
    if (ImGui::CollapsingHeader(extra_info.c_str()))
    {
        ImGui::Indent(IMGUI_INDENT);

        if (ImGui::CollapsingHeader(std::string(std::string("Deeper File Info###DeeperFileInfo_") + a_WaveFile->GetSoundTitle()).c_str()))
        {
            ImGui::Indent(IMGUI_INDENT);
            ShowValue("Block Align: ", std::string(std::to_string(a_WaveFile->GetWavFormat().fmtChunk.blockAlign)).c_str());
            ShowValue("Chunk Size: ", std::string(std::to_string(a_WaveFile->GetWavFormat().dataChunk.chunkSize)).c_str());
            ImGui::Unindent(IMGUI_INDENT);
        }
        ShowValue("Name: ", a_WaveFile->GetSoundTitle());
        ShowValue("Hash: ", std::to_string(a_SoundHash).c_str());
        ShowValue("Format: ", std::to_string(a_WaveFile->GetWavFormat().fmtChunk.audioFormat).c_str());
        ShowValue("Number Of Channels: ", std::string(std::to_string(a_WaveFile->GetWavFormat().fmtChunk.numChannels) + (a_WaveFile->GetWavFormat().fmtChunk.numChannels == 1 ? " (mono)" : " (stereo)")).c_str());
        ShowValue("Sample Rate: ", std::string(std::to_string(a_WaveFile->GetWavFormat().fmtChunk.sampleRate) + " Hz").c_str());
        ShowValue("Byte Rate: ", std::to_string(a_WaveFile->GetWavFormat().fmtChunk.byteRate).c_str());
        ShowValue("Bits Per Sample: ", std::to_string(a_WaveFile->GetWavFormat().fmtChunk.bitsPerSample).c_str());
        ShowValue("Duration (sec): ", std::string(std::to_string(uaudio::WaveFile::GetDuration(a_WaveFile->GetWavFormat().dataChunk.chunkSize, a_WaveFile->GetWavFormat().fmtChunk.byteRate)) + " secs").c_str());
        if (strcmp(std::string(&a_WaveFile->GetWavFormat().bextChunk.origination_date[0], &a_WaveFile->GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile->GetWavFormat().bextChunk.origination_date)).c_str(), "") != 0)
            ShowValue("Origination Date: ", std::string(&a_WaveFile->GetWavFormat().bextChunk.origination_date[0], &a_WaveFile->GetWavFormat().bextChunk.origination_date[0] + std::size(a_WaveFile->GetWavFormat().bextChunk.origination_date)).c_str());
        if (strcmp(std::string(&a_WaveFile->GetWavFormat().bextChunk.origination_time[0], &a_WaveFile->GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile->GetWavFormat().bextChunk.origination_time)).c_str(), "") != 0)
            ShowValue("Origination Time: ", std::string(&a_WaveFile->GetWavFormat().bextChunk.origination_time[0], &a_WaveFile->GetWavFormat().bextChunk.origination_time[0] + std::size(a_WaveFile->GetWavFormat().bextChunk.origination_time)).c_str());
        ShowValue("Duration: ", uaudio::WaveFile::FormatDuration(uaudio::WaveFile::GetDuration(a_WaveFile->GetWavFormat().dataChunk.chunkSize, a_WaveFile->GetWavFormat().fmtChunk.byteRate)).c_str());
        if (a_WaveFile->GetWavFormat().filledAcidChunk)
            ShowValue("Tempo: ", std::to_string(a_WaveFile->GetWavFormat().acidChunk.tempo).c_str());
        std::string sound_loop_text = "##Loop_Sound_" + std::string(a_WaveFile->GetSoundTitle());

        ImGui::Unindent(IMGUI_INDENT);
    }
    ImGui::Separator();
}